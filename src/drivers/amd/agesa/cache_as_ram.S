/* SPDX-License-Identifier: GPL-2.0-only */

/******************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * $Workfile:: cache_as_ram.S
 *
 * Description: cache_as_ram.S - AGESA Module Entry Point for GCC compiler
 *
 ******************************************************************************
 */

#include <cpu/x86/lapic_def.h>
#include <cpu/x86/post_code.h>
#include <amdblocks/post_codes.h>

.section .init

.code32

.global bootblock_pre_c_entry

_cache_as_ram_setup:

#include "gcccar.inc"

/*
 * on entry:
 *   mm0: BIST (ignored)
 *   mm2_mm1: timestamp
 */
bootblock_pre_c_entry:

	post_code(POST_BOOTBLOCK_PRE_C_ENTRY)

	AMD_ENABLE_STACK

	/*
	 * Set up bootblock stack on BSP.
	 * AMD_ENABLE_STACK macro sets up a stack for BSP at BSP_STACK_BASE_ADDR
	 * which is 0x30000 (_car_region_end), but for C bootblock the stack
	 * begins at _ecar_stack (see arch/x86/car.ld)
	 */
	mov	$LAPIC_BASE_MSR, %ecx
	rdmsr
	test	$LAPIC_BASE_MSR_BOOTSTRAP_PROCESSOR, %eax
	jz	ap_entry

	mov	$_ecar_stack, %esp

	/* Align the stack and keep aligned for call to bootblock_c_entry() */
	and	$0xfffffff0, %esp
	sub	$8, %esp

	movd	%mm2, %eax
	pushl	%eax		/* tsc[63:32] */
	movd	%mm1, %eax
	pushl	%eax		/* tsc[31:0] */

	post_code(POST_BOOTBLOCK_PRE_C_DONE)

	call	bootblock_c_entry

	/* Never reached. */

stop:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	stop

ap_entry:
	/* Align the stack for call to ap_bootblock_c_entry() */
	and	$0xfffffff0, %esp
	call	ap_bootblock_c_entry

	/* Never reached. */
	jmp	stop

_cache_as_ram_setup_end:
