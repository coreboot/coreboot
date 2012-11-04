#ifndef _ASM_IO_H
#define _ASM_IO_H

#include <stdint.h>

/*
 * This file contains the definitions for the x86 IO instructions
 * inb/inw/inl/outb/outw/outl and the "string versions" of the same
 * (insb/insw/insl/outsb/outsw/outsl).
 */
#if defined(__ROMCC__)

static inline void outb(uint8_t value, uint16_t port)
{
	__builtin_outb(value, port);
}

static inline void outw(uint16_t value, uint16_t port)
{
	__builtin_outw(value, port);
}

static inline void outl(uint32_t value, uint16_t port)
{
	__builtin_outl(value, port);
}


static inline uint8_t inb(uint16_t port)
{
	return __builtin_inb(port);
}


static inline uint16_t inw(uint16_t port)
{
	return __builtin_inw(port);
}

static inline uint32_t inl(uint16_t port)
{
	return __builtin_inl(port);
}
#else

#if defined(__PRE_RAM__) || defined(__SMM__) || CONFIG_ULINUX == 0

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

#else /* CONFIG_ULINUX == 1 */

#include <console/console.h>
#include <serialice.h>

static inline void outb(uint8_t value, uint16_t port)
{
	serialice_io_write(port, 1, value);
}

static inline void outw(uint16_t value, uint16_t port)
{
	serialice_io_write(port, 2, value);
}

static inline void outl(uint32_t value, uint16_t port)
{
	serialice_io_write(port, 4, value);
}

static inline uint8_t inb(uint16_t port)
{
	return serialice_io_read(port, 1);
}

static inline uint16_t inw(uint16_t port)
{
	return serialice_io_read(port, 2);
}

static inline uint32_t inl(uint16_t port)
{
	return serialice_io_read(port, 4);
}

#endif /* CONFIG_ULINUX */
#endif /* __ROMCC__ */

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
		);
}

static inline void insw(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insw "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}

static inline void insl(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insl "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}


#if defined(__PRE_RAM__) || defined(__SMM__) || CONFIG_ULINUX == 0

static inline __attribute__((always_inline)) uint8_t read8(unsigned long addr)
{
	return *((volatile uint8_t *)(addr));
}

static inline __attribute__((always_inline)) uint16_t read16(unsigned long addr)
{
	return *((volatile uint16_t *)(addr));
}

static inline __attribute__((always_inline)) uint32_t read32(unsigned long addr)
{
	return *((volatile uint32_t *)(addr));
}

static inline __attribute__((always_inline)) void write8(unsigned long addr, uint8_t value)
{
	*((volatile uint8_t *)(addr)) = value;
}

static inline __attribute__((always_inline)) void write16(unsigned long addr, uint16_t value)
{
	*((volatile uint16_t *)(addr)) = value;
}

static inline __attribute__((always_inline)) void write32(unsigned long addr, uint32_t value)
{
	*((volatile uint32_t *)(addr)) = value;
}

#else /* CONFIG_ULINUX */

static inline __attribute__((always_inline)) uint8_t read8(unsigned long addr)
{
	return serialice_io_read(addr, 1);
}

static inline __attribute__((always_inline)) uint16_t read16(unsigned long addr)
{
	return serialice_io_read(addr, 2);
}

static inline __attribute__((always_inline)) uint32_t read32(unsigned long addr)
{
	return serialice_io_read(addr, 4);
}

static inline __attribute__((always_inline)) void write8(unsigned long addr, uint8_t value)
{
	serialice_io_write(addr, 1, value);
}

static inline __attribute__((always_inline)) void write16(unsigned long addr, uint16_t value)
{
	serialice_io_write(addr, 2, value);
}

static inline __attribute__((always_inline)) void write32(unsigned long addr, uint32_t value)
{
	serialice_io_write(addr, 4, value);
}
#endif
#endif

