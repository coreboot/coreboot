/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H
#define NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H

#include <device/mmio.h>
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

_Static_assert(CONFIG_FIXED_MCHBAR_MMIO_BASE != 0, "MCHBAR base address is zero");

static __always_inline uint8_t mchbar_read8(const uintptr_t offset)
{
	return read8p(CONFIG_FIXED_MCHBAR_MMIO_BASE + offset);
}

static __always_inline uint16_t mchbar_read16(const uintptr_t offset)
{
	return read16p(CONFIG_FIXED_MCHBAR_MMIO_BASE + offset);
}

static __always_inline uint32_t mchbar_read32(const uintptr_t offset)
{
	return read32p(CONFIG_FIXED_MCHBAR_MMIO_BASE + offset);
}

static __always_inline void mchbar_write8(const uintptr_t offset, const uint8_t value)
{
	write8p(CONFIG_FIXED_MCHBAR_MMIO_BASE + offset, value);
}

static __always_inline void mchbar_write16(const uintptr_t offset, const uint16_t value)
{
	write16p(CONFIG_FIXED_MCHBAR_MMIO_BASE + offset, value);
}

static __always_inline void mchbar_write32(const uintptr_t offset, const uint32_t value)
{
	write32p(CONFIG_FIXED_MCHBAR_MMIO_BASE + offset, value);
}

_Static_assert(CONFIG_FIXED_DMIBAR_MMIO_BASE != 0, "DMIBAR base address is zero");

static __always_inline uint8_t dmibar_read8(const uintptr_t offset)
{
	return read8p(CONFIG_FIXED_DMIBAR_MMIO_BASE + offset);
}

static __always_inline uint16_t dmibar_read16(const uintptr_t offset)
{
	return read16p(CONFIG_FIXED_DMIBAR_MMIO_BASE + offset);
}

static __always_inline uint32_t dmibar_read32(const uintptr_t offset)
{
	return read32p(CONFIG_FIXED_DMIBAR_MMIO_BASE + offset);
}

static __always_inline void dmibar_write8(const uintptr_t offset, const uint8_t value)
{
	write8p(CONFIG_FIXED_DMIBAR_MMIO_BASE + offset, value);
}

static __always_inline void dmibar_write16(const uintptr_t offset, const uint16_t value)
{
	write16p(CONFIG_FIXED_DMIBAR_MMIO_BASE + offset, value);
}

static __always_inline void dmibar_write32(const uintptr_t offset, const uint32_t value)
{
	write32p(CONFIG_FIXED_DMIBAR_MMIO_BASE + offset, value);
}

_Static_assert(CONFIG_FIXED_EPBAR_MMIO_BASE  != 0,  "EPBAR base address is zero");

static __always_inline uint8_t epbar_read8(const uintptr_t offset)
{
	return read8p(CONFIG_FIXED_EPBAR_MMIO_BASE + offset);
}

static __always_inline uint16_t epbar_read16(const uintptr_t offset)
{
	return read16p(CONFIG_FIXED_EPBAR_MMIO_BASE + offset);
}

static __always_inline uint32_t epbar_read32(const uintptr_t offset)
{
	return read32p(CONFIG_FIXED_EPBAR_MMIO_BASE + offset);
}

static __always_inline void epbar_write8(const uintptr_t offset, const uint8_t value)
{
	write8p(CONFIG_FIXED_EPBAR_MMIO_BASE + offset, value);
}

static __always_inline void epbar_write16(const uintptr_t offset, const uint16_t value)
{
	write16p(CONFIG_FIXED_EPBAR_MMIO_BASE + offset, value);
}

static __always_inline void epbar_write32(const uintptr_t offset, const uint32_t value)
{
	write32p(CONFIG_FIXED_EPBAR_MMIO_BASE + offset, value);
}

#define _bar_clrsetbits_impl(base, addr, clear, set, bits)				\
		base##bar_write##bits(addr,						\
			(base##bar_read##bits(addr) & ~(uint##bits##_t)(clear)) | (set))

#define mchbar_clrsetbits8(addr, clear, set)	_bar_clrsetbits_impl(mch, addr, clear, set, 8)
#define mchbar_clrsetbits16(addr, clear, set)	_bar_clrsetbits_impl(mch, addr, clear, set, 16)
#define mchbar_clrsetbits32(addr, clear, set)	_bar_clrsetbits_impl(mch, addr, clear, set, 32)

#define mchbar_setbits8(addr, set)		mchbar_clrsetbits8(addr, 0, set)
#define mchbar_setbits16(addr, set)		mchbar_clrsetbits16(addr, 0, set)
#define mchbar_setbits32(addr, set)		mchbar_clrsetbits32(addr, 0, set)

#define mchbar_clrbits8(addr, clear)		mchbar_clrsetbits8(addr, clear, 0)
#define mchbar_clrbits16(addr, clear)		mchbar_clrsetbits16(addr, clear, 0)
#define mchbar_clrbits32(addr, clear)		mchbar_clrsetbits32(addr, clear, 0)

#define dmibar_clrsetbits8(addr, clear, set)	_bar_clrsetbits_impl(dmi, addr, clear, set, 8)
#define dmibar_clrsetbits16(addr, clear, set)	_bar_clrsetbits_impl(dmi, addr, clear, set, 16)
#define dmibar_clrsetbits32(addr, clear, set)	_bar_clrsetbits_impl(dmi, addr, clear, set, 32)

#define dmibar_setbits8(addr, set)		dmibar_clrsetbits8(addr, 0, set)
#define dmibar_setbits16(addr, set)		dmibar_clrsetbits16(addr, 0, set)
#define dmibar_setbits32(addr, set)		dmibar_clrsetbits32(addr, 0, set)

#define dmibar_clrbits8(addr, clear)		dmibar_clrsetbits8(addr, clear, 0)
#define dmibar_clrbits16(addr, clear)		dmibar_clrsetbits16(addr, clear, 0)
#define dmibar_clrbits32(addr, clear)		dmibar_clrsetbits32(addr, clear, 0)

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
