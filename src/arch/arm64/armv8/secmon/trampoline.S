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
 * Foundation, Inc.
 */

#include <arch/asm.h>

/*
 * Call entry(arg) after reinitializing stack state.
 * void secmon_trampoline(void *entry, void *arg);
 */
ENTRY(secmon_trampoline)
	mov	x22, x0			/* x22 = function pointer */
	mov	x23, x1			/* x23 = argument */
	bl	smp_processor_id	/* x0 = cpu */
	mov	x24, x0

	/* Set the exception stack for this cpu. */
	bl	cpu_get_exception_stack
	msr	SPSel, #1
	isb
	mov	sp, x0

	/* Have stack pointer use SP_EL0. */
	msr	SPSel, #0
	isb

	/* Set stack for this cpu. */
	mov	x0, x24		/* x0 = cpu */
	bl	cpu_get_stack
	mov	sp, x0

	/* Call the function with specified argument. */
	mov	x1, x22
	mov	x0, x23
	br	x1
ENDPROC(secmon_trampoline)
