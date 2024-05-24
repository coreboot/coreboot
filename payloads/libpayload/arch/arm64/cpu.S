/*
 * Optimized assembly for low-level CPU operations on ARM64 processors.
 *
 * Copyright (c) 2010 Per Odlund <per.odlund@armagedon.se>
 * Copyright (c) 2014 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <arch/asm.h>
#include <arch/lib_helpers.h>

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
	mrs	x0, sctlr_el2
	mov	x1, #~(SCTLR_C | SCTLR_M)
	and	x0, x0, x1
	msr	sctlr_el2, x0
	isb
	bl	dcache_clean_invalidate_all
	ldr	x30, [sp, #-0x8]
	ret
ENDPROC(mmu_disable)
