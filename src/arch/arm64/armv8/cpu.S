/*
 * Optimized assembly for low-level CPU operations on ARM64 processors.
 *
 * Copyright (c) 2010 Per Odlund <per.odlund@armagedon.se>
 * Copyright (c) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/asm.h>
#include <arch/cache.h>

.macro	dcache_apply_all crm
	dsb	sy
	mrs	x0, clidr_el1			// read CLIDR
	and	w3, w0, #0x07000000		// narrow to LoC
	lsr	w3, w3, #23			// left align LoC (low 4 bits)
	cbz	w3, 5f //done

	mov	w10, #0				// w10 = 2 * cache level
	mov	w8, #1				// w8 = constant 0b1

1:	//next_level
	add	w2, w10, w10, lsr #1		// calculate 3 * cache level
	lsr	w1, w0, w2			// extract 3-bit cache type for this level
	and	w1, w1, #0x7			// w1 = cache type
	cmp	w1, #2				// is it data or i&d?
	b.lt	4f //skip
	msr	csselr_el1, x10			// select current cache level
	isb					// sync change of csselr
	mrs	x1, ccsidr_el1			// w1 = read ccsidr
	and	w2, w1, #7			// w2 = log2(linelen_bytes) - 4
	add	w2, w2, #4			// w2 = log2(linelen_bytes)
	ubfx	w4, w1, #3, #10			// w4 = associativity - 1 (also
						// max way number)
	clz	w5, w4				// w5 = 32 - log2(ways)
						// (bit position of way in DC)
	lsl	w9, w4, w5			// w9 = max way number
						// (aligned for DC)
	lsl	w16, w8, w5			// w16 = amount to decrement (way
						// number per iteration)
2:	//next_way
	ubfx	w7, w1, #13, #15		// w7 = max set #, right aligned
	lsl	w7, w7, w2			// w7 = max set #, DC aligned
	lsl	w17, w8, w2			// w17 = amount to decrement (set
						// number per iteration)

3:	//next_set
	orr	w11, w10, w9			// w11 = combine way # & cache #
	orr	w11, w11, w7			// ... and set #
	dc	\crm, x11			// clean and/or invalidate line
	subs	w7, w7, w17			// decrement set number
	b.ge	3b //next_set
	subs	x9, x9, x16			// decrement way number
	b.ge	2b //next_way

4:	//skip
	add	w10, w10, #2			// increment 2 *cache level
	cmp	w3, w10				// Went beyond LoC?
	b.gt	1b //next_level

5:	//done
	dsb	sy
	isb
	ret
.endm

ENTRY(dcache_invalidate_all)
	dcache_apply_all crm=isw
ENDPROC(dcache_invalidate_all)

ENTRY(dcache_clean_all)
	dcache_apply_all crm=csw
ENDPROC(dcache_clean_all)

ENTRY(dcache_clean_invalidate_all)
	dcache_apply_all crm=cisw
ENDPROC(dcache_clean_invalidate_all)

/* This must be implemented in assembly to ensure there are no accesses to
   memory (e.g. the stack) in between disabling and flushing the cache. */
ENTRY(mmu_disable)
	str	x30, [sp, #-0x8]
	mrs	x0, sctlr_el3
	mov	x1, #~(SCTLR_C | SCTLR_M)
	and	x0, x0, x1
	msr	sctlr_el3, x0
	isb
	bl	dcache_clean_invalidate_all
	ldr	x30, [sp, #-0x8]
	ret
ENDPROC(mmu_disable)

/*
 * Bring an ARMv8 processor we just gained control of (e.g. from IROM) into a
 * known state regarding caches/SCTLR/PSTATE. Completely invalidates
 * icache/dcache, disables MMU and dcache (if active), and enables unaligned
 * accesses, icache and branch prediction (if inactive). Seeds the stack and
 * initializes SP_EL0. Clobbers R22 and R23.
 */
ENTRY(arm64_init_cpu)
	/* Initialize PSTATE (unmask all exceptions, select SP_EL0). */
	msr	SPSel, #0
	msr	DAIFClr, #0xf

	/* TODO: This is where we'd put non-boot CPUs into WFI if needed. */

	/* x22: SCTLR, return address: x23 (callee-saved by subroutine) */
	mov	x23, x30
	/* TODO: Assert that we always start running at EL3 */
	mrs	x22, sctlr_el3

	/* Activate ICache (12) already for speed during cache flush below. */
	orr	x22, x22, #(1 << 12)
	msr	sctlr_el3, x22
	isb

	/* Invalidate dcache */
	bl	dcache_invalidate_all

	/* Deactivate MMU (0), Alignment Check (1) and DCache (2) */
	and	x22, x22, # ~(1 << 0) & ~(1 << 1) & ~(1 << 2)
	/* Activate Stack Alignment (3) because why not */
	orr	x22, x22, #(1 << 3)
	/* Set to little-endian (25) */
	and	x22, x22, # ~(1 << 25)
	/* Deactivate write-xor-execute enforcement (19) */
	and	x22, x22, # ~(1 << 19)
	msr	sctlr_el3, x22

	/* Invalidate icache and TLB for good measure */
	ic	iallu
	tlbi	alle3
	dsb	sy
	isb

	/* Initialize stack with sentinel value to later check overflow. */
	ldr	x2, =0xdeadbeefdeadbeef
	ldr	x0, =_stack
	ldr	x1, =_estack
1:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	bne	1b

	/* Leave a line of beef dead for easier visibility in stack dumps. */
	sub	sp, x0, #16

	ret	x23
ENDPROC(arm64_init_cpu)
