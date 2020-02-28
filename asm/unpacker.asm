global xor_cipher

section .text

woody:
	pushfq
	push	rdi
	push	rsi
	push	rdx
	push	r8
	push	r9
	push	r10

xor_cipher_params:
	lea	rdi, [rel woody_str_end + 2]
	mov	rsi, 0xBBBBBBBBBBBBBBBB
	lea	rdx, [rel woody - 0x22222201]
	mov	rcx, 0xCCCCCCCCCCCCCCCC
	call	xor_cipher

print_woody:
	mov	rax, 1
	mov	rdi, 1
	lea	rsi, [rel woody_str]
	mov	rdx, woody_str_end - woody_str - 1
	syscall

exit:
	pop	r10
	pop	r9
	pop	r8
	pop	rdx
	pop	rsi
	pop	rdi
	popfq

	jmp 0xAAAAAAAE



; void xor_cipher(char *key, size_t key_size, void *text, size_t text_size)
xor_cipher:
	cmp	rsi, 0
	je	xor_cipher_end

	xor	r8, r8		; counter
	mov	r9, rdx		; address of .text
	mov	r10, rcx	; size of .text

loop:
	cmp	r8, r10
	je	xor_cipher_end

	xor	rdx, rdx	; clear dividend
	mov	rax, r8		; dividend
	mov	rcx, rsi	; divisor
	div	rcx, 		; rax = /, rdx = %

	lea	rax, [rel rdi + rdx]
	mov	dl, byte[rax]
	xor	byte[r9], dl

	inc	r8
	inc	r9
	jmp	loop

xor_cipher_end:
	ret



align 8

woody_str	db "....WOODY....", 0x0a, 0
woody_str_end	db 0x0
