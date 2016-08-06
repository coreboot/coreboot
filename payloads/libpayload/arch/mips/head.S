/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cpu.h>

	/* Disable interrupts and mark the kernel mode */
	.macro	setup_c0_status clr
	.set	push
	mfc0	$t0, $CP0_STATUS
	or	$t0, ST0_CU0 | 0x1f | \clr
	xor	$t0, 0x1f | \clr
	mtc0	$t0, $CP0_STATUS
	.set	noreorder
	sll	$zero, 3
	.set	pop
	.endm

	/* Don't reorder instructions */
	.set noreorder

	.align 4

	.global cb_header_ptr
cb_header_ptr:
	.word 0

	.global old_sp
old_sp:
	.word 0


	.global _entry, _leave
	.text

/* Our entry point */
_entry:

	/*
	 * This function saves off the previous stack and switches us to our
	 * own execution environment.
	 */

	/* Clear watch and cause registers */
	mtc0	$zero, $CP0_WATCHLO
	mtc0	$zero, $CP0_WATCHHI
	mtc0	$zero, $CP0_CAUSE

	/* Disable interrupts */
	setup_c0_status 0

	/* Don't use at in synthetic instr. */
	.set noat

	/* Init timer */
	mtc0	$zero, $CP0_COUNT
	mtc0	$zero, $CP0_COMPARE

	/* Initialize $gp */
	bal	1f
	nop
	.word	_gp
1:
	lw	$gp, 0($ra)

	/* Save off the location of the coreboot tables */
	la	$at, cb_header_ptr
	sw	$a0, 0x00($at)

	/* Save old stack pointer */
	la	$at, old_sp
	sw	$sp, 0x00($at)

	/* Setup new stack */
	la	$sp, _stack

	/* Let's rock */
	la	$a2, start_main
	jalr	$a2
	nop
_leave:
	/* Restore old stack. */
	lw	$sp, old_sp
	/* Return to the original context. */
	eret
