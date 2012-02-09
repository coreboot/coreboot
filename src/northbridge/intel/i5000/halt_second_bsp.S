	/* Save BIST result */

	movl	%eax, %ebp

	/* Read the semaphore register of i5000 (BOFL0).
	   If it returns zero, it means there was already
	   another read by another CPU */

	movl	$0x800080c0, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx

	addw	$4, %dx
	inl	%dx, %eax
	cmp	$0, %eax
	jne	1f

	/* degrade BSP to AP */
	mov	$0x1b, %ecx
	rdmsr
	andl $(~0x100), %eax
	wrmsr

	cli
loop:	hlt
	jmp	loop

1:	/* Restore BIST */
	mov	%ebp, %eax
