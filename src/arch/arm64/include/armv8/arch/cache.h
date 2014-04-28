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

#include <config.h>
#include <stddef.h>
#include <stdint.h>

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

/*
 * Utility macro to choose an instruction according to the exception
 * level (EL) passed, which number is concatenated between insa and insb parts
 */
#define SWITCH_EL(insa, insb, el) if (el == 1) asm volatile(insa "1" insb); \
	else if (el == 2) asm volatile (insa "2" insb); \
	else asm volatile (insa "3" insb)

/* get current exception level (EL1-EL3) */
static inline uint32_t current_el(void)
{
	uint32_t el;
	asm volatile ("mrs %0, CurrentEL" : "=r" (el));
	return el >> 2;
}

/*
 * Sync primitives
 */

/* data memory barrier */
static inline void dmb(void)
{
	asm volatile ("dmb sy" : : : "memory");
}

/* data sync barrier */
static inline void dsb(void)
{
	asm volatile ("dsb sy" : : : "memory");
}

/* instruction sync barrier */
static inline void isb(void)
{
	asm volatile ("isb sy" : : : "memory");
}

/*
 * Low-level TLB maintenance operations
 */

/* invalidate entire unified TLB */
static inline void tlbiall(uint32_t el)
{
	SWITCH_EL("tlbi alle", : : : "memory", el);
}

/* invalidate unified TLB by VA, all ASID (EL1) */
static inline void tlbivaa(uint64_t va)
{
	asm volatile("tlbi vaae1, %0" : : "r" (va) : "memory");
}

/* write translation table base register 0 (TTBR0_ELx) */
static inline void write_ttbr0(uint64_t val, uint32_t el)
{
	SWITCH_EL("msr ttbr0_el", ", %0" : : "r" (val) : "memory", el);
}

/* read translation control register (TCR_ELx) */
static inline uint64_t read_tcr(uint32_t el)
{
	uint64_t val = 0;
	SWITCH_EL("mrs %0, tcr_el", : "=r" (val), el);
	return val;
}

/* write translation control register (TCR_ELx) */
static inline void write_tcr(uint64_t val, uint32_t el)
{
	SWITCH_EL("msr tcr_el", ", %0" : : "r" (val) : "memory", el);
}

/*
 * Low-level cache maintenance operations
 */

/* data cache clean and invalidate by VA to PoC */
static inline void dccivac(uint64_t va)
{
	asm volatile ("dc civac, %0" : : "r" (va) : "memory");
}

/* data cache clean and invalidate by set/way */
static inline void dccisw(uint64_t val)
{
	asm volatile ("dc cisw, %0" : : "r" (val) : "memory");
}

/* data cache clean by VA to PoC */
static inline void dccvac(uint64_t va)
{
	asm volatile ("dc cvac, %0" : : "r" (va) : "memory");
}

/* data cache clean by set/way */
static inline void dccsw(uint64_t val)
{
	asm volatile ("dc csw, %0" : : "r" (val) : "memory");
}

/* data cache invalidate by VA to PoC */
static inline void dcivac(uint64_t va)
{
	asm volatile ("dc ivac, %0" : : "r" (va) : "memory");
}

/* data cache invalidate by set/way */
static inline void dcisw(uint64_t val)
{
	asm volatile ("dc isw, %0" : : "r" (val) : "memory");
}

/* instruction cache invalidate all */
static inline void iciallu(void)
{
	asm volatile ("ic iallu" : : : "memory");
}

/*
 * Cache registers functions
 */

/* read cache level ID register (CLIDR_EL1) */
static inline uint32_t read_clidr(void)
{
	uint32_t val = 0;
	asm volatile ("mrs %0, clidr_el1" : "=r" (val));
	return val;
}

/* read cache size ID register register (CCSIDR_EL1) */
static inline uint32_t read_ccsidr(void)
{
	uint32_t val = 0;
	asm volatile ("mrs %0, ccsidr_el1" : "=r" (val));
	return val;
}

/* read cache size selection register (CSSELR_EL1) */
static inline uint32_t read_csselr(void)
{
	uint32_t val = 0;
	asm volatile ("mrs %0, csselr_el1" : "=r" (val));
	return val;
}

/* write to cache size selection register (CSSELR_EL1) */
static inline void write_csselr(uint32_t val)
{
	/*
	 * Bits [3:1] - Cache level + 1 (0b000 = L1, 0b110 = L7, 0b111 is rsvd)
	 * Bit 0 - 0 = data or unified cache, 1 = instruction cache
	 */
	asm volatile ("msr csselr_el1, %0" : : "r" (val));
	isb();	/* ISB to sync the change to CCSIDR_EL1 */
}

/* read system control register (SCTLR_ELx) */
static inline uint32_t read_sctlr(uint32_t el)
{
	uint32_t val;
	SWITCH_EL("mrs %0, sctlr_el", : "=r" (val), el);
	return val;
}

/* write system control register (SCTLR_ELx) */
static inline void write_sctlr(uint32_t val, uint32_t el)
{
	SWITCH_EL("msr sctlr_el", ", %0" : : "r" (val) : "cc", el);
	isb();
}


/* dcache clean by virtual address to PoC */
void dcache_clean_by_va(void const *addr, size_t len);

/* dcache clean and invalidate by virtual address to PoC */
void dcache_clean_invalidate_by_va(void const *addr, size_t len);

/* dcache invalidate by virtual address to PoC */
void dcache_invalidate_by_va(void const *addr, size_t len);

/* dcache invalidate all */
void flush_dcache_all(void);

/* returns number of bytes per cache line */
unsigned int dcache_line_bytes(void);

/* dcache and MMU disable */
void dcache_mmu_disable(void);

/* dcache and MMU enable */
void dcache_mmu_enable(void);

/* perform all icache/dcache maintenance needed after loading new code */
void cache_sync_instructions(void);

/* tlb invalidate all */
void tlb_invalidate_all(void);

#endif /* ARM_ARM64_CACHE_H */
