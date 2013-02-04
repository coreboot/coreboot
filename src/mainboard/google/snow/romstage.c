/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <system.h>

#include <cache.h>
#include <cbfs.h>
#include <common.h>

#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/setup.h>

#include <console/console.h>
#include <arch/stages.h>

#include "mainboard.h"

void main(void)
{
	struct mem_timings *mem;
	int ret;

	console_init();
	printk(BIOS_INFO, "hello from romstage\n");

	mem = get_mem_timings();
	if (!mem) {
		printk(BIOS_CRIT, "Unable to auto-detect memory timings\n");
		while(1);
	}
	printk(BIOS_SPEW, "man: 0x%x type: 0x%x, div: 0x%x, mhz: 0x%x\n",
		mem->mem_manuf,
		mem->mem_type,
		mem->mpll_mdiv,
		mem->frequency_mhz);

	ret = ddr3_mem_ctrl_init(mem, DMC_INTERLEAVE_SIZE);
	if (ret) {
		printk(BIOS_ERR, "Memory controller init failed, err: %x\n",
		ret);
		while(1);
	}

	printk(BIOS_INFO, "ddr3_init done\n");

	void *entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA,
				      "fallback/coreboot_ram");
	printk(BIOS_INFO, "entry is 0x%p, leaving romstage.\n", entry);

	stage_exit((unsigned long)entry);
}
