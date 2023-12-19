/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * I/O device access primitives. Simplified based on related U-Boot code,
 * which is in turn based on early versions from the Linux kernel:
 *
 *   Copyright (C) 1996-2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ARCH_GENERIC_IO_H__
#define __ARCH_GENERIC_IO_H__

#include <stdint.h>
#include <stddef.h>
#include <endian.h>
#include <arch/mmio.h>

#define __io(a)	(void *)(uintptr_t)(CONFIG_PCI_IOBASE + a)

static inline void outb(uint8_t value, uint16_t port)
{
	write8(__io(port), value);
}

static inline void outw(uint16_t value, uint16_t port)
{
	write16(__io(port), cpu_to_le16(value));
}

static inline void outl(uint32_t value, uint16_t port)
{
	write32(__io(port), cpu_to_le32(value));
}

static inline uint8_t inb(uint16_t port)
{
	return read8(__io(port));
}

static inline uint16_t inw(uint16_t port)
{
	return le16_to_cpu(read16(__io(port)));
}

static inline uint32_t inl(uint16_t port)
{
	return le32_to_cpu(read32(__io(port)));
}

static inline void outsb(uint16_t port, const void *addr, unsigned long count)
{
	uint8_t *buf = (uint8_t *)addr;
	while (count--)
		write8(__io(port), *buf++);
}

static inline void outsw(uint16_t port, const void *addr, unsigned long count)
{
	uint16_t *buf = (uint16_t *)addr;
	while (count--)
		write16(__io(port), *buf++);
}

static inline void outsl(uint16_t port, const void *addr, unsigned long count)
{
	uint32_t *buf = (uint32_t *)addr;
	while (count--)
		write32(__io(port), *buf++);
}

static inline void insb(uint16_t port, void *addr, unsigned long count)
{
	uint8_t *buf = (uint8_t *)addr;
	while (count--)
		*buf++ = read8(__io(port));
}

static inline void insw(uint16_t port, void *addr, unsigned long count)
{
	uint16_t *buf = (uint16_t *)addr;
	while (count--)
		*buf++ = read16(__io(port));
}

static inline void insl(uint16_t port, void *addr, unsigned long count)
{
	uint32_t *buf = (uint32_t *)addr;
	while (count--)
		*buf++ = read32(__io(port));
}

#endif
