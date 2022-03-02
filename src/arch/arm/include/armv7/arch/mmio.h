/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Originally imported from linux/include/asm-arm/io.h. This file has changed
 * substantially since then.
 */

#ifndef __ARCH_MMIO_H__
#define __ARCH_MMIO_H__

#include <arch/cache.h>
#include <endian.h>
#include <stdint.h>

static inline uint8_t read8(const void *addr)
{
	dmb();
	return *(volatile uint8_t *)__builtin_assume_aligned(addr, sizeof(uint8_t));
}

static inline uint16_t read16(const void *addr)
{
	dmb();
	return *(volatile uint16_t *)__builtin_assume_aligned(addr, sizeof(uint16_t));
}

static inline uint32_t read32(const void *addr)
{
	dmb();
	return *(volatile uint32_t *)__builtin_assume_aligned(addr, sizeof(uint32_t));
}

/* Not supported */
uint64_t read64(const void *addr);

static inline void write8(void *addr, uint8_t val)
{
	dmb();
	*(volatile uint8_t *)__builtin_assume_aligned(addr, sizeof(uint8_t)) = val;
	dmb();
}

static inline void write16(void *addr, uint16_t val)
{
	dmb();
	*(volatile uint16_t *)__builtin_assume_aligned(addr, sizeof(uint16_t)) = val;
	dmb();
}

static inline void write32(void *addr, uint32_t val)
{
	dmb();
	*(volatile uint32_t *)__builtin_assume_aligned(addr, sizeof(uint32_t)) = val;
	dmb();
}

/* Not supported */
void write64(void *addr, uint64_t val);

#endif /* __ARCH_MMIO_H__ */
