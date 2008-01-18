/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 1999-2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <ppc_asm.tmpl>

	.globl	ppc_init_float_registers
    
ppc_init_float_registers:
	lfd	fr0, 0(r3)
	lfd	fr1, 0(r3)
	lfd	fr2, 0(r3)
	lfd	fr3, 0(r3)
	lfd	fr4, 0(r3)
	lfd	fr5, 0(r3)
	lfd	fr6, 0(r3)
	lfd	fr7, 0(r3)
	lfd	fr8, 0(r3)
	lfd	fr9, 0(r3)
	lfd	fr10, 0(r3)
	lfd	fr11, 0(r3)
	lfd	fr12, 0(r3)
	lfd	fr13, 0(r3)
	lfd	fr14, 0(r3)
	lfd	fr15, 0(r3)
	lfd	fr16, 0(r3)
	lfd	fr17, 0(r3)
	lfd	fr18, 0(r3)
	lfd	fr19, 0(r3)
	lfd	fr20, 0(r3)
	lfd	fr21, 0(r3)
	lfd	fr22, 0(r3)
	lfd	fr23, 0(r3)
	lfd	fr24, 0(r3)
	lfd	fr25, 0(r3)
	lfd	fr26, 0(r3)
	lfd	fr27, 0(r3)
	lfd	fr28, 0(r3)
	lfd	fr29, 0(r3)
	lfd	fr30, 0(r3)
	lfd	fr31, 0(r3)
	blr

	.end

