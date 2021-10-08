/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef TESTS_MOCKS_X86_IO_H_
#define TESTS_MOCKS_X86_IO_H_

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

#endif
