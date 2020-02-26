global woody

section .text

woody:
	pushfq
	push rdi
	push rsi
	push rdx
	push r8
	push r9
	push r10

	mov	rax, 1
	mov	rdi, 1
	lea	rsi, [rel woody_str]
	mov	rdx, woody_str_end - woody_str - 1
	syscall

	pop r10
	pop r9
	pop r8
	pop rdx
	pop rsi
	pop rdi
	popfq

	jmp 0xAAAAAAAE


align 8

woody_str	db "....WOODY....", 0x0a, 0
woody_str_end	db 0x0
