/*
 * Based on arch/arm/include/asm/cacheflush.h
 *
 * Copyright (C) 1999-2002 Russell King.
 * Copyright (C) 2012 ARM Ltd.
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
 * Foundation, Inc.
 */

#include <arch/asm.h>
#include <arch/cache_helpers.h>

/*
 * Bring an ARMv8 processor we just gained control of (e.g. from IROM) into a
 * known state regarding caches/SCTLR. Completely cleans and invalidates
 * icache/dcache, disables MMU and dcache (if active), and enables unaligned
 * accesses, icache and branch prediction (if inactive). Clobbers x4 and x5.
 */
ENTRY(arm_init_caches)
	/* w4: SCTLR, return address: x8 (stay valid for the whole function) */
	mov	x8, x30
	/* XXX: Assume that we always start running at EL3 */
	mrs	x4, sctlr_el3

	/* FIXME: How to enable branch prediction on ARMv8? */

	/* Flush and invalidate dcache */
	mov	x0, #DCCISW
	bl	flush_dcache_all

	/* Deactivate MMU (0), Alignment Check (1) and DCache (2) */
	and	x4, x4, # ~(1 << 0) & ~(1 << 1) & ~(1 << 2)
	/* Activate ICache (12) already for speed */
	orr	x4, x4, #(1 << 12)
	msr	sctlr_el3, x4

	/* Invalidate icache and TLB for good measure */
	ic	iallu
	tlbi	alle3
	dsb	sy
	isb

	ret	x8
ENDPROC(arm_init_caches)

/* Based on u-boot transition.S */
ENTRY(switch_el3_to_el2)
	mov	x0, #0x5b1	/* Non-secure EL0/EL1 | HVC | 64bit EL2 */
	msr	scr_el3, x0
	msr	cptr_el3, xzr	/* Disable coprocessor traps to EL3 */
	mov	x0, #0x33ff
	msr	cptr_el2, x0	/* Disable coprocessor traps to EL2 */

	/* Return to the EL2_SP2 mode from EL3 */
	mov	x0, sp
	msr	sp_el2, x0	/* Migrate SP */
	mrs	x0, vbar_el3
	msr	vbar_el2, x0	/* Migrate VBAR */
	mrs	x0, sctlr_el3
	msr	sctlr_el2, x0	/* Migrate SCTLR */
	mov	x0, #0x3c9
	msr	spsr_el3, x0	/* EL2_SP2 | D | A | I | F */
	msr	elr_el3, x30
	eret
ENDPROC(switch_el3_to_el2)
