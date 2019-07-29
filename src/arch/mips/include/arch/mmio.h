/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ARCH_MMIO_H__
#define __ARCH_MMIO_H__

#include <types.h>
#include <arch/cache.h>
#include <endian.h>

static inline uint8_t read8(const volatile void *addr)
{
	asm("sync");
	return *(volatile uint8_t *)addr;
}

static inline uint16_t read16(const volatile void *addr)
{
	asm("sync");
	return *(volatile uint16_t *)addr;
}

static inline uint32_t read32(const volatile void *addr)
{
	asm("sync");
	return *(volatile uint32_t *)addr;
}

static inline void write8(volatile void *addr, uint8_t val)
{
	asm("sync");
	*(volatile uint8_t *)addr = val;
	asm("sync");
}

static inline void write16(volatile void *addr, uint16_t val)
{
	asm("sync");
	*(volatile uint16_t *)addr = val;
	asm("sync");
}

static inline void write32(volatile void *addr, uint32_t val)
{
	asm("sync");
	*(volatile uint32_t *)addr = val;
	asm("sync");
}

/* Fixing soc/imgtech/pistachio seemed painful at the time. */
#define read32_x(addr)		read32((void *)(addr))
#define write32_x(addr, val)	write32((void *)(addr), (val))

#endif	/* __MIPS_ARCH_IO_H */
