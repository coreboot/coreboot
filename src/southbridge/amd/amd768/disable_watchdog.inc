#define PM_DEV_FN (AMD768_DEV + 0x300)
	/* Disable the watchdog timer */
	movl	$(0x80000000 | PM_DEV_FN | 0x49), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfd, %dx
	inb	%dx, %al
	orb	$0x04, %al
	outb	%al, %dx
	movl	$(0x80000000 | PM_DEV_FN | 0x41), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfd, %dx
	inb	%dx, %al
	orb	$0x40, %al
	outb	%al, %dx
	/* added from sourceforge */
        movl    $(0x80000000 | PM_DEV_FN | 0x48), %eax
        movw    $0xcf8, %dx
        outl    %eax, %dx
        movw    $0xcfd, %dx
        inb     %dx, %al
        orb     $0x8, %al
        outb    %al, %dx


