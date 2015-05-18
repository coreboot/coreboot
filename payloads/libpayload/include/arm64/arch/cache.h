/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 *
 * cache.h: Cache maintenance API for ARM64
 */

#ifndef ARM64_CACHE_H
#define ARM64_CACHE_H

#include <stddef.h>
#include <stdint.h>

/* SCTLR bits */
#define SCTLR_M		(1 << 0)	/* MMU enable			*/
#define SCTLR_A		(1 << 1)	/* Alignment check enable	*/
#define SCTLR_C		(1 << 2)	/* Data/unified cache enable	*/
/* Bits 4:3 are reserved */
#define SCTLR_CP15BEN	(1 << 5)	/* CP15 barrier enable		*/
/* Bit 6 is reserved */
#define SCTLR_B		(1 << 7)	/* Endianness			*/
/* Bits 9:8 */
#define SCTLR_SW	(1 << 10)	/* SWP and SWPB enable		*/
#define SCTLR_Z		(1 << 11)	/* Branch prediction enable	*/
#define SCTLR_I		(1 << 12)	/* Instruction cache enable	*/
#define SCTLR_V		(1 << 13)	/* Low/high exception vectors 	*/
#define SCTLR_RR  	(1 << 14)	/* Round Robin select		*/
/* Bits 16:15 are reserved */
#define SCTLR_HA	(1 << 17)	/* Hardware Access flag enable	*/
/* Bit 18 is reserved */
/* Bits 20:19 reserved virtualization not supported */
#define SCTLR_WXN	(1 << 19)	/* Write permission implies XN	*/
#define SCTLR_UWXN	(1 << 20)	/* Unprivileged write permission
					   implies PL1 XN		*/
#define SCTLR_FI	(1 << 21)	/* Fast interrupt config enable	*/
#define SCTLR_U		(1 << 22)	/* Unaligned access behavior	*/
#define SCTLR_VE	(1 << 24)	/* Interrupt vectors enable	*/
#define SCTLR_EE	(1 << 25)	/* Exception endianness		*/
/* Bit 26 is reserved */
#define SCTLR_NMFI	(1 << 27)	/* Non-maskable FIQ support	*/
#define SCTLR_TRE	(1 << 28)	/* TEX remap enable		*/
#define SCTLR_AFE	(1 << 29)	/* Access flag enable		*/
#define SCTLR_TE	(1 << 30)	/* Thumb exception enable	*/
/* Bit 31 is reserved */

/*
 * Cache maintenance API
 */

/* dcache clean and invalidate all (on current level given by CCSELR) */
void dcache_clean_invalidate_all(void);

/* dcache clean by virtual address to PoC */
void dcache_clean_by_mva(void const *addr, size_t len);

/* dcache clean and invalidate by virtual address to PoC */
void dcache_clean_invalidate_by_mva(void const *addr, size_t len);

/* dcache invalidate by virtual address to PoC */
void dcache_invalidate_by_mva(void const *addr, size_t len);

void dcache_clean_all(void);

/* dcache invalidate all (on current level given by CCSELR) */
void dcache_invalidate_all(void);

/* returns number of bytes per cache line */
unsigned int dcache_line_bytes(void);

/* dcache and MMU disable */
void dcache_mmu_disable(void);

/* dcache and MMU enable */
void dcache_mmu_enable(void);

/* perform all icache/dcache maintenance needed after loading new code */
void cache_sync_instructions(void);

/* Ensure that loaded program segment is synced back from cache to PoC */
void arch_program_segment_loaded(void const *addr, size_t len);

/* tlb invalidate all */
void tlb_invalidate_all(void);

/* Invalidate all of the instruction cache for PE to PoU. */
static inline void icache_invalidate_all(void)
{
	__asm__ __volatile__(
		"dsb	sy\n\t"
		"ic	iallu\n\t"
		"dsb	sy\n\t"
		"isb\n\t"
	: : : "memory");
}

#endif /* ARM64_CACHE_H */
