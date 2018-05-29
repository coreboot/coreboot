/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Copyright (C) 2018  Lubomir Rintel <lkundrak@v3.sk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define __SIMPLE_DEVICE__

#include "vx900.h"

#include <device/pci.h>
#include <cbmem.h>

#define MCU PCI_DEV(0, 0, 3)

#define CHROME_9_HD_MIN_FB_SIZE   8
#define CHROME_9_HD_MAX_FB_SIZE 512

/* Helper to determine the framebuffer size */
void vx900_set_chrome9hd_fb_size(u32 size_mb)
{
	u8 reg8, ranksize;
	u32 tom_mb, max_size_mb;
	int i;

	/* The minimum framebuffer size is 8MB. */
	size_mb = MAX(size_mb, CHROME_9_HD_MIN_FB_SIZE);

	/*
	 * We have two limitations on the maximum framebuffer size:
	 * 1) (Sanity) No more that 1/4 of system RAM
	 * 2) (Hardware limitation) No larger than DRAM in last rank
	 * Check both of these limitations and apply them to our framebuffer */
	tom_mb = (pci_read_config16(MCU, 0x88) & 0x07ff) << (24 - 20);
	max_size_mb = tom_mb >> 2;
	if (size_mb > max_size_mb) {
		printk(BIOS_ALERT, "The framebuffer size of %dMB is larger"
		       " than 1/4 of available memory.\n"
		       " Limiting framebuffer to %dMB\n", size_mb, max_size_mb);
		size_mb = max_size_mb;
	}

	/* Now handle limitation #2
	 * Look at the ending address of the memory ranks, from last to first,
	 * until we find one that is not zero. That is our last rank, and its
	 * size is the limit of our framebuffer. */
	/* FIXME:  This has a bug. If we remap memory above 4G, we consider the
	 * memory hole as part of our RAM. Thus if we install 3G, with a TOLM of
	 * 2.5G, our TOM will be at 5G and we'll assume we have 5G RAM instead
	 * of the actual 3.5G */
	for (i = VX900_MAX_MEM_RANKS - 1; i > -1; i--) {
		reg8 = pci_read_config8(MCU, 0x40 + i);
		if (reg8 == 0)
			continue;
		/* We've reached the last populated rank */
		ranksize = reg8 - pci_read_config8(MCU, 0x48 + i);
		max_size_mb = ranksize << 6;
		/* That's it. We got what we needed. */
		break;
	}
	if (size_mb > max_size_mb) {
		printk(BIOS_ALERT, "The framebuffer size of %dMB is larger"
		       " than size of the last DRAM rank.\n"
		       " Limiting framebuffer to %dMB\n", size_mb, max_size_mb);
		size_mb = max_size_mb;
	}

	/* Now round the framebuffer size to the closest power of 2 */
	u8 fb_pow = 0;
	while (size_mb >> fb_pow)
		fb_pow++;
	fb_pow--;
	size_mb = (1 << fb_pow);

	pci_mod_config8(MCU, 0xa1, 7 << 4, (fb_pow - 2) << 4);
}

/* Gets the configured framebuffer size as a power of 2 */
u8 vx900_get_chrome9hd_fb_pow(void)
{
	u8 fb_pow = (pci_read_config8(MCU, 0xa1) >> 4) & 7;

	if (fb_pow > 0)
		fb_pow += 2;

	return fb_pow;
}

/* Gets the configured framebuffer size in MB */
u32 vx900_get_chrome9hd_fb_size(void)
{
	u8 size = vx900_get_chrome9hd_fb_pow();

	if (size == 0)
		return 0;

	return 1 << size;
}

u32 vx900_get_tolm(void)
{
	return (pci_read_config16(MCU, 0x84) & 0xfff0) >> 4;
}

void *cbmem_top(void)
{
	uintptr_t tolm;
	uintptr_t fb_size;

	tolm = vx900_get_tolm ();
	fb_size = vx900_get_chrome9hd_fb_size ();

	if (tolm > 0xfc0 || tolm <= 0x3ff || fb_size == 0x0)
		return NULL;

	return (void *)((tolm - fb_size) << 20);
}
