/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define WAKEUP_BASE		0x600
#define RELOCATED(x)	(x - __wakeup + WAKEUP_BASE)

/* CR0 bits */
#define PE		(1 << 0)

#ifdef __x86_64__
	.code64
#else
	.code32
#endif

	.globl __wakeup
__wakeup:
#ifdef __x86_64__
	xor	%rax,%rax
	mov	%ss, %ax
	push	%rax
	mov	%rsp, %rax
	add	$8, %rax
	push	%rax
	pushfq
	push	$0x10
	lea	3(%rip), %rax
	push	%rax
	iretq

	.code32

	/* disable paging */
	mov	%cr0, %eax
	btc	$31, %eax
	mov	%eax, %cr0

	/* disable long mode */
	mov	$0xC0000080, %ecx
	rdmsr
	btc	$8, %eax
	wrmsr
#endif
	/* First prepare the jmp to the resume vector */
	mov	0x4(%esp), %eax	/* vector */
	/* last 4 bits of linear addr are taken as offset */
	andw	$0x0f, %ax
	movw	%ax, (__wakeup_offset)
	mov	0x4(%esp), %eax
	/* the rest is taken as segment */
	shr	$4, %eax
	movw	%ax, (__wakeup_segment)

	/* Activate the right segment descriptor real mode. */
	ljmp	$0x28, $RELOCATED(1f)
1:
.code16
	/* 16 bit code from here on... */

	/* Load the segment registers w/ properly configured
	 * segment descriptors. They will retain these
	 * configurations (limits, writability, etc.) once
	 * protected mode is turned off.
	 */
	mov	$0x30, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss

	/* Turn off protection */
	movl	%cr0, %eax
	andl	$~PE, %eax
	movl	%eax, %cr0

	/* Now really going into real mode */
	ljmp	$0, $RELOCATED(1f)
1:
	movw	$0x0, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* This is a FAR JMP to the OS waking vector. The C code changed
	 * the address to be correct.
	 */
	.byte 0xea

__wakeup_offset = RELOCATED(.)
	.word 0x0000

__wakeup_segment = RELOCATED(.)
	.word 0x0000

	.globl __wakeup_size
__wakeup_size:
	.long . - __wakeup
