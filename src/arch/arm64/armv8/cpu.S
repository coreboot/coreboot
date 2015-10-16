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
 */

#include <arch/asm.h>
#include <arch/cache_helpers.h>

/*
 * Bring an ARMv8 processor we just gained control of (e.g. from IROM) into a
 * known state regarding caches/SCTLR/PSTATE. Completely cleans and invalidates
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

	/* Flush and invalidate dcache */
	mov	x0, #DCCISW
	bl	flush_dcache_all

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
