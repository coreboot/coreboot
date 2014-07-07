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

/* Macro to initialize stack, perform seeding if required and finally call the
 * function provided
 * @stack : Stack address
 * @size  : Stack size
 * @seed  : Stack seeding required (1=yes/otherwise=no)
 * @func  : Function to call after initializing stack
 */
.macro stack_init stack, size, seed, func
        /* Check if stack seeding is required */
	mov	r0, #\seed
	cmp     r0, #1
	bne     call_func
	/* Stack seeding */
	ldr     r0, \stack
	ldr	r1, \size
	sub	r0, r0, r1
	ldr	r1, \stack
	ldr	r2, =0xdeadbeef
init_stack_loop:
	str	r2, [r0]
	add	r0, #4
	cmp	r0, r1
	bne	init_stack_loop

call_func:
	ldr	sp, \stack /* Set up stack pointer */
	bl	\func
.endm
