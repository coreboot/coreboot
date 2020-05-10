/* SPDX-License-Identifier: GPL-2.0-only */

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
