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

#ifndef ARM_ARM64_CACHE_H
#define ARM_ARM64_CACHE_H

/* SCTLR_ELx common bits */
#define SCTLR_M		(1 << 0)	/* MMU enable			*/
#define SCTLR_A		(1 << 1)	/* Alignment check enable	*/
#define SCTLR_C		(1 << 2)	/* Data/unified cache enable	*/
#define SCTLR_SA	(1 << 3)	/* Stack alignment check enable	*/
#define SCTLR_I		(1 << 12)	/* Instruction cache enable	*/
#define SCTLR_WXN	(1 << 19)	/* Write permission implies XN	*/
#define SCTLR_EE	(1 << 25)	/* Exception endianness		*/

/* SCTLR_EL1 bits */
#define SCTLR_EL1_CP15B	(1 << 5)	/* CP15 barrier enable		*/
#define SCTLR_EL1_ITD	(1 << 7)	/* IT disable			*/
#define SCTLR_EL1_SED	(1 << 8)	/* SETEND disable		*/
#define SCTLR_EL1_UMA	(1 << 9)	/* User mask access		*/
#define SCTLR_EL1_DZE	(1 << 14)	/* DC ZVA instruction at EL0	*/
#define SCTLR_EL1_UCT	(1 << 15)	/* CTR_EL0 register EL0 access	*/
#define SCTLR_EL1_NTWI	(1 << 16)	/* Not trap WFI		 	*/
#define SCTLR_EL1_NTWE	(1 << 18)	/* Not trap WFE		 	*/
#define SCTLR_EL1_E0E	(1 << 24)	/* Exception endianness at EL0	*/
#define SCTLR_EL1_UCI	(1 << 26)	/* EL0 access to cache instructions */

#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>
#include <arch/barrier.h>

/* dcache clean by virtual address to PoC */
void dcache_clean_by_mva(void const *addr, size_t len);

/* dcache clean and invalidate by virtual address to PoC */
void dcache_clean_invalidate_by_mva(void const *addr, size_t len);

/* dcache invalidate by virtual address to PoC */
void dcache_invalidate_by_mva(void const *addr, size_t len);

/* dcache clean and/or invalidate all sets/ways to PoC */
void dcache_clean_all(void);
void dcache_invalidate_all(void);
void dcache_clean_invalidate_all(void);

/* returns number of bytes per cache line */
unsigned int dcache_line_bytes(void);

/* perform all icache/dcache maintenance needed after loading new code */
void cache_sync_instructions(void);

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

#endif /* __ASSEMBLER__ */

#endif /* ARM_ARM64_CACHE_H */
