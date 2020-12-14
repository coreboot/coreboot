/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ASM_IO_H
#define _ASM_IO_H

#include <stdint.h>

/* Set MSB to 1 to ignore HRMOR */
#define MMIO_GROUP0_CHIP0_LPC_BASE_ADDR 0x8006030000000000
#define LPCHC_IO_SPACE 0xD0010000
#define LPC_BASE_ADDR (MMIO_GROUP0_CHIP0_LPC_BASE_ADDR + LPCHC_IO_SPACE)
#define MMIO_GROUP0_CHIP0_SCOM_BASE_ADDR 0x800603FC00000000

/* Enforce In-order Execution of I/O */
static inline void eieio(void)
{
	asm volatile("eieio" ::: "memory");
}

static inline void outb(uint8_t value, uint16_t port)
{
	asm volatile("stbcix %0, %1, %2" :: "r"(value), "b"(LPC_BASE_ADDR), "r"(port));
	eieio();
}

static inline void outw(uint16_t value, uint16_t port)
{
	asm volatile("sthcix %0, %1, %2" :: "r"(value), "b"(LPC_BASE_ADDR), "r"(port));
	eieio();
}

static inline void outl(uint32_t value, uint16_t port)
{
	asm volatile("stwcix %0, %1, %2" :: "r"(value), "b"(LPC_BASE_ADDR), "r"(port));
	eieio();
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t buffer;
	asm volatile("lbzcix %0, %1, %2" : "=r"(buffer) : "b"(LPC_BASE_ADDR), "r"(port));
	eieio();
	return buffer;
}

static inline uint16_t inw(uint16_t port)
{
	uint16_t buffer;
	asm volatile("lhzcix %0, %1, %2" : "=r"(buffer) : "b"(LPC_BASE_ADDR), "r"(port));
	eieio();
	return buffer;
}

static inline uint32_t inl(uint16_t port)
{
	uint32_t buffer;
	asm volatile("lwzcix %0, %1, %2" : "=r"(buffer) : "b"(LPC_BASE_ADDR), "r"(port));
	eieio();
	return buffer;
}

static inline void report_istep(uint8_t step, uint8_t substep)
{
	outb(step, 0x81);
	outb(substep, 0x82);
}

#endif
