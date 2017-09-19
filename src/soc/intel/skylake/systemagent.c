/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation.
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
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <intelblocks/systemagent.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include "chip.h"

bool soc_is_vtd_capable(void)
{
	struct device *const root_dev = SA_DEV_ROOT;
	return root_dev &&
		!(pci_read_config32(root_dev, CAPID0_A) & VTD_DISABLE);
}

/*
 * SoC implementation
 *
 * Add all known fixed memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_fixed_mmio_resources(struct device *dev, int *index)
{
	static const struct sa_mmio_descriptor soc_fixed_resources[] = {
		{ PCIEXBAR, CONFIG_MMCONF_BASE_ADDRESS, CONFIG_SA_PCIEX_LENGTH,
				"PCIEXBAR" },
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
		{ DMIBAR, DMI_BASE_ADDRESS, DMI_BASE_SIZE, "DMIBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
		{ GDXCBAR, GDXC_BASE_ADDRESS, GDXC_BASE_SIZE, "GDXCBAR" },
		{ EDRAMBAR, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE, "EDRAMBAR" },
	};
	const struct soc_intel_skylake_config *const config = dev->chip_info;

	sa_add_fixed_mmio_resources(dev, index, soc_fixed_resources,
			ARRAY_SIZE(soc_fixed_resources));

	if (!(config && config->ignore_vtd) && soc_is_vtd_capable())
		sa_add_fixed_mmio_resources(dev, index, soc_vtd_resources,
				ARRAY_SIZE(soc_vtd_resources));
}

/*
 * SoC implementation
 *
 * Perform System Agent Initialization during Ramstage phase.
 */
void soc_systemagent_init(struct device *dev)
{
	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Enable BIOS Reset CPL */
	enable_bios_reset_cpl();

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
	set_power_limits(28);
}

int soc_get_uncore_prmmr_base_and_mask(uint64_t *prmrr_base,
	uint64_t *prmrr_mask)
{
	msr_t msr;
	msr = rdmsr(MSR_UNCORE_PRMRR_PHYS_BASE);
	*prmrr_base = (uint64_t) msr.hi << 32 | msr.lo;
	msr = rdmsr(MSR_UNCORE_PRMRR_PHYS_MASK);
	*prmrr_mask = (uint64_t) msr.hi << 32 | msr.lo;
	return 0;
}
