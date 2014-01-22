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
 * cache.c: Cache maintenance routines for ARMv7-A and ARMv7-R
 *
 * Reference: ARM Architecture Reference Manual, ARMv7-A and ARMv7-R edition
 */

#include <stdint.h>

#include <arch/cache.h>

void tlb_invalidate_all(void)
{
}

void dcache_clean_all(void)
{
}

void dcache_clean_invalidate_all(void)
{
}

void dcache_invalidate_all(void)
{
}

unsigned int dcache_line_bytes(void)
{
	/*
	 * TODO: Implement this correctly. For now we just return a
	 * reasonable value. It was added during Nyan development and
	 * may be used in bootblock code. It matters only if dcache is
	 * turned on.
	 */
	return 64;
}

void dcache_clean_by_mva(void const *addr, size_t len)
{
}

void dcache_clean_invalidate_by_mva(void const *addr, size_t len)
{
}

void dcache_invalidate_by_mva(void const *addr, size_t len)
{
}

void dcache_mmu_disable(void)
{
}

void dcache_mmu_enable(void)
{
}

void cache_sync_instructions(void)
{
}
