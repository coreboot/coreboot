#define PM_DEV_FN (AMD766_DEV + 0x300)
	/* Disable the watchdog timer */
	movl	$(0x80000000 | PM_DEV_FN | 0x40), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfd, %dx
	inb	%dx, %al
	orb	$0x44, %al
	outb	%al, %dx

