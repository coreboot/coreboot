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
 */

/*
 * ======================== stage_entry.S =====================================
 * This file acts as an entry point to the different stages of arm64. They share
 * the same process of setting up stacks and jumping to c code. It is important
 * to save x25 from corruption as it contains the argument for rmodule.
 * =============================================================================
 */

#include <arch/asm.h>
#define __ASSEMBLY__
#include <arch/lib_helpers.h>

#define STACK_SZ CONFIG_STACK_SIZE
#define EXCEPTION_STACK_SZ CONFIG_STACK_SIZE

/*
 * Stack for armv8 CPU grows down from _estack. Additionally, provide exception
 * stack for the CPU.
 */
.section .bss, "aw", @nobits

.global _stack
.global _estack
.balign STACK_SZ
_stack:
.space STACK_SZ
_estack:

.global _stack_exceptions
.global _estack_exceptions
.balign EXCEPTION_STACK_SZ
_stack_exceptions:
.space EXCEPTION_STACK_SZ
_estack_exceptions:

ENTRY(cpu_get_stack)
	ldr	x0, 1f
	ret
.align 3
1:
	.quad	_estack
ENDPROC(cpu_get_stack)

ENTRY(cpu_get_exception_stack)
	ldr	x0, 1f
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
/*
 * IMPORTANT: Ensure x25 is not corrupted because it saves the argument to
 * any rmodules.
 */
ENTRY(arm64_c_environment)
	/* Set the exception stack for the cpu. */
	bl	cpu_get_exception_stack
	msr	SPSel, #1
	isb
	mov	sp, x0

	/* Have stack pointer use SP_EL0. */
	msr	SPSel, #0
	isb

	/* Set the non-exception stack for the cpu. */
	bl	cpu_get_stack
	mov	sp, x0

	/* Get entry point by dereferencing c_entry. */
	ldr	x1, 1f
	/* Move back the arguments from x25 to x0 */
	mov     x0, x25
	br	x1
.align 3
	1:
	.quad	c_entry
ENDPROC(arm64_c_environment)

ENTRY(_start)
	/* Save any arguments to current rmodule in x25 */
	mov	x25, x0
	b	arm64_c_environment
ENDPROC(_start)

/*
 * Setup SCTLR so that:
 * Little endian mode is setup, XN is not enforced, MMU and caches are disabled.
 * Alignment and stack alignment checks are disabled.
 */
.macro setup_sctlr
	read_current x0, sctlr
	bic	x0, x0, #(1 << 25)	/* Little Endian */
	bic	x0, x0, #(1 << 19)	/* XN not enforced */
	bic	x0, x0, #(1 << 12)	/* Disable Instruction Cache */
	bic	x0, x0, #0xf		/* Clear SA, C, A and M */
	write_current sctlr, x0, x1
.endm

CPU_RESET_ENTRY(arm64_cpu_startup)
	bl arm64_cpu_early_setup
	setup_sctlr
	b	arm64_c_environment
ENDPROC(arm64_cpu_startup)

/*
 * stage_entry is defined as a weak symbol to allow SoCs/CPUs to define a custom
 * entry point to perform any fixups that need to be done immediately after
 * power on reset. In case SoC/CPU does not need any custom-defined entrypoint,
 * this weak symbol can be used to jump directly to arm64_cpu_startup.
 */
ENTRY_WEAK(stage_entry)
	b	arm64_cpu_startup
ENDPROC(stage_entry)
