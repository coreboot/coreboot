/*
 * Originally imported from linux/include/asm-arm/io.h. This file has changed
 * substantially since then.
 *
 *  Copyright 2013 Google Inc.
 *  Copyright (C) 1996-2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Modifications:
 *  08-Apr-2013	G	Replaced several macros with inlines for type safety.
 *  16-Sep-1996	RMK	Inlined the inx/outx functions & optimised for both
 *			constant addresses and variable addresses.
 *  04-Dec-1997	RMK	Moved a lot of this stuff to the new architecture
 *			specific IO header files.
 *  27-Mar-1999	PJB	Second parameter of memcpy_toio is const..
 *  04-Apr-1999	PJB	Added check_signature.
 *  12-Dec-1999	RMK	More cleanups
 *  18-Jun-2000 RMK	Removed virt_to_* and friends definitions
 */
#ifndef __ASM_ARM_IO_H
#define __ASM_ARM_IO_H

#include <types.h>
#include <arch/cache.h>		/* for dmb() */
#include <arch/byteorder.h>

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

static inline void write8(uint8_t val, const void *addr)
{
	dmb();
	*(volatile uint8_t *)addr = val;
	dmb();
}

static inline void write16(uint16_t val, const void *addr)
{
	dmb();
	*(volatile uint16_t *)addr = val;
	dmb();
}

static inline void write32(uint32_t val, const void *addr)
{
	dmb();
	*(volatile uint32_t *)addr = val;
	dmb();
}

/*
 * FIXME: These are to avoid breaking existing ARM code. We should eventually
 * re-factor all code to specify the data length intended.
  */
#define readb(a)	read8(a)
#define writeb(v,a)	write8(v,a)
#define readl(a)	read32(a)
#define writel(v,a)	write32(v,a)

/*
 * Clear and set bits in one shot. These macros can be used to clear and
 * set multiple bits in a register using a single call. These macros can
 * also be used to set a multiple-bit bit pattern using a mask, by
 * specifying the mask in the 'clear' parameter and the new bit pattern
 * in the 'set' parameter.
 */

#define out_arch(type,endian,a,v)	write##type(cpu_to_##endian(v),a)
#define in_arch(type,endian,a)		endian##_to_cpu(read##type(a))

#define out_le32(a,v)	out_arch(l,le32,a,v)
#define out_le16(a,v)	out_arch(w,le16,a,v)

#define in_le32(a)	in_arch(l,le32,a)
#define in_le16(a)	in_arch(w,le16,a)

#define out_be32(a,v)	out_arch(l,be32,a,v)
#define out_be16(a,v)	out_arch(w,be16,a,v)

#define in_be32(a)	in_arch(l,be32,a)
#define in_be16(a)	in_arch(w,be16,a)

#define out_8(a,v)	writeb(v,a)
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

#endif	/* __ASM_ARM_IO_H */
