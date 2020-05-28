/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_IO_H__
#define __ARCH_IO_H__

#include <stdint.h>

u8 io_read8(u16 reg);
void io_write8(u16 reg, u8 value);

static inline void outb(uint8_t value, uint16_t port)
{
	io_write8(port, value);
}

static inline uint8_t inb(uint16_t port)
{
	return io_read8(port);
}

#endif
