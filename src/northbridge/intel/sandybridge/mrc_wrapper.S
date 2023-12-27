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
