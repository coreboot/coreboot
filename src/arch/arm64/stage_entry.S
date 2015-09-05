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
 * Foundation, Inc.
 */

/*
 * ======================== stage_entry.S =====================================
 * This file acts as an entry point to the different stages of arm64 as well as
 * for the secure monitor. They share the same process of setting up stacks and
 * jumping to c code. It is important to save x25 from corruption as it contains
 * the argument for secure monitor.
 * =============================================================================
 */

#include <arch/asm.h>
#define __ASSEMBLY__
#include <arch/lib_helpers.h>
#include <arch/startup.h>

#define STACK_SZ CONFIG_STACK_SIZE
#define EXCEPTION_STACK_SZ CONFIG_STACK_SIZE

/*
 * The stacks for each of the armv8 cores grows down from _estack. It is sized
 * according to MAX_CPUS. Additionally provide exception stacks for each CPU.
 */
.section .bss, "aw", @nobits

.global _arm64_startup_data
.balign 8
_arm64_startup_data:
.space NUM_ELEMENTS*PER_ELEMENT_SIZE_BYTES

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
/*
 * IMPORTANT: Ensure x25 is not corrupted because it saves the argument to
 * secmon
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

	/* Get entry point by dereferencing c_entry. */
	ldr	x1, 1f
	/* Retrieve entry in c_entry array using  x26 as the index. */
	adds	x1, x1, x26, lsl #3
	ldr	x1, [x1]
	/* Move back the arguments from x25 to x0 */
	mov     x0, x25
	br	x1
.align 3
	1:
	.quad	c_entry
ENDPROC(arm64_c_environment)

/* The first 2 instructions are for BSP and secondary CPUs,
 * respectively. x26 holds the index into c_entry array. */
.macro split_bsp_path
	b	2000f
	b	2001f
	2000:
	mov	x26, #0
	b	2002f
	2001:
	mov	x26, #1
	2002:
.endm

ENTRY(_start)
	split_bsp_path
	/* Save the arguments to secmon in x25 */
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

/*
 * This macro assumes x2 has base address and returns value read in x0
 * x1 is used as temporary register.
 */
.macro get_element_addr index
	add x1, x2, #(\index * PER_ELEMENT_SIZE_BYTES)
	ldr x0, [x1]
.endm

/*
 * Uses following registers:
 * x0 = reading stored value
 * x1 = temp reg
 * x2 = base address of saved data region
 */
.macro startup_restore
	adr x2, _arm64_startup_data

	get_element_addr MAIR_INDEX
	write_current mair, x0, x1

	get_element_addr TCR_INDEX
	write_current tcr, x0, x1

	get_element_addr TTBR0_INDEX
	write_current ttbr0, x0, x1

	get_element_addr SCR_INDEX
	write_el3 scr, x0, x1

	get_element_addr VBAR_INDEX
	write_current vbar, x0, x1

	get_element_addr CNTFRQ_INDEX
	write_el0 cntfrq, x0, x1

	get_element_addr CPTR_INDEX
	write_el3 cptr, x0, x1

	get_element_addr CPACR_INDEX
	write_el1 cpacr, x0, x1

	dsb sy
	isb

	tlbiall_current x1
	read_current x0, sctlr
	orr	x0, x0, #(1 << 12)	/* Enable Instruction Cache */
	orr	x0, x0, #(1 << 2)	/* Enable Data/Unified Cache */
	orr	x0, x0, #(1 << 0)	/* Enable MMU */
	write_current sctlr, x0, x1

	dsb sy
	isb
.endm

CPU_RESET_ENTRY(arm64_cpu_startup)
	split_bsp_path
	bl arm64_cpu_early_setup
	setup_sctlr
	b	arm64_c_environment
ENDPROC(arm64_cpu_startup)

CPU_RESET_ENTRY(arm64_cpu_startup_resume)
	split_bsp_path
	bl arm64_cpu_early_setup
	setup_sctlr
	startup_restore
	b	arm64_c_environment
ENDPROC(arm64_cpu_startup_resume)

/*
 * stage_entry is defined as a weak symbol to allow SoCs/CPUs to define a custom
 * entry point to perform any fixups that need to be done immediately after
 * power on reset. In case SoC/CPU does not need any custom-defined entrypoint,
 * this weak symbol can be used to jump directly to arm64_cpu_startup.
 */
ENTRY_WEAK(stage_entry)
	b	arm64_cpu_startup
ENDPROC(stage_entry)
