/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Calls a x86_64 function from x86_32 context.
 * Must not be directly invoked from C code!
 */

#include <cpu/x86/64bit/entry64.inc>

.text
.code32
	.section ".text.long_mode_call_3arg", "ax", @progbits
	.global long_mode_call_3arg
long_mode_call_3arg:

	/* Function to call is passed in EAX. */

	/* Backup registers */
	pushal

	/* Backup stack pointer */
	mov	%esp, %ebp

	/* Enter long mode, preserves ebx */
	setup_longmode $PML4E

	/* Align stack */
	movabs	$0xfffffffffffffff0, %rax
	andq	%rax, %rsp

	movl	28(%rbp), %ebx	/* Function to call */
	movl	36(%rbp), %edi	/* 1st arg */
	movl	40(%rbp), %esi	/* 2nd arg */
	movl	44(%rbp), %edx	/* 3rd arg */

	call	*%rbx

	/* Store return value on stack. popal will fetch it. */
	mov	%eax, 28(%rbp)
	shr	$32, %rax
	movl	%eax, 24(%rbp)

	#include <cpu/x86/64bit/exit32.inc>

	/* Restore stack pointer */
	mov	%ebp, %esp

	/* Restore registers */
	popal

	ret
