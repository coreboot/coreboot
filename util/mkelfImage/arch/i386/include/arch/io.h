#ifndef ARCH_IO_H
#define ARCH_IO_H

#include <stdint.h>
/* Helper functions for directly doing I/O */

extern inline uint8_t inb(uint16_t port)
{
	uint8_t result;

	__asm__ __volatile__ (
		"inb %w1,%0"
		:"=a" (result)
		:"Nd" (port));
	return result;
}

extern inline uint16_t inw(uint16_t port)
{
	uint16_t result;

	__asm__ __volatile__ (
		"inw %w1,%0"
		:"=a" (result)
		:"Nd" (port));
	return result;
}

extern inline uint32_t inl(uint32_t port)
{
	uint32_t result;

	__asm__ __volatile__ (
		"inl %w1,%0"
		:"=a" (result)
		:"Nd" (port));
	return result;
}

extern inline void outb (uint8_t value, uint16_t port)
{
	__asm__ __volatile__ (
		"outb %b0,%w1"
		:
		:"a" (value), "Nd" (port));
}

extern inline void outw (uint16_t value, uint16_t port)
{
	__asm__ __volatile__ (
		"outw %w0,%w1"
		:
		:"a" (value), "Nd" (port));
}

extern inline void outl (uint32_t value, uint16_t port)
{
	__asm__ __volatile__ (
		"outl %0,%w1"
		:
		:"a" (value), "Nd" (port));
}

#endif /* ARCH_IO_H */
