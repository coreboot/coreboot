/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

.set	noreorder	/* Prevent assembler from "optimizing" this code. */

.section ".text._start", "ax", %progbits
.globl _start
_start:
	/* Set the stack pointer */
	la	$sp, _estack

	/*
	 * Initialise the stack to a known value, used later to check for
	 * overflow.
	 */
	la	$t0, _stack
	addi	$t1, $sp, -4
	li	$t2, 0xdeadbeef
1:	sw	$t2, 0($t0)
	bne	$t0, $t1, 1b
	addi	$t0, $t0, 4

	/* Run main */
	b	main

	/*
	 * Should never return from main. Make sure there is no branch in the
	 * branch delay slot.
	 */
2:	nop
	b	2b
	nop	/* Make sure there is no branch after this either. */
