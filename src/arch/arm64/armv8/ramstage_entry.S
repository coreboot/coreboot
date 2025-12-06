/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Ramstage entrypoint code for aarch64 (a.k.a. armv8)
 */

#include <arch/asm.h>

ENTRY(_start)
	/*
	 * Note: x0 must be preserved as it contains the CBMEM top pointer
	 * for the next stage entry.
	 */
	ldr	x10, =_preram_stack
	ldr	x11, =_postram_stack

	cmp	x10, x11
	beq	stack_init_done

	/* Initialize stack with sentinel value to later check overflow. */
	ldr	x9, =0xdeadbeefdeadbeef
	ldr	x10, =_epostram_stack
1:
	stp	x9, x9, [x11], #16
	cmp	x11, x10
	bne	1b

	/* Update stack pointer with DRAM mapped stack base */
	/* Leave a line of beef dead for easier visibility in stack dumps. */
	sub	sp, x11, #16

stack_init_done:
	/* Call the C function. */
	bl	stage_entry
ENDPROC(_start)
