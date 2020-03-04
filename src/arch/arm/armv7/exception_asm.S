/* SPDX-License-Identifier: BSD-3-Clause */
/* This file is part of the coreboot project. */

	.text

	.align 6
	.arm
	.global exception_table
exception_table:
	b	1f
	b	2f
	b	3f
	b	4f
	b	5f
	b	6f
	b	7f
	b	8f

1:
	ldr	sp, _not_used
	b	exception_common
2:
	ldr	sp, _undefined_instruction
	b	exception_common
3:
	ldr	sp, _software_interrupt
	b	exception_common
4:
	ldr	sp, _prefetch_abort
	b	exception_common
5:
	ldr	sp, _data_abort
	b	exception_common
6:
	ldr	sp, _not_used
	b	exception_common
7:
	ldr	sp, _irq
	b	exception_common
8:
	ldr	sp, _fiq
	b	exception_common

exception_common:
	str	sp, exception_handler
	ldr	sp, exception_stack_end
	push	{ lr }
	stmfd	sp, { sp, lr }^
	sub	sp, sp, $8
	push	{ r0 - r12 }
	mov	r0, sp
	mov	lr, pc
	ldr	pc, exception_handler
	pop	{ r0 - r12 }
	add	sp, sp, $8
	ldmfd	sp!, { pc }^


	.align 2
_undefined_instruction:	.word exception_undefined_instruction
_software_interrupt:	.word exception_software_interrupt
_prefetch_abort:	.word exception_prefetch_abort
_data_abort:		.word exception_data_abort
_not_used:		.word exception_not_used
_irq:			.word exception_irq
_fiq:			.word exception_fiq

	.global exception_stack_end
exception_stack_end:
	.word 0

exception_handler:
	.word 0

	.thumb
	.global set_vbar
	.thumb_func
set_vbar:
	mcr	p15, 0, r0, c12, c0, 0
	bx	lr
