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

#include <arch/lib_helpers.h>

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

/* Invalidate all TLB entries. */
static inline void tlb_invalidate_all(void)
{
	/* TLBIALL includes dTLB and iTLB on systems that have them. */
	tlbiall_el3();
	dsb();
	isb();
}

/* Invalidate all of the instruction cache for PE to PoU. */
static inline void icache_invalidate_all(void)
{
	dsb();
	iciallu();
	dsb();
	isb();
}

#endif /* __ASSEMBLER__ */

#endif /* ARM_ARM64_CACHE_H */
