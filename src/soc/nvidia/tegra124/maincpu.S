/* SPDX-License-Identifier: BSD-3-Clause */

#include <arch/asm.h>

.arm
ENTRY(maincpu_setup)
	/*
	 * Set the CPU to System mode with IRQ and FIQ disabled. Prefetch/Data
	 * aborts may happen early and crash before the abort handlers are
	 * installed, but at least the problem will show up near the code that
	 * causes it.
	 */
	msr	cpsr_cxf, #0xdf

	ldr	sp, maincpu_stack_pointer
	eor	lr, lr
	ldr	r0, maincpu_entry_point
	bx	r0
ENDPROC(maincpu_setup)

	.align 2

	.global maincpu_stack_pointer
maincpu_stack_pointer:
	.word 0

	.global maincpu_entry_point
maincpu_entry_point:
	.word 0
