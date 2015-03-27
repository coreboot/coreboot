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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

.set	noreorder	/* Prevent assembler from "optimizing" this code. */

.section ".start", "ax", %progbits
.globl _start
_start:
	/* Set the stack pointer */
	li	$sp, CONFIG_BOOTBLOCK_STACK_TOP

	/*
	 * Initialise the stack to a known value, used later to check for
	 * overflow.
	 */
	li	$t0, CONFIG_BOOTBLOCK_STACK_BOTTOM
	addi	$t1, $sp, -4
	li	$t2, 0xdeadbeef
1:	sw	$t2, 0($t0)
	bne	$t0, $t1, 1b
	addi	$t0, $t0, 4

	/* Run main */
	b	main

	/* Should never return from main. */
2:
	b	2b
