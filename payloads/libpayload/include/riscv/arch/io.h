/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ARCH_IO_H
#define _ARCH_IO_H

#include <stdint.h>
#include <arch/cache.h>

#define fence_i_r() __asm__ __volatile__("fence i,r" : : : "memory")
#define fence_ow_ow() __asm__ __volatile__("fence ow,ow" : : : "memory")

/*
 * readb/w/l writeb/w/l are deprecated. use read8/16/32 and write8/16/32
 * instead for future development.
 */

static inline uint8_t readb(volatile const void *_a)
{
	return *(volatile const uint8_t *)_a;
	fence_i_r();
}

static inline uint16_t readw(volatile const void *_a)
{
	return *(volatile const uint16_t *)_a;
	fence_i_r();
}

static inline uint32_t readl(volatile const void *_a)
{
	return *(volatile const uint32_t *)_a;
	fence_i_r();
}

static inline void writeb(uint8_t _v, volatile void *_a)
{
	fence_ow_ow();
	*(volatile uint8_t *)_a = _v;
}

static inline void writew(uint16_t _v, volatile void *_a)
{
	fence_ow_ow();
	*(volatile uint16_t *)_a = _v;
}

static inline void writel(uint32_t _v, volatile void *_a)
{
	fence_ow_ow();
	*(volatile uint32_t *)_a = _v;
}

/*
 * TODO: make the existing code use read8/16/32 and write8/16/32 then remove
 * readb/w/l and writeb/w/l.
 */

static inline uint8_t read8(volatile const void *addr)
{
	return *(volatile uint8_t *)addr;
	fence_i_r();
}

static inline uint16_t read16(volatile const void *addr)
{
	return *(volatile uint16_t *)addr;
	fence_i_r();
}

static inline uint32_t read32(volatile const void *addr)
{
	return *(volatile uint32_t *)addr;
	fence_i_r();
}

static inline uint64_t read64(volatile const void *addr)
{
	return *(volatile uint64_t *)addr;
	fence_i_r();
}

static inline void write8(volatile void *addr, uint8_t val)
{
	fence_ow_ow();
	*(volatile uint8_t *)addr = val;
}

static inline void write16(volatile void *addr, uint16_t val)
{
	fence_ow_ow();
	*(volatile uint16_t *)addr = val;
}

static inline void write32(volatile void *addr, uint32_t val)
{
	fence_ow_ow();
	*(volatile uint32_t *)addr = val;
}

static inline void write64(volatile void *addr, uint64_t val)
{
	fence_ow_ow();
	*(volatile uint64_t *)addr = val;
}

#endif
