/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2009 coresystems GmbH
 * Copyright (c) 2012 Rudolf Marek - imported it from SerialICE/QEMU
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef SERIALICE_HH
#define SERIALICE_HH
#include <stdint.h>
extern const char *serialice_device;
extern int serialice_active;

void serialice_init(void);
void serialice_exit(void);
const char *serialice_lua_execute(const char *cmd);

uint8_t serialice_inb(uint16_t port);
uint16_t serialice_inw(uint16_t port);
uint32_t serialice_inl(uint16_t port);

void serialice_outb(uint8_t data, uint16_t port);
void serialice_outw(uint16_t data, uint16_t port);
void serialice_outl(uint32_t data, uint16_t port);

uint8_t serialice_readb(uint32_t addr);
uint16_t serialice_readw(uint32_t addr);
uint32_t serialice_readl(uint32_t addr);

void serialice_writeb(uint8_t data, uint32_t addr);
void serialice_writew(uint16_t data, uint32_t addr);
void serialice_writel(uint32_t data, uint32_t addr);

uint64_t serialice_rdmsr(uint32_t addr, uint32_t key);
void serialice_wrmsr(uint64_t data, uint32_t addr, uint32_t key);

typedef struct {
    uint32_t eax, ebx, ecx, edx;
} cpuid_regs_t;

cpuid_regs_t serialice_cpuid(uint32_t eax, uint32_t ecx);

int serialice_handle_load(uint32_t addr, uint32_t * result,
                          unsigned int data_size);
void serialice_log_load(int caught, uint32_t addr, uint32_t result,
                        unsigned int data_size);
int serialice_handle_store(uint32_t addr, uint32_t val, unsigned int data_size);

#endif
