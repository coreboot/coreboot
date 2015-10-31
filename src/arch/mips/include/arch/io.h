/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * Based on arch/armv7/include/arch/io.h:
 *   Copyright 2013 Google Inc.
 *   Copyright (C) 1996-2000 Russell King
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

#ifndef __MIPS_ARCH_IO_H
#define __MIPS_ARCH_IO_H

#include <types.h>
#include <arch/cache.h>
#include <endian.h>

static inline uint8_t read8(unsigned long addr)
{
	asm("sync");
	return *(volatile uint8_t *)addr;
}

static inline uint16_t read16(unsigned long addr)
{
	asm("sync");
	return *(volatile uint16_t *)addr;
}

static inline uint32_t read32(unsigned long addr)
{
	asm("sync");
	return *(volatile uint32_t *)addr;
}

static inline void write8(unsigned long addr, uint8_t val)
{
	asm("sync");
	*(volatile uint8_t *)addr = val;
	asm("sync");
}

static inline void write16(unsigned long addr, uint16_t val)
{
	asm("sync");
	*(volatile uint16_t *)addr = val;
	asm("sync");
}

static inline void write32(unsigned long addr, uint32_t val)
{
	asm("sync");
	*(volatile uint32_t *)addr = val;
	asm("sync");
}

#endif	/* __MIPS_ARCH_IO_H */
