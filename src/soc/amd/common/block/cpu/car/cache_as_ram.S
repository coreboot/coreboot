/* SPDX-License-Identifier: GPL-2.0-only */

/******************************************************************************
 * $Workfile:: cache_as_ram.S
 *
 * Description: CAR setup called from bootblock_crt0.S.
 *
 ******************************************************************************
 */

#include <cpu/x86/post_code.h>

.section .init

.code32

_cache_as_ram_setup:

#include "gcccar.inc"

/*
 * on entry:
 *   mm0: BIST (ignored)
 *   mm2_mm1: timestamp
 */

.global bootblock_pre_c_entry
bootblock_pre_c_entry:

	post_code(0xa0)

	AMD_ENABLE_STACK

	/* Align the stack and keep aligned for call to bootblock_c_entry() */
	and	$0xfffffff0, %esp
	sub	$8, %esp

	movd	%mm2, %eax
	pushl	%eax		/* tsc[63:32] */
	movd	%mm1, %eax
	pushl	%eax		/* tsc[31:0] */

before_carstage:
	post_code(0xa2)

	call	bootblock_c_entry
	/* Never reached */

.halt_forever:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.halt_forever

_cache_as_ram_setup_end:
