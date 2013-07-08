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
 * they should go away  in the future...
 */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include "../debug.h"
#include "../biosemu.h"
#include <vbe.h>
#include "../compat/time.h"

#define VMEM_SIZE (1024 * 1024) /* 1 MB */

#if !CONFIG_YABEL_DIRECTHW
#if CONFIG_YABEL_VIRTMEM_LOCATION
u8* vmem = (u8 *) CONFIG_YABEL_VIRTMEM_LOCATION;
#else
u8* vmem = (u8 *) (16*1024*1024); /* default to 16MB */
#endif
#else
u8* vmem = NULL;
#endif

void run_bios(struct device * dev, unsigned long addr)
{

	biosemu(vmem, VMEM_SIZE, dev, addr);

#if CONFIG_FRAMEBUFFER_SET_VESA_MODE
	vbe_set_graphics();
#endif
}

unsigned long tb_freq = 0;

u64 get_time(void)
{
    u64 act = 0;
#if CONFIG_ARCH_X86
    u32 eax, edx;

    __asm__ __volatile__(
	"rdtsc"
        : "=a"(eax), "=d"(edx)
        : /* no inputs, no clobber */);
    act = ((u64) edx << 32) | eax;
#endif
    return act;
}
