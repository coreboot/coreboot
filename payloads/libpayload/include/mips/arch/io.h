/*
 * This file is part of the libpayload project.
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

#ifndef __MIPS_ARCH_IO_H__
#define __MIPS_ARCH_IO_H__

#include <arch/types.h>
#include <arch/cache.h>
#include <arch/byteorder.h>

#define read8(a)	(*(volatile uint8_t *) (a))
#define read16(a)	(*(volatile uint16_t *) (a))
#define read32(a)	(*(volatile uint32_t *) (a))

#define write8(v, a)	(*(volatile uint8_t *) (a) = (v))
#define write16(v, a)	(*(volatile uint16_t *) (a) = (v))
#define write32(v, a)	(*(volatile uint32_t *) (a) = (v))


/*
 * Clear and set bits in one shot. These macros can be used to clear and
 * set multiple bits in a register using a single call. These macros can
 * also be used to set a multiple-bit bit pattern using a mask, by
 * specifying the mask in the 'clear' parameter and the new bit pattern
 * in the 'set' parameter.
 */

#define out_arch(type, endian, a, v)	write##type(cpu_to_##endian(v), a)
#define in_arch(type, endian, a)	endian##_to_cpu(read##type(a))

#define readb(a)	read8(a)
#define readw(a)	read16(a)
#define readl(a)	read32(a)

#define inb(a)		read8(a)
#define inw(a)		read16(a)
#define inl(a)		read32(a)

#define writeb(v, a)	write8(v, a)
#define writew(v, a)	write16(v, a)
#define writel(v, a)	write32(v, a)

#define outb(v, a)	write8(v, a)
#define outw(v, a)	write16(v, a)
#define outl(v, a)	write32(v, a)

#endif	/* __MIPS_ARCH_IO_H__ */
