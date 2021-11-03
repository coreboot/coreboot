/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ARCH_IO_H
#define _ARCH_IO_H

#include <inttypes.h>

/* Functions in this file are unimplemented by default. Tests are expected to implement
   mocks for these functions, if tests will call functions using functions listed below. */

uint8_t readb(volatile const void *_a);
uint16_t readw(volatile const void *_a);
uint32_t readl(volatile const void *_a);

void writeb(uint8_t _v, volatile void *_a);
void writew(uint16_t _v, volatile void *_a);
void writel(uint32_t _v, volatile void *_a);

uint8_t read8(volatile const void *addr);
uint16_t read16(volatile const void *addr);
uint32_t read32(volatile const void *addr);
uint64_t read64(volatile const void *addr);

void write8(volatile void *addr, uint8_t val);
void write16(volatile void *addr, uint16_t val);
void write32(volatile void *addr, uint32_t val);
void write64(volatile void *addr, uint64_t val);

/* x86 I/O functions */
unsigned int inl(int port);
unsigned short inw(int port);
unsigned char inb(int port);

void outl(unsigned int val, int port);
void outw(unsigned short val, int port);
void outb(unsigned char val, int port);

void outsl(int port, const void *addr, unsigned long count);
void outsw(int port, const void *addr, unsigned long count);
void outsb(int port, const void *addr, unsigned long count);

void insl(int port, void *addr, unsigned long count);
void insw(int port, void *addr, unsigned long count);
void insb(int port, void *addr, unsigned long count);

#endif /* _ARCH_IO_H */
