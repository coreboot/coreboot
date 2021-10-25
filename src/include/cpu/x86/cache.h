/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_CACHE
#define CPU_X86_CACHE

#include <cpu/x86/cr.h>

#define CR0_CacheDisable	(CR0_CD)
#define CR0_NoWriteThrough	(CR0_NW)

#define CPUID_FEATURE_CLFLUSH_BIT 19

#if !defined(__ASSEMBLER__)

static inline void wbinvd(void)
{
	asm volatile ("wbinvd" ::: "memory");
}

static inline void invd(void)
{
	asm volatile("invd" ::: "memory");
}

static inline void clflush(void *addr)
{
	asm volatile ("clflush (%0)"::"r" (addr));
}

/* The following functions require the __always_inline due to AMD
 * function STOP_CAR_AND_CPU that disables cache as
 * RAM, the cache as RAM stack can no longer be used. Called
 * functions must be inlined to avoid stack usage. Also, the
 * compiler must keep local variables register based and not
 * allocated them from the stack. With gcc 4.5.0, some functions
 * declared as inline are not being inlined. This patch forces
 * these functions to always be inlined by adding the qualifier
 * __always_inline to their declaration.
 */
static __always_inline void enable_cache(void)
{
	CRx_TYPE cr0;
	cr0 = read_cr0();
	cr0 &= ~(CR0_CD | CR0_NW);
	write_cr0(cr0);
}

static __always_inline void disable_cache(void)
{
	/* Disable and write back the cache */
	CRx_TYPE cr0;
	cr0 = read_cr0();
	cr0 |= CR0_CD;
	wbinvd();
	write_cr0(cr0);
	wbinvd();
}

#endif /* !__ASSEMBLER__ */
#endif /* CPU_X86_CACHE */
