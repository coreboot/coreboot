/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Stack layout after pushing all the registers.
 * +------------+
 * |  ret addr  | <-- esp + 0x78
 * +------------+
 * |    rax     | <-- esp + 0x70
 * +------------+
 * |    rcx     | <-- esp + 0x68
 * +------------+
 * |    rdx     | <-- esp + 0x60
 * +------------+
 * |    rbx     | <-- esp + 0x58
 * +------------+
 * |    rbp     | <-- esp + 0x50
 * +------------+
 * |    rsi     | <-- esp + 0x48
 * +------------+
 * |    rdi     | <-- esp + 0x40
 * +------------+
 * |    r8      | <-- esp + 0x38
 * +------------+
 * |    r9      | <-- esp + 0x30
 * +------------+
 * |    r10     | <-- esp + 0x28
 * +------------+
 * |    r11     | <-- esp + 0x20
 * +------------+
 * |    r12     | <-- esp + 0x18
 * +------------+
 * |    r13     | <-- esp + 0x10
 * +------------+
 * |    r14     | <-- esp + 0x08
 * +------------+
 * |    r15     | <-- esp + 0x00
 * +------------+
 */
.global switch_to_thread
switch_to_thread:
	/* Backup registers. */
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx

	push	%rbp
	push	%rsi
	push	%rdi

	push	%r8
	push	%r9
	push	%r10
	push	%r11
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	/* Save the current stack */
	mov	%rsp, (%rsi)

	/* Switch to the new stack. */
	mov	%rdi, %rsp

	/* Restore registers. */
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%r11
	pop	%r10
	pop	%r9
	pop	%r8

	pop	%rdi
	pop	%rsi
	pop	%rbp

	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax

	/* Done. */
	ret
