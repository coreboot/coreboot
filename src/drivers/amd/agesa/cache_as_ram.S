/*
 * This file is part of the coreboot project.
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

/******************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * $Workfile:: cache_as_ram.S
 *
 * Description: cache_as_ram.S - AGESA Module Entry Point for GCC complier
 *
 ******************************************************************************
 */

#include "gcccar.inc"
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/post_code.h>

.code32
.globl _cache_as_ram_setup, _cache_as_ram_setup_end

_cache_as_ram_setup:

	/* Preserve BIST. */
	movd	%eax, %mm0

	post_code(0xa0)

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

	/* Align the stack. */
	and	$0xFFFFFFF0, %esp

	/* Must maintain 16-byte stack alignment here. */
	pushl	$0x0
	pushl	$0x0
	pushl	$0x0
	movd	%mm0, %eax		/* bist */
	pushl	%eax
	call	romstage_main

	/* Never reached. */

stop:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	stop

ap_entry:
	/* Align the stack for call to ap_romstage_main() */
	and	$0xfffffff0, %esp
	call	ap_romstage_main

	/* Never reached. */
	jmp	stop

_cache_as_ram_setup_end:
