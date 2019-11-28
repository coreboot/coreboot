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

#ifndef __ARCH_IO_H__
#define __ARCH_IO_H__

#include <stdint.h>

/*
 * This file contains the definitions for the x86 IO instructions
 * inb/inw/inl/outb/outw/outl and the "string versions" of the same
 * (insb/insw/insl/outsb/outsw/outsl).
 */
static inline void outb(uint8_t value, uint16_t port)
{
	__asm__ __volatile__ ("outb %b0, %w1" : : "a" (value), "Nd" (port));
}

static inline void outw(uint16_t value, uint16_t port)
{
	__asm__ __volatile__ ("outw %w0, %w1" : : "a" (value), "Nd" (port));
}

static inline void outl(uint32_t value, uint16_t port)
{
	__asm__ __volatile__ ("outl %0, %w1" : : "a" (value), "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t value;
	__asm__ __volatile__ ("inb %w1, %b0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline uint16_t inw(uint16_t port)
{
	uint16_t value;
	__asm__ __volatile__ ("inw %w1, %w0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline uint32_t inl(uint16_t port)
{
	uint32_t value;
	__asm__ __volatile__ ("inl %w1, %0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline void outsb(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsb "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void outsw(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsw "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void outsl(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsl "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}


static inline void insb(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insb "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		: "memory"
		);
}

static inline void insw(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insw "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		: "memory"
		);
}

static inline void insl(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insl "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		: "memory"
		);
}

#endif
