/*
    freebios/src/northbridge/nsc/gx1/bridge_setup.inc

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Setup the northbridge of a National Semiconductor GX1 CPU.
*/

northbridge_setup:
	leal	northbridge_setup_table, %esi
	movl	$northbridge_setup_len, %ecx
	movl	$0x80000000, %ebx	/* Host Bridge Config Space */

northbridge_setup_loop:
	movb	(%esi), %bl
	incl	%esi
	movb	(%esi), %al
	incl	%esi

	movw	$0xcf8, %dx		/* PCI Write Config Byte */
	xchgl	%ebx, %eax
	outl	%eax, %dx
	movw	%ax, %dx
	andw	$3, %dx
	orw	$0xcfc, %dx
	xchgl	%ebx, %eax
	outb	%al, %dx

	loop	northbridge_setup_loop

	jmp	northbridge_setup_end

northbridge_setup_table:
	/* I belive this optimizes for X-bus performance */
	.byte	0x40, 0x1e
	.byte	0x41, 0x52
	.byte	0x43, 0xc1		/* PCI arbitration 1 */
	.byte	0x44, 0x00		/* PCI arbitration 2 */
northbridge_setup_len = (.-northbridge_setup_table)/2

northbridge_setup_end:
	nop

/*
    Local variables:
        compile-command: "make -C /export/bios/voyager2"
    End:
*/
