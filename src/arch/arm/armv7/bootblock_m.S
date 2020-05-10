/* SPDX-License-Identifier: BSD-3-Clause */

#include <arch/asm.h>

ENTRY(_start)
	/*
	 * Initialize the stack to a known value. This is used to check for
	 * stack overflow later in the boot process.
	 */
	ldr	r0, =_stack
	ldr	r1, =_estack
	ldr	r2, =0xdeadbeef
init_stack_loop:
	str	r2, [r0]
	add	r0, #4
	cmp	r0, r1
	bne	init_stack_loop

call_bootblock:
	ldr	sp, =_estack /* Set up stack pointer */
	bl	main
ENDPROC(_start)
