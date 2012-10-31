/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Google Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _ASM_IO_H
#define _ASM_IO_H

#include <stdint.h>
#include <arch/byteorder.h>

static inline __attribute__((always_inline)) u8 read8(unsigned long addr)
{
	return *((volatile u8 *)(addr));
}

static inline __attribute__((always_inline)) u16 read16(unsigned long addr)
{
	return *((volatile u16 *)(addr));
}

static inline __attribute__((always_inline)) u32 read32(unsigned long addr)
{
	return *((volatile u32 *)(addr));
}

static inline __attribute__((always_inline)) void write8(unsigned long addr, u8 value)
{
	*((volatile u8 *)(addr)) = value;
}

static inline __attribute__((always_inline)) void write16(unsigned long addr, u16 value)
{
	*((volatile u16 *)(addr)) = value;
}

static inline __attribute__((always_inline)) void write32(unsigned long addr, u32 value)
{
	*((volatile u32 *)(addr)) = value;
}

#define writel(val, addr) write32(addr, val)
#define readl(addr) read32(val)

// platform specific. arm maps all io to memory. 
#define IO_BASE_ADDR 0 // lets see if we can do 1:1 mapping

#define __io(x) (x + IO_BASE_ADDR)

static inline __attribute__((always_inline)) void outb(u8 v, unsigned long p)
{
	write8(__io(p), v);
}

static inline __attribute__((always_inline)) void outw(u16 v, unsigned long p)
{
	write16(__io(p), (u16)cpu_to_le16(v));
}

static inline __attribute__((always_inline)) void outl(u32 v, unsigned long p)
{
	write32(__io(p), (u32)cpu_to_le32(v));
}

static inline __attribute__((always_inline)) u8 inb(unsigned long p)
{
	u8 __v = read8(__io(p)); 
	return __v;
}

static inline __attribute__((always_inline)) u16 inw(unsigned long p)
{
	u16 __v = le16_to_cpu((u16)read16(__io(p)));
	return __v;
}
static inline __attribute__((always_inline)) u32 inl(unsigned long p)
{
	u32 __v = le32_to_cpu((u32)read32(__io(p)));
	return __v;
}

#endif

