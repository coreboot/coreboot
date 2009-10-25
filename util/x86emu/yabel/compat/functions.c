/****************************************************************************
 * YABEL BIOS Emulator
 *
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Copyright (c) 2008 Pattrick Hueper <phueper@hueper.net>
 ****************************************************************************/

/* this file contains functions provided by SLOF, that the current biosemu implementation needs
 * they should go away  inthe future...
 */

#include <types.h>
#ifndef CONFIG_COREBOOT_V2
#include <config.h>
#endif
#include <string.h>
#include <device/device.h>

#define VMEM_SIZE 1024 *1024 /* 1 MB */

#ifdef CONFIG_YABEL_VIRTMEM_LOCATION
u8* vmem = (u8 *) CONFIG_YABEL_VIRTMEM_LOCATION;
#else
u8* vmem = (u8 *) (16*1024*1024); /* default to 16MB */
#endif

u32 biosemu(u8 *biosmem, u32 biosmem_size, struct device *dev,
	    unsigned long rom_addr);

void run_bios(struct device * dev, unsigned long addr)
{
	biosemu(vmem, VMEM_SIZE, dev, addr);
	memcpy(0x0, vmem + 0x0, 0x400);
	memcpy(0x400, vmem + 0x400, 0x100);
	memcpy(0xc0000, vmem + 0xc0000, 0x10000);
}

u64 get_time(void)
{
    u64 act;
    u32 eax, edx;

    __asm__ __volatile__(
	"rdtsc"
        : "=a"(eax), "=d"(edx)
        : /* no inputs, no clobber */);
    act = ((u64) edx << 32) | eax; 
    return act;
}

unsigned int
read_io(void *addr, size_t sz)
{
        unsigned int ret;
	/* since we are using inb instructions, we need the port number as 16bit value */
	u16 port = (u16)(u32) addr;

        switch (sz) {
        case 1:
		asm volatile ("inb %1, %b0" : "=a"(ret) : "d" (port));
                break;
        case 2:
		asm volatile ("inw %1, %w0" : "=a"(ret) : "d" (port));
                break;
        case 4:
		asm volatile ("inl %1, %0" : "=a"(ret) : "d" (port));
                break;
        default:
                ret = 0;
        }

        return ret;
}

int
write_io(void *addr, unsigned int value, size_t sz)
{
	u16 port = (u16)(u32) addr;
        switch (sz) {
	/* since we are using inb instructions, we need the port number as 16bit value */
        case 1:
		asm volatile ("outb %b0, %1" : : "a"(value), "d" (port));
                break;
        case 2:
		asm volatile ("outw %w0, %1" : : "a"(value), "d" (port));
                break;
        case 4:
		asm volatile ("outl %0, %1" : : "a"(value), "d" (port));
                break;
        default:
                return -1;
        }

        return 0;
}

