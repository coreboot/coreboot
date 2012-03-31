/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Eric W. Biederman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CPU_X86_CACHE
#define CPU_X86_CACHE

#define CR0_CacheDisable	(1 << 30)
#define CR0_NoWriteThrough	(1 << 29)

#if !defined(__ASSEMBLER__)

/*
 * Need two versions because ROMCC chokes on certain clobbers:
 * cache.h:29.71: cache.h:60.24: earlymtrr.c:117.23: romstage.c:144.33:
 * 0x1559920 asm        Internal compiler error: lhs 1 regcm == 0
 */

#if defined(__GNUC__)

/* The memory clobber prevents the GCC from reordering the read/write order
 * of CR0
 */
static inline unsigned long read_cr0(void)
{
	unsigned long cr0;
	asm volatile ("movl %%cr0, %0" : "=r" (cr0) :: "memory");
	return cr0;
}

static inline void write_cr0(unsigned long cr0)
{
	asm volatile ("movl %0, %%cr0" : : "r" (cr0) : "memory");
}

static inline void wbinvd(void)
{
	asm volatile ("wbinvd" ::: "memory");
}

#else

static inline unsigned long read_cr0(void)
{
	unsigned long cr0;
	asm volatile ("movl %%cr0, %0" : "=r" (cr0));
	return cr0;
}

static inline void write_cr0(unsigned long cr0)
{
	asm volatile ("movl %0, %%cr0" : : "r" (cr0));
}

static inline void wbinvd(void)
{
	asm volatile ("wbinvd");
}

#endif

static inline void invd(void)
{
	asm volatile("invd" ::: "memory");
}

/* The following functions require the always_inline due to AMD
 * function STOP_CAR_AND_CPU that disables cache as
 * ram, the cache as ram stack can no longer be used. Called
 * functions must be inlined to avoid stack usage. Also, the
 * compiler must keep local variables register based and not
 * allocated them from the stack. With gcc 4.5.0, some functions
 * declared as inline are not being inlined. This patch forces
 * these functions to always be inlined by adding the qualifier
 * __attribute__((always_inline)) to their declaration.
 */
static inline __attribute__((always_inline)) void enable_cache(void)
{
	unsigned long cr0;
	cr0 = read_cr0();
	cr0 &= 0x9fffffff;
	write_cr0(cr0);
}

static inline __attribute__((always_inline)) void disable_cache(void)
{
	/* Disable and write back the cache */
	unsigned long cr0;
	cr0 = read_cr0();
	cr0 |= 0x40000000;
	wbinvd();
	write_cr0(cr0);
	wbinvd();
}

#if !defined(__PRE_RAM__)
void x86_enable_cache(void);
#endif

#endif /* !__ASSEMBLER__ */
#endif /* CPU_X86_CACHE */
