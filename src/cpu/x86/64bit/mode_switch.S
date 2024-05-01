/* SPDX-License-Identifier: GPL-2.0-only */
#include <cpu/x86/64bit/entry64.inc>

/* Calls a x86_32 function from x86_64 context */
.text
.code64
	.section ".text.protected_mode_call", "ax", @progbits
	.globl protected_mode_call_wrapper
protected_mode_call_wrapper:
	/* Preserve registers */
	push	%rbp
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	/* Backup gs to stack */
	movl	%gs, %eax
	push	%rax

	/* Backup IDT to stack */
	sub	$16, %rsp
	sidt	(%rsp)

	/* Store stack pointer */
	mov	%rsp, %rbp

	/* Backup cr3 to stack */
	mov	%cr3, %rax
	push	%rax

	/* New IDT to stack */
	pushq	$0
	pushq	$0

	/* Align stack */
	movabs	$0xfffffffffffffff0, %rax
	andq	%rax, %rsp

	/* Make room for arguments on stack */
	sub	$16, %rsp

	/* Arguments to stack */
	movl	%edi, 12(%rsp)
	movl	%esi, 0(%rsp)
	movl	%edx, 4(%rsp)
	movl	%ecx, 8(%rsp)

	/* Drop to protected mode */
	#include <cpu/x86/64bit/exit32.inc>

	/* Load zero IDT. x86_32 FSP doesn't like to find a x86_64 IDT */
	lidt	-24(%ebp)

	/* Fetch function to call */
	movl	12(%esp), %ebx

	/* Call function */
	call	*%ebx
	movl	%eax, %ebx

	/*
	 * Back to long mode by using cr3 previously stored on stack.
	 * Preserves ebx.
	 */
	setup_longmode -8(%ebp)

	/* Place return value in rax */
	movl	%ebx, %eax

	/* Restore stack pointer */
	mov	%rbp, %rsp

	/* Restore IDT */
	lidt	(%rsp)
	add	$16, %rsp

	/* Restore registers */
	pop	%rbx
	movl	%ebx, %gs
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbx
	pop	%rbp

	ret
