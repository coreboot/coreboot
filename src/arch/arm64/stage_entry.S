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
	ldr	x1, [x1]
	/* Move back the arguments from x25 to x0 */
	mov     x0, x25
	br	x1
.align 3
	1:
	.quad	c_entry
ENDPROC(arm64_c_environment)

ENTRY(__rmodule_entry)
	/* Save the arguments to secmon in x25 */
	mov    x25, x0
	b      arm64_c_environment
ENDPROC(__rmodule_entry)

CPU_RESET_ENTRY(arm64_cpu_startup)
        read_current x0, sctlr
	bic	x0, x0, #(1 << 25)	/* Little Endian */
	bic	x0, x0, #(1 << 19)	/* XN not enforced */
	bic	x0, x0, #(1 << 12)	/* Disable Instruction Cache */
	bic	x0, x0, #0xf		/* Clear SA, C, A, and M */
	write_current sctlr, x0, x1
	isb
	b	arm64_c_environment
ENDPROC(arm64_cpu_startup)

ENTRY(stage_entry)
	b	arm64_cpu_startup
ENDPROC(stage_entry)
