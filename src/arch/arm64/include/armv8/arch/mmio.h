/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
/*
 * Originally imported from linux/include/asm-arm/io.h. This file has changed
 * substantially since then.
 */

#ifndef __ARCH_MMIO_H__
#define __ARCH_MMIO_H__

#include <stdint.h>
#include <arch/barrier.h>
#include <arch/lib_helpers.h>

static inline uint8_t read8(const void *addr)
{
	dmb();
	return *(volatile uint8_t *)addr;
}

static inline uint16_t read16(const void *addr)
{
	dmb();
	return *(volatile uint16_t *)addr;
}

static inline uint32_t read32(const void *addr)
{
	dmb();
	return *(volatile uint32_t *)addr;
}

static inline uint64_t read64(const void *addr)
{
	dmb();
	return *(volatile uint64_t *)addr;
}

static inline void write8(void *addr, uint8_t val)
{
	dmb();
	*(volatile uint8_t *)addr = val;
	dmb();
}

static inline void write16(void *addr, uint16_t val)
{
	dmb();
	*(volatile uint16_t *)addr = val;
	dmb();
}

static inline void write32(void *addr, uint32_t val)
{
	dmb();
	*(volatile uint32_t *)addr = val;
	dmb();
}

static inline void write64(void *addr, uint64_t val)
{
	dmb();
	*(volatile uint64_t *)addr = val;
	dmb();
}

#endif /* __ARCH_MMIO_H__ */
