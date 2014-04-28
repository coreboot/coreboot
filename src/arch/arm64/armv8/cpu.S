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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <arch/asm.h>

/*
 *	flush_dcache_all()
 *
 *	Flush the whole D-cache.
 *
 *	Corrupted registers: x0-x7, x9-x11
 * 	From: Linux arch/arm64/mm/cache.S
 */
ENTRY(flush_dcache_all)
	dsb	sy		// ensure ordering with previous memory accesses
	mrs	x0, clidr_el1		// read clidr
	and	x3, x0, #0x7000000	// extract loc from clidr
	lsr	x3, x3, #23		// left align loc bit field
	cbz	x3, finished		// if loc is 0, then no need to clean
	mov	x10, #0			// start clean at cache level 0
loop1:
	add	x2, x10, x10, lsr #1	// work out 3x current cache level
	lsr	x1, x0, x2		// extract cache type bits from clidr
	and	x1, x1, #7		// mask of the bits for current cache only
	cmp	x1, #2			// see what cache we have at this level
	b.lt	skip			// skip if no cache, or just i-cache
	mrs	x9, daif		// make CSSELR and CCSIDR access atomic
	msr	csselr_el1, x10		// select current cache level in csselr
	isb				// isb to sych the new cssr&csidr
	mrs	x1, ccsidr_el1		// read the new ccsidr
	msr	daif, x9
	and	x2, x1, #7		// extract the length of the cache lines
	add	x2, x2, #4		// add 4 (line length offset)
	mov	x4, #0x3ff
	and	x4, x4, x1, lsr #3	// find maximum number on the way size
	clz	x5, x4			// find bit position of way size increment
	mov	x7, #0x7fff
	and	x7, x7, x1, lsr #13	// extract max number of the index size
loop2:
	mov	x9, x4			// create working copy of max way size
loop3:
	lsl	x6, x9, x5
	orr	x11, x10, x6		// factor way and cache number into x11
	lsl	x6, x7, x2
	orr	x11, x11, x6		// factor index number into x11
	dc	cisw, x11		// clean & invalidate by set/way
	subs	x9, x9, #1		// decrement the way
	b.ge	loop3
	subs	x7, x7, #1		// decrement the index
	b.ge	loop2
skip:
	add	x10, x10, #2		// increment cache number
	cmp	x3, x10
	b.gt	loop1
finished:
	mov	x10, #0			// swith back to cache level 0
	msr	csselr_el1, x10		// select current cache level in csselr
	dsb	sy
	isb
	ret
ENDPROC(flush_dcache_all)

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
