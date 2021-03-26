/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H
#define NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H

_Static_assert(CONFIG_FIXED_MCHBAR_MMIO_BASE != 0, "MCHBAR base address is zero");
_Static_assert(CONFIG_FIXED_DMIBAR_MMIO_BASE != 0, "DMIBAR base address is zero");
_Static_assert(CONFIG_FIXED_EPBAR_MMIO_BASE  != 0,  "EPBAR base address is zero");

#include <stdint.h>

/* Deprecated in favor of the read/write accessors below */
#define MCHBAR8(x)	(*((volatile u8  *)((uintptr_t)CONFIG_FIXED_MCHBAR_MMIO_BASE + (x))))
#define MCHBAR16(x)	(*((volatile u16 *)((uintptr_t)CONFIG_FIXED_MCHBAR_MMIO_BASE + (x))))
#define MCHBAR32(x)	(*((volatile u32 *)((uintptr_t)CONFIG_FIXED_MCHBAR_MMIO_BASE + (x))))

#define MCHBAR8_AND(x,  and) (MCHBAR8(x)  = MCHBAR8(x)  & (and))
#define MCHBAR16_AND(x, and) (MCHBAR16(x) = MCHBAR16(x) & (and))
#define MCHBAR32_AND(x, and) (MCHBAR32(x) = MCHBAR32(x) & (and))
#define MCHBAR8_OR(x,  or) (MCHBAR8(x)  = MCHBAR8(x)  | (or))
#define MCHBAR16_OR(x, or) (MCHBAR16(x) = MCHBAR16(x) | (or))
#define MCHBAR32_OR(x, or) (MCHBAR32(x) = MCHBAR32(x) | (or))
#define MCHBAR8_AND_OR(x,  and, or) (MCHBAR8(x)  = (MCHBAR8(x)  & (and)) | (or))
#define MCHBAR16_AND_OR(x, and, or) (MCHBAR16(x) = (MCHBAR16(x) & (and)) | (or))
#define MCHBAR32_AND_OR(x, and, or) (MCHBAR32(x) = (MCHBAR32(x) & (and)) | (or))

#define DMIBAR8(x)	(*((volatile u8  *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))
#define DMIBAR16(x)	(*((volatile u16 *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))
#define DMIBAR32(x)	(*((volatile u32 *)((uintptr_t)CONFIG_FIXED_DMIBAR_MMIO_BASE + (x))))

#define EPBAR8(x)	(*((volatile u8  *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))
#define EPBAR16(x)	(*((volatile u16 *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))
#define EPBAR32(x)	(*((volatile u32 *)((uintptr_t)CONFIG_FIXED_EPBAR_MMIO_BASE + (x))))

/* The *bar_{read,write}X macros will be replaced with functions in follow-ups */
#define _bar_clrsetbits_impl(base, addr, clear, set, bits)				\
		base##bar_write##bits(addr,						\
			(base##bar_read##bits(addr) & ~(uint##bits##_t)(clear)) | (set))

#define mchbar_read8(addr)		MCHBAR8(addr)
#define mchbar_read16(addr)		MCHBAR16(addr)
#define mchbar_read32(addr)		MCHBAR32(addr)

#define mchbar_write8(addr, value)	(MCHBAR8(addr) = (value))
#define mchbar_write16(addr, value)	(MCHBAR16(addr) = (value))
#define mchbar_write32(addr, value)	(MCHBAR32(addr) = (value))

#define mchbar_clrsetbits8(addr, clear, set)	_bar_clrsetbits_impl(mch, addr, clear, set, 8)
#define mchbar_clrsetbits16(addr, clear, set)	_bar_clrsetbits_impl(mch, addr, clear, set, 16)
#define mchbar_clrsetbits32(addr, clear, set)	_bar_clrsetbits_impl(mch, addr, clear, set, 32)

#define mchbar_setbits8(addr, set)		mchbar_clrsetbits8(addr, 0, set)
#define mchbar_setbits16(addr, set)		mchbar_clrsetbits16(addr, 0, set)
#define mchbar_setbits32(addr, set)		mchbar_clrsetbits32(addr, 0, set)

#define mchbar_clrbits8(addr, clear)		mchbar_clrsetbits8(addr, clear, 0)
#define mchbar_clrbits16(addr, clear)		mchbar_clrsetbits16(addr, clear, 0)
#define mchbar_clrbits32(addr, clear)		mchbar_clrsetbits32(addr, clear, 0)

#define dmibar_read8(addr)		DMIBAR8(addr)
#define dmibar_read16(addr)		DMIBAR16(addr)
#define dmibar_read32(addr)		DMIBAR32(addr)

#define dmibar_write8(addr, value)	(DMIBAR8(addr) = (value))
#define dmibar_write16(addr, value)	(DMIBAR16(addr) = (value))
#define dmibar_write32(addr, value)	(DMIBAR32(addr) = (value))

#define dmibar_clrsetbits8(addr, clear, set)	_bar_clrsetbits_impl(dmi, addr, clear, set, 8)
#define dmibar_clrsetbits16(addr, clear, set)	_bar_clrsetbits_impl(dmi, addr, clear, set, 16)
#define dmibar_clrsetbits32(addr, clear, set)	_bar_clrsetbits_impl(dmi, addr, clear, set, 32)

#define dmibar_setbits8(addr, set)		dmibar_clrsetbits8(addr, 0, set)
#define dmibar_setbits16(addr, set)		dmibar_clrsetbits16(addr, 0, set)
#define dmibar_setbits32(addr, set)		dmibar_clrsetbits32(addr, 0, set)

#define dmibar_clrbits8(addr, clear)		dmibar_clrsetbits8(addr, clear, 0)
#define dmibar_clrbits16(addr, clear)		dmibar_clrsetbits16(addr, clear, 0)
#define dmibar_clrbits32(addr, clear)		dmibar_clrsetbits32(addr, clear, 0)

#define epbar_read8(addr)		EPBAR8(addr)
#define epbar_read16(addr)		EPBAR16(addr)
#define epbar_read32(addr)		EPBAR32(addr)

#define epbar_write8(addr, value)	(EPBAR8(addr) = (value))
#define epbar_write16(addr, value)	(EPBAR16(addr) = (value))
#define epbar_write32(addr, value)	(EPBAR32(addr) = (value))

#define epbar_clrsetbits8(addr, clear, set)	_bar_clrsetbits_impl(ep, addr, clear, set, 8)
#define epbar_clrsetbits16(addr, clear, set)	_bar_clrsetbits_impl(ep, addr, clear, set, 16)
#define epbar_clrsetbits32(addr, clear, set)	_bar_clrsetbits_impl(ep, addr, clear, set, 32)

#define epbar_setbits8(addr, set)		epbar_clrsetbits8(addr, 0, set)
#define epbar_setbits16(addr, set)		epbar_clrsetbits16(addr, 0, set)
#define epbar_setbits32(addr, set)		epbar_clrsetbits32(addr, 0, set)

#define epbar_clrbits8(addr, clear)		epbar_clrsetbits8(addr, clear, 0)
#define epbar_clrbits16(addr, clear)		epbar_clrsetbits16(addr, clear, 0)
#define epbar_clrbits32(addr, clear)		epbar_clrsetbits32(addr, clear, 0)

#endif	/* ! NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H */
