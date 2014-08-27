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

#define STACK_SZ CONFIG_STACK_SIZE
#define EXCEPTION_STACK_SZ CONFIG_STACK_SIZE

/*
 * The stacks for each of the armv8 cores grows down from _estack. It is sized
 * according to MAX_CPUS. Additionally provide exception stacks for each CPU.
 */
.section .bss, "aw", @nobits
.global _stack
.global _estack
.balign STACK_SZ
_stack:
.space CONFIG_MAX_CPUS*STACK_SZ
_estack:

.global _stack_exceptions
.global _estack_exceptions
.balign EXCEPTION_STACK_SZ
_stack_exceptions:
.space CONFIG_MAX_CPUS*EXCEPTION_STACK_SZ
_estack_exceptions:

ENTRY(cpu_get_stack)
	mov	x1, #STACK_SZ
	mul	x0, x0, x1
	ldr	x1, 1f
	sub	x0, x1, x0
	ret
.align 3
1:
	.quad	_estack
ENDPROC(cpu_get_stack)

ENTRY(cpu_get_exception_stack)
	mov	x1, #EXCEPTION_STACK_SZ
	mul	x0, x0, x1
	ldr	x1, 1f
	sub	x0, x1, x0
	ret
.align 3
1:
	.quad	_estack_exceptions
ENDPROC(cpu_get_exception_stack)

/*
 * Boot strap the processor into a C environemnt. That consists of providing
 * 16-byte aligned stack. The programming enviroment uses SP_EL0 as its main
 * stack while keeping SP_ELx reserved for exception entry.
 */
ENTRY(arm64_c_environment)
	bl	smp_processor_id	/* x0 = cpu */
	mov	x24, x0


	/* Set the exception stack for this cpu. */
	bl	cpu_get_exception_stack
	msr	SPSel, #1
	isb
	mov	sp, x0

	/* Have stack pointer use SP_EL0. */
	msr	SPSel, #0
	isb

	/* Set stack for this cpu. */
	mov	x0, x24		/* x0 = cpu */
	bl	cpu_get_stack
	mov	sp, x0

	b	arm64_init
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
