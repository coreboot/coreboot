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

ENTRY(__wait_for_spin_table_request)
	/* Entry here is in EL2 with the magic address in x0. */
	mov	x28, x0
1:
	ldr	x27, [x28]
	cmp	x27, xzr
	b.ne	2f
	wfe
	b	1b
2:
	/* Entry into the kernel. */
	mov	x0, xzr
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
	br	x27
ENDPROC(__wait_for_spin_table_request)
