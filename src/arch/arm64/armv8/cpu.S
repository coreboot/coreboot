/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Optimized assembly for low-level CPU operations on ARM64 processors.
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

	mrs	x12, id_aa64mmfr2_el1		// read ID_AA64MMFR2_EL1
	ubfx	x12, x12, #20, #4		// [23:20] - CCIDX support

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

	cbz	x12, 11f			// check FEAT_CCIDX for associativity
						// branch to 11 if FEAT_CCIDX is not implemented
	ubfx	x4, x1, #3, #21			// x4 = associativity CCSIDR_EL1[23:3]
	b	12f
11:
	ubfx	x4, x1, #3, #10			// x4 = associativity CCSIDR_EL1[12:3]
12:
	clz	w5, w4				// w5 = 32 - log2(ways)
						// (bit position of way in DC)
	lsl	w9, w4, w5			// w9 = max way number
						// (aligned for DC)
	lsl	w16, w8, w5			// w16 = amount to decrement (way
						// number per iteration)
2:	//next_way
	cbz	x12, 21f			// check FEAT_CCIDX for numsets
						// branch to 21 if FEAT_CCIDX is not implemented
	ubfx	x7, x1, #32, #24		// x7(w7) = numsets CCSIDR_EL1[55:32]
	b	22f
21:
	ubfx	w7, w1, #13, #15		// w7 = numsets CCSIDR_EL1[27:13]
22:
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
	mrs	x0, CURRENT_EL(sctlr)
	mov	x1, #~(SCTLR_C | SCTLR_M)
	and	x0, x0, x1
	msr	CURRENT_EL(sctlr), x0
	isb
	bl	dcache_clean_invalidate_all
	ldr	x30, [sp, #-0x8]
	ret
ENDPROC(mmu_disable)

/*
 * Bring an ARMv8 processor we just gained control of (e.g. from IROM) into a
 * known state regarding caches/SCTLR/SCR/PSTATE. Completely invalidates
 * icache/dcache, disables MMU and dcache (if active), and enables unaligned
 * accesses, icache. Seeds stack and initializes SP_EL0. Clobbers R22 and R23.
 */
ENTRY(arm64_init_cpu)
	/* Initialize PSTATE (mask all exceptions, select SP_EL0). */
	msr	SPSel, #0
	msr	DAIFSet, #0xf

	/* TODO: This is where we'd put non-boot CPUs into WFI if needed. */

	/* x22: SCTLR, return address: x23 (callee-saved by subroutine) */
	mov	x23, x30
	mrs	x22, CURRENT_EL(sctlr)

	/* Activate ICache already for speed during cache flush below. */
	orr	x22, x22, #SCTLR_I
	msr	CURRENT_EL(sctlr), x22
	isb

	/* Invalidate dcache */
	bl	dcache_invalidate_all

	/* Reinitialize SCTLR from scratch to known-good state.
	   This may disable MMU or DCache. */
	ldr	w22, =(SCTLR_RES1 | SCTLR_I | SCTLR_SA)
	msr	CURRENT_EL(sctlr), x22

#if CONFIG_ARM64_CURRENT_EL == EL3
	/* Initialize SCR to unmask all interrupts (so that if we get a spurious
	   IRQ/SError we'll see it when it happens, not hang in BL31). This will
	   only have an effect after we DAIFClr in exception_init(). */
	mov	x22, #SCR_RES1 | SCR_IRQ | SCR_FIQ | SCR_EA
	msr	scr_el3, x22
#endif

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
