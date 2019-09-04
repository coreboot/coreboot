/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <mcall.h>
#include <device/mmio.h>

#define SPIKE_CLINT_BASE	0x02000000

/* This function is used to initialize HLS()->time/HLS()->timecmp  */
void mtime_init(void)
{
	long hart_id = read_csr(mhartid);
	HLS()->time = (uint64_t *)(SPIKE_CLINT_BASE + 0xbff8);
	HLS()->timecmp = (uint64_t *)(SPIKE_CLINT_BASE + 0x4000 + 8 * hart_id);
}

void set_msip(int hartid, int val)
{
	write32((void *)(SPIKE_CLINT_BASE + 4 * (uintptr_t)hartid), !!val);
}
