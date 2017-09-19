/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <device/device.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/systemagent.h>
#include "chip.h"

static void systemagent_vtd_init(void)
{
	const struct device *const dev = dev_find_slot(0, SA_DEVFN_ROOT);
	const struct soc_intel_skylake_config *config = NULL;

	if (dev)
		config = dev->chip_info;
	if (config && config->ignore_vtd)
		return;

	const bool vtd_capable =
		!(pci_read_config32(SA_DEV_ROOT, CAPID0_A) & VTD_DISABLE);
	if (!vtd_capable)
		return;

	sa_set_mch_bar(soc_vtd_resources, ARRAY_SIZE(soc_vtd_resources));
}

void systemagent_early_init(void)
{
	static const struct sa_mmio_descriptor soc_fixed_pci_resources[] = {
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
		{ DMIBAR, DMI_BASE_ADDRESS, DMI_BASE_SIZE, "DMIBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
	};

	static const struct sa_mmio_descriptor soc_fixed_mch_resources[] = {
		{ GDXCBAR, GDXC_BASE_ADDRESS, GDXC_BASE_SIZE, "GDXCBAR" },
		{ EDRAMBAR, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE, "EDRAMBAR" },
	};

	/* Set Fixed MMIO address into PCI configuration space */
	sa_set_pci_bar(soc_fixed_pci_resources,
			ARRAY_SIZE(soc_fixed_pci_resources));
	/* Set Fixed MMIO address into MCH base address */
	sa_set_mch_bar(soc_fixed_mch_resources,
			ARRAY_SIZE(soc_fixed_mch_resources));

	systemagent_vtd_init();

	/* Enable PAM registers */
	enable_pam_region();
}
