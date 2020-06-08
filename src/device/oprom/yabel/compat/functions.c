/****************************************************************************
 * YABEL BIOS Emulator
 *
 * Copyright (c) 2008 Pattrick Hueper <phueper@hueper.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/* this file contains functions provided by SLOF, that the current biosemu implementation needs
 * they should go away  in the future...
 */

#include <types.h>
#include <device/device.h>
#include "../debug.h"
#include "../biosemu.h"
#include <vbe.h>
#include "../compat/time.h"

#define VMEM_SIZE (1024 * 1024) /* 1 MB */

#if !CONFIG(YABEL_DIRECTHW)
#if CONFIG_YABEL_VIRTMEM_LOCATION
u8* vmem = (u8 *) CONFIG_YABEL_VIRTMEM_LOCATION;
#else
u8* vmem = (u8 *) (16*1024*1024); /* default to 16MB */
#endif
#else
u8* vmem = NULL;
#endif

extern u8 *biosmem;

void run_bios(struct device * dev, unsigned long addr)
{
	biosmem = vmem;

	biosemu(vmem, VMEM_SIZE, dev, addr);

#if CONFIG(FRAMEBUFFER_SET_VESA_MODE)
	vbe_set_graphics();
#endif
}

unsigned long tb_freq = 0;

u64 get_time(void)
{
	u64 act = 0;
#if ENV_X86
	u32 eax, edx;

	__asm__ __volatile__(
		"rdtsc"
		: "=a"(eax), "=d"(edx)
		: /* no inputs, no clobber */);
	act = ((u64) edx << 32) | eax;
#endif
	return act;
}
