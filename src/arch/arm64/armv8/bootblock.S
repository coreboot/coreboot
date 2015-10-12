/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/asm.h>

ENTRY(_start)
	/* Initialize PSTATE, SCTLR and caches to clean state. */
	bl	arm64_init_cpu

	/* Initialize stack with sentinel value to later check overflow. */
	ldr	x0, =_stack
	ldr	x1, =_estack
	ldr	x2, =0xdeadbeefdeadbeef
stack_init_loop:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	bne	stack_init_loop

	/* Leave a line of beef dead for easier visibility in stack dumps. */
	sub	sp, x0, #16

	bl	main
ENDPROC(_start)
