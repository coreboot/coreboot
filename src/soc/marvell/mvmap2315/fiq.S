/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <arch/asm.h>

ENTRY(fiq_stack_init)

	mrs	r0, CPSR
	mov	r1, r0
	bic	r1, r1, #0x40
	orr	r0, r0, #0x51
	and	r0, r0, #0xFFFFFFF1
	msr	CPSR_c, r0
	ldr	sp, =_efiq_stack
	orr	r0, r0, #0x10
	and	r0, r0, #0xFFFFFFB0
	msr	CPSR_cf, r1

	dsb
	isb

	bx	lr

ENDPROC(fiq_stack_init)
