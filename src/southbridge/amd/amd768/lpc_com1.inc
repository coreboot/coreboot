	/* enable LPC superio on the AMD 768 south bridge */
#if 0
	movl	$(0x80000000 | AMD768_DEV | 0x54), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inb	%dx, %al
	orb	$0x30, %al
	outb	%al, %dx

	/* enable LPC serial port 0x3f8 */
	movl	$(0x80000000 | AMD768_DEV | 0x50), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfe, %dx
	movb	$0x8, %al
	outb	%al, %dx
#endif
