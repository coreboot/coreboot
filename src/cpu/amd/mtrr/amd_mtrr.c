/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/biosram.h>
#include <console/console.h>
#include <device/device.h>
#include <cpu/amd/mtrr.h>

void add_uma_resource_below_tolm(struct device *nb, int idx)
{
	uint32_t topmem = amd_topmem();
	uint32_t top_of_cacheable = restore_top_of_low_cacheable();

	if (top_of_cacheable == topmem)
		return;

	uint32_t uma_base = top_of_cacheable;
	uint32_t uma_size = topmem - top_of_cacheable;

	printk(BIOS_INFO, "%s: uma size 0x%08x, memory start 0x%08x\n", __func__, uma_size,
	       uma_base);

	uma_resource_kb(nb, idx, uma_base / KiB, uma_size / KiB);
}
