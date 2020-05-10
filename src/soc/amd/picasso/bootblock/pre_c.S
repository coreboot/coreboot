/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/post_code.h>

/*
 * on entry:
 *   mm0: BIST (ignored)
 *   mm2_mm1: timestamp at bootblock_protected_mode_entry
 */

.global bootblock_pre_c_entry
bootblock_pre_c_entry:
	post_code(0xa0)

	movl	$_eearlyram_stack, %esp

	/* Align the stack and keep aligned for call to bootblock_c_entry() */
	and	$0xfffffff0, %esp
	sub	$8, %esp

	movd	%mm2, %eax
	pushl	%eax		/* tsc[63:32] */
	movd	%mm1, %eax
	pushl	%eax		/* tsc[31:0] */

	post_code(0xa2)

	call	bootblock_c_entry
	/* Never reached */

.halt_forever:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.halt_forever
