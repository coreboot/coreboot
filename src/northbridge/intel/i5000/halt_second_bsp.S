	/* Save BIST result */

	movl	%eax, %ebp

	/* check if SPAD0 is cleared. If yes, it means this was a hard reset */
	movl	$0x800080d0, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx

	addw	$4, %dx
	inl	%dx, %eax
	cmp	$0, %eax
	je	no_reset

	/* perform hard reset */
	movw	$0xcf9, %dx
	movb	$0x06, %al
	outb	%al, %dx

loop0:	hlt
	jmp	loop0

no_reset:

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

1:	/* set magic value for soft reset detection */
	movl    $0x800080d0, %eax
	movw    $0xcf8, %dx
	outl    %eax, %dx

	addw    $4, %dx
	movl	$0x12345678, %eax
	outl	%eax, %dx

	/* Restore BIST */
	mov	%ebp, %eax
