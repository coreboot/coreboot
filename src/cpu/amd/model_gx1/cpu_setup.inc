/*
    freebios/src/northbridge/nsc/gx1/cpu_setup.inc

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Initialize the GX1 CPU configuration registers
*/

/* USES: esi, ecx, eax */

#include <cpu/amd/gx1def.h>

	movl %eax, %ebp /* preserve bist */

cpu_setup_start:
	leal	cpu_setup_table, %esi
	movl	$cpu_setup_len, %ecx

cpu_setup_loop:
	movw	(%esi), %ax
	addl	$2, %esi
	outb	%al, $0x22
	movb	%ah, %al
	outb	%al, $0x23
	loop	cpu_setup_loop

	movb	$0xff, %al		/* DIR1 -- Identification Register 1 */
	outb	%al, $0x22
	inb	$0x23, %al
	cmpb	$0x63, %al		/* Revision for GXLV rev 3 */
	jbe	cpu_no_ccr4

	movb	$0xe8, %al		/* CCR4 */
	outb	%al, $0x22
	inb	$0x23, %al
	orb	$0x20, %al		/* Enable FPU Fast Mode */
	outb	%al, $0x23

	movb	$0xf0, %al		/* PCR1 --- Performace Control  */
	outb	%al, $0x22
	inb	$0x23, %al
	orb	$0x02, %al		/* Incrementor on, whatever that is */
	outb	%al, $0x23

	movb	$0x20, %al		/* PCR0 --- Performace Control  */
	outb	%al, $0x22
	inb	$0x23, %al
	orb	$0x20, %al		/* Must be 1 according to data book */
	orb	$0x04, %al		/* Incrementor Margin 10 */
	outb	%al, $0x23
cpu_no_ccr4:

	jmp	cpu_setup_end

cpu_setup_table:
	.byte	0xc1, 0x00		/* NO SMIs */
	.byte	0xc3, 0x14		/* Enable CPU config register */
	.byte	0x20, 0x00
	.byte	0xb8, GX_BASE>>30	/* Enable GXBASE address */
	.byte	0xc2, 0x00
	.byte	0xe8, 0x98
	.byte	0xc3, 0xf8		/* Enable CPU config register */
cpu_setup_len = (.-cpu_setup_table)/2

cpu_setup_end:
	nop

	movl %ebp, %eax	/* Restore bist */
