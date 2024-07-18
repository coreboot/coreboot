/* SPDX-License-Identifier: GPL-2.0-only */
/* Functions to call MRC.bin from x86_64 */

.text
.code32
	.section ".text.mrc_wrapper", "ax", @progbits
	.globl mrc_wrapper
mrc_wrapper:
	/* Set up new stack frame */
	pushal
	mov	%esp, %ebp

	/* Align stack */
	andl	$0xfffffff0, %esp

	movl	36(%ebp), %ebx	/* Get function pointer */
	movl	40(%ebp), %eax	/* Get argument */

	call	*%ebx

	/* Place return value on stack so that popal fetches it */
	movl	%eax, 28(%ebp)

	/* Restore stack pointer */
	mov	%ebp, %esp
	popal
	ret

	/*
	 * Callback for MRC to print a character on the console.
	 * As MRC is x86_32 call into long mode and use the x86_64
	 * function do_putchar to print to console.
	 */

#include <cpu/x86/64bit/prot2long.inc>

prot2lm_wrapper do_putchar
