#include <stdio.h>
#include <iostream>
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "sgx_utils/sgx_utils.h"
#include<fcntl.h>
#include<sys/mman.h>
#include<unistd.h>
#include<stdlib.h>
#include<x86intrin.h>

#define PAGE_SIZE 4096
#define READ_SIZE 4096
#define DELAY 100000000

#define SHARED_FILE "/bin/sudo"
#define CLOCK_FILE "/bin/whoami"

void* mmap_file(char* filename){
        int fd;
        void* result;

        // Setup fd of shared_file and mmap
        fd = open(filename, O_RDONLY);
        // mmap
        result = mmap(NULL,PAGE_SIZE,PROT_READ,MAP_SHARED,fd,0);
        close(fd);

        return result;
}

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

// OCall implementations
void ocall_print(const char* str) {
    printf("%s\n", str);
}

int main(int argc, char const *argv[]) {
    if (initialize_enclave(&global_eid, "enclave.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return 1;
    }
    /*
    int ptr;
    sgx_status_t status = generate_random_number(global_eid, &ptr);
    std::cout << status << std::endl;
    if (status != SGX_SUCCESS) {
        std::cout << "noob" << std::endl;
    }
    printf("Random number: %d\n", ptr);

    // Seal the random number
    size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(ptr);
    uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);

    sgx_status_t ecall_status;
    status = seal(global_eid, &ecall_status,
            (uint8_t*)&ptr, sizeof(ptr),
            (sgx_sealed_data_t*)sealed_data, sealed_size);

    if (!is_ecall_successful(status, "Sealing failed :(", ecall_status)) {
        return 1;
    }

    int unsealed;
    status = unseal(global_eid, &ecall_status,
            (sgx_sealed_data_t*)sealed_data, sealed_size,
            (uint8_t*)&unsealed, sizeof(unsealed));

    if (!is_ecall_successful(status, "Unsealing failed :(", ecall_status)) {
        return 1;
    }

    std::cout << "Seal round trip success! Receive back " << unsealed << std::endl;
	*/

        int fd, i, j, amount_read;
        unsigned long long t;
        void *shared_mapaddr, *clock_mapaddr;
        volatile int throw_away;
        char buffer[READ_SIZE];
        unsigned char c, clock_on;

        if(argc!=2){
                printf("Usage: %s data_file\n", argv[0]);
                return 1;
        }

        shared_mapaddr = mmap_file(SHARED_FILE);
	// move the data into the enclave
	marshall_data(global_eid, (char*)shared_mapaddr, READ_SIZE);
        clock_mapaddr = mmap_file(CLOCK_FILE);

        // Open data_file
        fd = open(argv[1], O_RDONLY);

        clock_on = 0xF;

        usleep(10000); // Sleep before starting

        // Read until we read zero
        while( (amount_read = read(fd, buffer, READ_SIZE)) ){
                // Run through amount_read bytes
                for(i = 0; i<amount_read; i++){
                        // Get byte
                        c = buffer[i];
                        // Transmit bit by bit
                        for(j = 0; j<8; j++){
                                t = __rdtsc();
                                do{
                                        if(clock_on){
                                                throw_away = *((char*)clock_mapaddr);
                                        }
                                        if(c & 0x80){
						// Do the access inside the enclave
                                                access_data(global_eid);
                                        }
                                }while((__rdtsc()-t)<DELAY);
                                // Next bit from data
                                c = c << 1;
                                // Flip clock
                                if(clock_on) clock_on = 0;
                                else clock_on = 0xF;
                        }
                }
        }
    return 0;
}
