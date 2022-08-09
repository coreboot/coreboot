/*
 *
 * Copyright 2022 Google LLC
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

#include <stdint.h>

#include <arch/cache.h>
#include <arch/cpuid.h>

unsigned int dcache_line_bytes(void)
{
	/*
	 * The value returned in EBX[15:8] is in 8-byte increments.
	 * Cache line size is EBX[15:8] * 8
	 */
	return (cpuid_ebx(1) & 0xff00) >> 5;
}

static inline int cpu_supports_wbnoinvd(void)
{
	return (cpuid_ebx(0x80000008) >> 9) & 1;
}

static inline int cpu_supports_clwb(void)
{
	return (cpuid_ebx(7) >> 24) & 1;
}

static inline int cpu_supports_clflushopt(void)
{
	return (cpuid_sub_leaf_ebx(7, 0) >> 23) & 1;
}

static inline int cpu_supports_clflush(void)
{
	return (cpuid_ebx(1) >> 19) & 1;
}

inline void dcache_invalidate_all(void)
{
	asm volatile("invd" ::: "memory");
}

inline void dcache_clean_invalidate_all(void)
{
	asm volatile("wbinvd" ::: "memory");
}

inline void dcache_clean_all(void)
{
	if (cpu_supports_wbnoinvd()) {
		asm volatile(
			"sfence\n\t"
			"wbnoinvd\n\t"
		::: "memory");
	} else {
		dcache_clean_invalidate_all();
	}
}

void dcache_clean_by_mva(void const *addr, size_t len)
{
	unsigned long line, linesize;

	linesize = dcache_line_bytes();
	line = (uintptr_t)addr & ~(linesize - 1);

	if (cpu_supports_clwb()) {
		asm volatile("sfence");
		while (line < (uintptr_t)addr + len) {
			asm volatile("clwb (%0)" : : "r"(line) : "memory");
			line += linesize;
		}
	} else {
		dcache_clean_invalidate_by_mva(addr, len);
	}
}

void dcache_invalidate_by_mva(void const *addr, size_t len)
{
	/*
	 * x86 doesn't have a "invalidate without clean" for a cache line, fall
	 * back to both.
	 */
	dcache_clean_invalidate_by_mva(addr, len);
}

void dcache_clean_invalidate_by_mva(void const *addr, size_t len)
{
	unsigned long line, linesize;

	linesize = dcache_line_bytes();
	line = (uintptr_t)addr & ~(linesize - 1);

	if (cpu_supports_clflushopt()) {
		asm volatile("sfence");
		while (line < (uintptr_t)addr + len) {
			asm volatile("clflushopt (%0)" ::"r"(line) : "memory");
			line += linesize;
		}
	} else if (cpu_supports_clflush()) {
		asm volatile("sfence");
		while (line < (uintptr_t)addr + len) {
			asm volatile("clflush (%0)" : : "r"(line) : "memory");
			line += linesize;
		}
	} else {
		dcache_clean_invalidate_all();
	}
}
