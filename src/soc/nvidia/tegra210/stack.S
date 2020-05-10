/* SPDX-License-Identifier: GPL-2.0-only */

/* Macro to initialize stack, perform seeding if required and finally call the
 * function provided
 * @stack_top    : First address above the stack
 * @stack_bottom : Lowest address on the stack
 * @seed         : Stack seeding required (1=yes/otherwise=no)
 * @func         : Function to call after initializing stack
 */
.macro stack_init stack_top, stack_bottom, seed, func
	/* Check if stack seeding is required */
	mov	r0, #\seed
	cmp     r0, #1
	bne     call_func
	/* Stack seeding */
	ldr     r0, =\stack_bottom
	ldr	r1, =\stack_top
	ldr	r2, =0xdeadbeef
init_stack_loop:
	str	r2, [r0]
	add	r0, #4
	cmp	r0, r1
	bne	init_stack_loop

call_func:
	ldr	sp, =\stack_top /* Set up stack pointer */
	bl	\func
.endm
