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
 */


#include <arch/io.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/id.h>
#include <soc/mc.h>
#include <soc/sdram.h>
#include <stdlib.h>
#include <symbols.h>

static uintptr_t tz_base_mib;
static const size_t tz_size_mib = CONFIG_TRUSTZONE_CARVEOUT_SIZE_MB;

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
	bom |= bom_hi << (32 - 20);

	*base_mib = bom;
}

void carveout_range(int id, uintptr_t *base_mib, size_t *size_mib)
{
	*base_mib = 0;
	*size_mib = 0;
	struct tegra_mc_regs * const mc = (struct tegra_mc_regs *)TEGRA_MC_BASE;

	switch (id) {
	case CARVEOUT_TZ:
		*base_mib = tz_base_mib;
		*size_mib = tz_size_mib;
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

static void memory_in_range(uintptr_t *base_mib, uintptr_t *end_mib,
				int ignore_tz)
{
	uintptr_t base;
	uintptr_t end;
	int i;

	base = (uintptr_t)_dram / MiB;
	end = base + sdram_size_mb();

	/* Requested limits out of range. */
	if (*end_mib <= base || *base_mib >= end) {
		*end_mib = *base_mib = 0;
		return;
	}

	/* Clip region to passed in limits. */
	if (*end_mib < end)
		end = *end_mib;
	if (*base_mib > base)
		base = *base_mib;

	for (i = 0; i < CARVEOUT_NUM; i++) {
		uintptr_t carveout_base;
		uintptr_t carveout_end;
		size_t carveout_size;

		if (i == CARVEOUT_TZ && ignore_tz)
			continue;

		carveout_range(i, &carveout_base, &carveout_size);

		if (carveout_size == 0)
			continue;

		carveout_end = carveout_base + carveout_size;

		/* Bypass carveouts out of requested range. */
		if (carveout_base >= end || carveout_end <= base)
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

void memory_in_range_below_4gb(uintptr_t *base_mib, uintptr_t *end_mib)
{
	*base_mib = 0;
	*end_mib = 4096;
	memory_in_range(base_mib, end_mib, 0);
}

void memory_in_range_above_4gb(uintptr_t *base_mib, uintptr_t *end_mib)
{
	*base_mib = 4096;
	*end_mib = ~0UL;
	memory_in_range(base_mib, end_mib, 0);
}

void trustzone_region_init(void)
{
	struct tegra_mc_regs * const mc = (void *)(uintptr_t)TEGRA_MC_BASE;
	uintptr_t end = 4096;

	/* Already has been initialized. */
	if (tz_size_mib != 0 && tz_base_mib != 0)
		return;

	/*
	 * Get memory layout below 4GiB ignoring the TZ carveout because
	 * that's the one to initialize.
	 */
	memory_in_range(&tz_base_mib, &end, 1);
	tz_base_mib = end - tz_size_mib;

	/* AVP cannot set the TZ registers proper as it is always non-secure. */
	if (context_avp())
		return;

	/* Set the carveout region. */
	write32(&mc->security_cfg0, tz_base_mib << 20);
	write32(&mc->security_cfg1, tz_size_mib);

	/* Enable SMMU translations */
	write32(&mc->smmu_config, MC_SMMU_CONFIG_ENABLE);
}
