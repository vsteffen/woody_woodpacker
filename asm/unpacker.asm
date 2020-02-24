global woody

extern puts

section .text

woody:
	push	rbp
	mov	rax, 1
	mov	rdi, 1
	lea	rsi, [rel woody_str]
	mov	rdx, woody_str_end - woody_str
	syscall
	pop	rbp

	mov rax, 0xAAAAAAAAAAAAAAAA
	jmp rax


align 8

woody_str	db "....WOODY....", 0x0a, 0
woody_str_end	db 0x0
