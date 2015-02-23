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
#ifndef __ARCH_IO_H
#define __ARCH_IO_H

#include <endian.h>
#include <stdint.h>

static inline uint8_t read8(const void *addr)
{
	return *(volatile uint8_t *)addr;
}

static inline uint16_t read16(const void *addr)
{
	return *(volatile uint16_t *)addr;
}

static inline uint32_t read32(const void *addr)
{
	return *(volatile uint32_t *)addr;
}

static inline void write8(void *addr, uint8_t val)
{
	*(volatile uint8_t *)addr = val;
}

static inline void write16(void *addr, uint16_t val)
{
	*(volatile uint16_t *)addr = val;
}

static inline void write32(void *addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
}

#endif	/* __ARCH_IO_H */
