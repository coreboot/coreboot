/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __ARCH_MMIO_H__
#define __ARCH_MMIO_H__

#include <stdint.h>

/* NOTE: These are just stubs; if the architecture requires special
 * care to avoid posted writes or cachelines, it is not yet done here.
 */

static inline uint8_t read8(const volatile void *addr)
{
	return *(volatile uint8_t *)addr;
}

static inline uint16_t read16(const volatile void *addr)
{
	return *(volatile uint16_t *)addr;
}

static inline uint32_t read32(const volatile void *addr)
{
	return *(volatile uint32_t *)addr;
}

static inline void write8(volatile void *addr, uint8_t val)
{
	*(volatile uint8_t *)addr = val;
}

static inline void write16(volatile void *addr, uint16_t val)
{
	*(volatile uint16_t *)addr = val;
}

static inline void write32(volatile void *addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
}

#endif /* __ARCH_MMIO_H__ */
