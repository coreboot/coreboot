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

#if CONFIG_ARM64_CPUS_START_IN_EL3
#define SCTLR_ELx sctlr_el3
#elif CONFIG_ARM64_CPUS_START_IN_EL2
#define SCTLR_ELx sctlr_el2
#elif CONFIG_ARM64_CPUS_START_IN_EL1
#define SCTLR_ELx sctlr_el1
#else
#error Need to know what ELx processor starts up in.
#endif

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

/*
 * Boot strap the processor into a C environemnt. That consists of providing
 * 16-byte aligned stack. The programming enviroment uses SP_EL0 as its main
 * stack while keeping SP_ELx reserved for exception entry.
 */
ENTRY(arm64_c_environment)
	ldr	x0, .exception_stack_top
	cmp	x0, #0
	b.eq	2f
	msr	SPSel, #1
	isb

	mov	sp, x0

	2:
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
	.exception_stack_top:
		.quad CONFIG_EXCEPTION_STACK_TOP
	.stack_top:
		.quad _estack
	.entry:
		.quad seed_stack
ENDPROC(arm64_c_environment)

CPU_RESET_ENTRY(arm64_cpu_startup)
	mrs	x0, SCTLR_ELx
	bic	x0, x0, #(1 << 25)	/* Little Endian */
	bic	x0, x0, #(1 << 19)	/* XN not enforced */
	bic	x0, x0, #(1 << 12)	/* Disable Instruction Cache */
	bic	x0, x0, #0xf		/* Clear SA, C, A, and M */
	msr	SCTLR_ELx, x0
	isb
	b	arm64_c_environment
ENDPROC(arm64_cpu_startup)

ENTRY(stage_entry)
	b	arm64_cpu_startup
ENDPROC(stage_entry)
