/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H
#define NORTHBRIDGE_INTEL_COMMON_FIXED_BARS_H

#include <device/mmio.h>
#include <stdint.h>

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
