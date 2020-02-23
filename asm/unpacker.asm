global unpacker

extern puts

section .text

unpacker:
	push	rbp
	mov	rbp, rsp


print:
	mov	rdi, woody_str
	call puts

exit:
	mov	rsp, rbp
	pop	rbp
	ret

woody_str db "....WOODY....", 0
