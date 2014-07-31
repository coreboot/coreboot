/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <arch/asm.h>

ENTRY(seed_stack)
	/*
	 * Initialize the stack to a known value. This is used to check for
	 * stack overflow later in the boot process.
	 */
	ldr	x0, .stack_bottom
	ldr	x1, .stack_top
	ldr	x2, =0xdeadbeefdeadbeef
	ldr	x3, =0x8

init_stack_loop:
	str	x2, [x0]
	add	x0, x0, x3
	cmp	x0, x1
	bne	init_stack_loop

load_stack:
	b	arm64_init
	.align	4
	.stack_bottom:
		.quad _stack
ENDPROC(seed_stack)

ENTRY(arm64_el3_startup)
	/* Set all bits in SCTLR_EL3 to 0 except RES1 and RES0 */
	mrs     x0, SCTLR_EL3
	ldr     x1, .SCTLR_MASK
	and     x0, x0, x1
	msr	SCTLR_EL3, x0
	/* Set all bits in SCR_EL3 to 0 except RES1 and RES0 */
	mrs     x0, SCR_EL3
	ldr     x1, .SCR_MASK
	and     x0, x0, x1
	msr     SCR_EL3, x0

	/* Have stack pointer use SP_EL0. */
	msr	SPSel, #0
	isb

	/* Load up the stack if non-zero. */
	ldr	x0, .stack_top
	cmp	x0, #0
	b.eq	1f
	mov	sp, x0
	1:

	ldr	x1, .entry
	br	x1

	.align 4
	.SCTLR_MASK:
		.quad 0x0FFFFEFF0

	.SCR_MASK:
		.quad 0x0FFFFC070
	.align 4
	/*
	 * By default branch to main() and initialize the stack according
	 * to the Kconfig option for cpu0. However, this code can be relocated
	 * and reused to start up secondary cpus.
	 */
	.stack_top:
		.quad _estack
	.entry:
		.quad seed_stack
ENDPROC(arm64_el3_startup)
.global arm64_el3_startup_end
arm64_el3_startup_end:

ENTRY(stage_entry)
	b	arm64_el3_startup
ENDPROC(stage_entry)
