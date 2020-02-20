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


.text
.global chipset_teardown_car
chipset_teardown_car:

	pop	%ebx
	/* Move the stack pointer to real RAM */
	movl	post_car_stack_top, %esp
	/* Align the stack 16 bytes */
	andl	$0xfffffff0, %esp

	call	chipset_teardown_car_main

	jmp	*%ebx
