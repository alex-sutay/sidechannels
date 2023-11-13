#include "Enclave_t.h"

char* DATA;


void marshall_data(char* data, int size) {
	// This function moves the data into the PRM to simulate like it was code
	DATA = data;
	return;
}

void access_data() {
	// This function accesses the data marshalled in earlier
	char throw_away = *DATA;
	return;
}
