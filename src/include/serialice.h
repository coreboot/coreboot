/*
 * QEMU PC System Emulator
 *
 * Copyright (c) 2009 coresystems GmbH
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

#ifndef SERIALICE_H
#define SERIALICE_H

#define READ_FROM_QEMU		(1 << 0)
#define READ_FROM_SERIALICE	(1 << 1)

#define WRITE_TO_QEMU		(1 << 0)
#define WRITE_TO_SERIALICE	(1 << 1)

extern const char *serialice_device;
extern int serialice_active;

int serialice_lua_init(void);
const char *serialice_lua_execute(const char *cmd);

uint32_t serialice_io_read(uint16_t port, unsigned int size);
void serialice_io_write(uint16_t port, unsigned int size, uint32_t data);

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

/* serialice protocol */
typedef struct {
    void (*version) (void);
    void (*mainboard) (void);
    uint32_t (*io_read) (uint16_t port, unsigned int size);
    void (*io_write) (uint16_t port, unsigned int size, uint32_t data);
    uint32_t (*load) (uint32_t addr, unsigned int size);
    void (*store) (uint32_t addr, unsigned int size, uint32_t data);
    void (*rdmsr) (uint32_t addr, uint32_t key, uint32_t * hi, uint32_t * lo);
    void (*wrmsr) (uint32_t addr, uint32_t key, uint32_t hi, uint32_t lo);
    void (*cpuid) (uint32_t eax, uint32_t ecx, cpuid_regs_t * ret);
} SerialICE_target;

const SerialICE_target *serialice_serial_init(void);
void serialice_serial_exit(void);

/* serialice LUA */
int serialice_io_read_filter(uint32_t * data, uint16_t port, int size);
int serialice_io_write_filter(uint32_t * data, uint16_t port, int size);
int serialice_memory_read_filter(uint32_t addr, uint32_t * data, int size);
int serialice_memory_write_filter(uint32_t addr, int size, uint32_t * data);
int serialice_cpuid_filter(uint32_t eax, uint32_t ecx, cpuid_regs_t * regs);
int serialice_rdmsr_filter(uint32_t addr, uint32_t * hi, uint32_t * lo);
int serialice_wrmsr_filter(uint32_t addr, uint32_t * hi, uint32_t * lo);

void serialice_io_read_log(int caught, uint32_t data, uint32_t addr, int size);
void serialice_io_write_log(int caught, uint32_t data, uint32_t addr, int size);
void serialice_memory_read_log(int caught, uint32_t data, uint32_t addr, int size);
void serialice_memory_write_log(int caught, uint32_t data, uint32_t addr, int size);
void serialice_rdmsr_log(uint32_t addr, uint32_t hi, uint32_t lo, int filtered);
void serialice_wrmsr_log(uint32_t addr, uint32_t hi, uint32_t lo, int filtered);
void serialice_cpuid_log(uint32_t eax, uint32_t ecx, cpuid_regs_t res, int filtered);

#endif
