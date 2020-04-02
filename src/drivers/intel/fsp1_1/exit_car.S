/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */


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
