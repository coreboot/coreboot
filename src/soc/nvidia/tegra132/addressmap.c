/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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


#include <arch/io.h>
#include <stdlib.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include "mc.h"
#include "sdram.h"

/* returns total amount of DRAM (in MB) from memory controller registers */
int sdram_size_mb(void)
{
	struct tegra_mc_regs *mc = (struct tegra_mc_regs *)TEGRA_MC_BASE;
	static int total_size = 0;

	if (total_size)
		return total_size;

	/*
	 * This obtains memory size from the External Memory Aperture
	 * Configuration register. Nvidia confirmed that it is safe to assume
	 * this value represents the total physical DRAM size.
	 */
	total_size = (read32(&mc->emem_cfg) >> MC_EMEM_CFG_SIZE_MB_SHIFT) &
		      MC_EMEM_CFG_SIZE_MB_MASK;

	printk(BIOS_DEBUG, "%s: Total SDRAM (MB): %u\n", __func__, total_size);
	return total_size;
}

uintptr_t sdram_max_addressable_mb(void)
{
	return MIN((CONFIG_SYS_SDRAM_BASE/MiB) + sdram_size_mb(), 4096);
}
