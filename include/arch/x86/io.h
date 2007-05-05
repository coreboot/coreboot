/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef _ARCH_IO_H
#define _ARCH_IO_H

#include <types.h>

/*
 * This file contains the definitions for the x86 IO instructions
 * inb/inw/inl/outb/outw/outl and the "string versions" of the same
 * (insb/insw/insl/outsb/outsw/outsl). You can also use "pausing"
 * versions of the single-IO instructions (inb_p/inw_p/..).
 */

static inline void outb(u8 value, u16 port)
{
	__asm__ __volatile__ ("outb %b0, %w1" : : "a" (value), "Nd" (port));
}

static inline void outw(u16 value, u16 port)
{
	__asm__ __volatile__ ("outw %w0, %w1" : : "a" (value), "Nd" (port));
}

static inline void outl(u32 value, u16 port)
{
	__asm__ __volatile__ ("outl %0, %w1" : : "a" (value), "Nd" (port));
}

static inline u8 inb(u16 port)
{
	u8 value;
	__asm__ __volatile__ ("inb %w1, %b0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline u16 inw(u16 port)
{
	u16 value;
	__asm__ __volatile__ ("inw %w1, %w0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline u32 inl(u16 port)
{
	u32 value;
	__asm__ __volatile__ ("inl %w1, %0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline void outsb(u16 port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsb " 
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void outsw(u16 port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsw " 
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void outsl(u16 port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsl " 
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}


static inline void insb(u16 port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insb " 
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void insw(u16 port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insw " 
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void insl(u16 port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insl " 
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void writeb(u8 b, volatile void *addr)
{
	*(volatile u8 *) addr = b;
}

static inline void writew(u16 b, volatile void *addr)
{
	*(volatile u16 *) addr = b;
}

static inline void writel(u32 b, volatile void *addr)
{
	*(volatile u32 *) addr = b;
}

static inline u8 readb(const volatile void *addr)
{
	return *(volatile u8 *) addr;
}

static inline u16 readw(const volatile void *addr)
{
	return *(volatile u16 *) addr;
}

static inline u32 readl(const volatile void *addr)
{
	return *(volatile u32 *) addr;
}

#endif

