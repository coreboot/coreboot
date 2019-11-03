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

/*
 * mmio.h provides update*() as well as read/write*() from arch/mmio.h
 */

#ifndef __MMIO_H__
#define __MMIO_H__

#include <arch/mmio.h>
#include <stdint.h>

static __always_inline void update8(volatile void *addr, uint8_t mask, uint8_t or)
{
	uint8_t reg = read8(addr);
	reg = (reg & mask) | or;
	write8(addr, reg);
}

static __always_inline void update16(volatile void *addr, uint16_t mask, uint16_t or)
{
	uint16_t reg = read16(addr);
	reg = (reg & mask) | or;
	write16(addr, reg);
}

static __always_inline void update32(volatile void *addr, uint32_t mask, uint32_t or)
{
	uint32_t reg = read32(addr);
	reg = (reg & mask) | or;
	write32(addr, reg);
}

#endif /* __MMIO_H__ */
