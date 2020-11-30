/* SPDX-License-Identifier: GPL-2.0-only */

.text
.code64
	.section ".text.protected_mode_call", "ax", @progbits
	.globl protected_mode_call_narg
protected_mode_call_narg:

	push	%rbp
	mov	%rsp, %rbp
	/* Preserve registers */
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	/* Arguments to stack */
	push	%rdi
	push	%rsi
	push	%rdx
	push	%rcx

	#include <cpu/x86/64bit/exit32.inc>

	movl	-48(%ebp), %eax	/* Argument count */
	movl	-64(%ebp), %edx	/* Argument 0 */
	movl	-72(%ebp), %ecx	/* Argument 1 */

	/* Align the stack */
	andl	$0xFFFFFFF0, %esp
	test	%eax, %eax
	je	1f	/* Zero arguments */

	subl	$1, %eax
	test	%eax, %eax
	je	2f	/* One argument */

	/* Two arguments */
	subl	$8, %esp
	pushl	%ecx	/* Argument 1 */
	pushl	%edx	/* Argument 0 */
	jmp	1f
2:
	subl	$12, %esp
	pushl	%edx	/* Argument 0 */

1:
	movl	-56(%ebp), %ebx	/* Function to call */
	call	*%ebx
	movl	%eax, %ebx

	/* Preserves ebx */
	#include <cpu/x86/64bit/entry64.inc>

	/* Place return value in rax */
	movl	%ebx, %eax

	/* Restore registers */
	mov	-40(%rbp), %r15
	mov	-32(%rbp), %r14
	mov	-24(%rbp), %r13
	mov	-16(%rbp), %r12
	mov	-8(%rbp), %rbx

	/* Restore stack pointer */
	mov	%rbp, %rsp
	pop	%rbp

	ret
