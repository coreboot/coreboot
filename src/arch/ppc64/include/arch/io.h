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

#ifndef _ASM_IO_H
#define _ASM_IO_H

#include <stdint.h>

static inline void outb(uint8_t value, uint16_t port)
{
}

static inline void outw(uint16_t value, uint16_t port)
{
}

static inline void outl(uint32_t value, uint16_t port)
{
}


static inline uint8_t inb(uint16_t port)
{
	return 0;
}


static inline uint16_t inw(uint16_t port)
{
	return 0;
}

static inline uint32_t inl(uint16_t port)
{
	return 0;
}

#endif
