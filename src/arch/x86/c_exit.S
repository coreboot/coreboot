/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ram_segs.h>
#include <cpu/x86/cr.h>

#if ENV_X86_64

	/*
	 * Functions to handle mode switches from long mode to protected
	 * mode.
	 */
.text
.code64
	.section ".text.protected_mode_jump", "ax", @progbits
	.globl protected_mode_jump
protected_mode_jump:

	push %rbp
	mov %rsp, %rbp

	/* Arguments to stack */
	push	%rdi
	push	%rsi

	#include <cpu/x86/64bit/exit32.inc>

	movl	-8(%ebp), %eax	/* Function to call */
	movl	-16(%ebp), %ebx	/* Argument 0 */

	/* Align the stack */
	andl	$0xFFFFFFF0, %esp
	subl	$12, %esp
	pushl	%ebx	/* Argument 0 */

	jmp	*%eax
#endif
