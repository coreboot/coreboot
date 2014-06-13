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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __MIPS_ARCH_IO_H
#define __MIPS_ARCH_IO_H

#include <types.h>
#include <arch/cache.h>
#include <arch/byteorder.h>

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

/*
 * Clear and set bits in one shot. These macros can be used to clear and
 * set multiple bits in a register using a single call. These macros can
 * also be used to set a multiple-bit bit pattern using a mask, by
 * specifying the mask in the 'clear' parameter and the new bit pattern
 * in the 'set' parameter.
 */

#define out_arch(type, endian, a, v)	write##type(cpu_to_##endian(v), a)
#define in_arch(type, endian, a)	endian##_to_cpu(read##type(a))

#define out_le32(a, v)	out_arch(l, le32, a, v)
#define out_le16(a, v)	out_arch(w, le16, a, v)

#define in_le32(a)	in_arch(l, le32, a)
#define in_le16(a)	in_arch(w, le16, a)

#define out_be32(a, v)	out_arch(l, be32, a, v)
#define out_be16(a, v)	out_arch(w, be16, a, v)

#define in_be32(a)	in_arch(l, be32, a)
#define in_be16(a)	in_arch(w, be16, a)

#define out_8(a, v)	writeb(v, a)
#define in_8(a)		readb(a)

#define clrbits(type, addr, clear) \
	out_##type((addr), in_##type(addr) & ~(clear))

#define setbits(type, addr, set) \
	out_##type((addr), in_##type(addr) | (set))

#define clrsetbits(type, addr, clear, set) \
	out_##type((addr), (in_##type(addr) & ~(clear)) | (set))

#define clrbits_be32(addr, clear) clrbits(be32, addr, clear)
#define setbits_be32(addr, set) setbits(be32, addr, set)
#define clrsetbits_be32(addr, clear, set) clrsetbits(be32, addr, clear, set)

#define clrbits_le32(addr, clear) clrbits(le32, addr, clear)
#define setbits_le32(addr, set) setbits(le32, addr, set)
#define clrsetbits_le32(addr, clear, set) clrsetbits(le32, addr, clear, set)

#define clrbits_be16(addr, clear) clrbits(be16, addr, clear)
#define setbits_be16(addr, set) setbits(be16, addr, set)
#define clrsetbits_be16(addr, clear, set) clrsetbits(be16, addr, clear, set)

#define clrbits_le16(addr, clear) clrbits(le16, addr, clear)
#define setbits_le16(addr, set) setbits(le16, addr, set)
#define clrsetbits_le16(addr, clear, set) clrsetbits(le16, addr, clear, set)

#define clrbits_8(addr, clear) clrbits(8, addr, clear)
#define setbits_8(addr, set) setbits(8, addr, set)
#define clrsetbits_8(addr, clear, set) clrsetbits(8, addr, clear, set)

#endif	/* __MIPS_ARCH_IO_H */
