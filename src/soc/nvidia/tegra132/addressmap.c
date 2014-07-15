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
#include <soc/display.h>
#include <soc/id.h>
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

static void carveout_from_regs(uintptr_t *base_mib, size_t *size_mib,
				uint32_t bom, uint32_t bom_hi, uint32_t size)
{

	/* All size regs of carveouts are in MiB. */
	if (size == 0)
		return;

	*size_mib = size;
	bom >>= 20;
	bom |= bom_hi >> (32 - 20);

	*base_mib = bom;
}

void carveout_range(int id, uintptr_t *base_mib, size_t *size_mib)
{
	*base_mib = 0;
	*size_mib = 0;
	struct tegra_mc_regs * const mc = (struct tegra_mc_regs *)TEGRA_MC_BASE;

	switch (id) {
	case CARVEOUT_TZ:
		/* AVP does not have access to the TZ carveout registers. */
		if (context_avp())
			return;
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->security_cfg0),
					0,
					read32(&mc->security_cfg1));
		break;
	case CARVEOUT_SEC:
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->sec_carveout_bom),
					read32(&mc->sec_carveout_adr_hi),
					read32(&mc->sec_carveout_size_mb));
		break;
	case CARVEOUT_MTS:
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->mts_carveout_bom),
					read32(&mc->mts_carveout_adr_hi),
					read32(&mc->mts_carveout_size_mb));
		break;
	case CARVEOUT_VPR:
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->video_protect_bom),
					read32(&mc->video_protect_bom_adr_hi),
					read32(&mc->video_protect_size_mb));
		break;
	default:
		break;
	}
}

void memory_range_by_bits(int bits, uintptr_t *base_mib, uintptr_t *end_mib)
{
	uintptr_t base;
	uintptr_t end;
	int i;

	base = CONFIG_SYS_SDRAM_BASE / MiB;
	end = base + sdram_size_mb();

	if (bits == ADDRESS_SPACE_32_BIT)
		end = MIN(end, 4096);

	for (i = 0; i < CARVEOUT_NUM; i++) {
		uintptr_t carveout_base;
		size_t carveout_size;

		carveout_range(i, &carveout_base, &carveout_size);

		if (carveout_size == 0)
			continue;

		/* Bypass carveouts out of requested range. */
		if (carveout_base >= end)
			continue;

		/*
		 * This is crude, but the assumption is that carveouts live
		 * at the upper range of physical memory. Therefore, update
		 * the end address to be equal to the base of the carveout.
		 */
		end = carveout_base;
	}

	*base_mib = base;
	*end_mib = end;
}

uintptr_t framebuffer_attributes(size_t *size_mib)
{
	uintptr_t begin;
	uintptr_t end;

	/* Place the framebuffer just below the 32-bit addressable limit. */
	memory_range_by_bits(ADDRESS_SPACE_32_BIT, &begin, &end);

	/*
	 * Need to take into account that the Trust Zone region is not able to
	 * be read by the AVP. The Trust Zone region will live just below the
	 * rest of the carveout regions.
	 */
	if (context_avp())
		end -= CONFIG_TRUSTZONE_CARVEOUT_SIZE_MB;

	*size_mib = FB_SIZE_MB;
	end -= *size_mib;

	return end;
}
