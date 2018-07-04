/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
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

#include <console/console.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>

const struct soc_intel_common_config *chip_get_common_soc_structure(void)
{
	const struct soc_intel_common_config *soc_config;
	const config_t *config;
	int devfn = SA_DEVFN_ROOT;
	const struct device *dev = dev_find_slot(0, devfn);

	if (!dev || !dev->chip_info)
		die("Could not find SA_DEV_ROOT devicetree config!\n");

	config = dev->chip_info;
	soc_config = &config->common_soc_config;

	return soc_config;
}

/*
 * This function will get MP Init config
 *
 * Return values:
 * 0 = Make use of coreboot MP Init
 * 1 = Make use of FSP MP Init
 */
int chip_get_fsp_mp_init(void)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return common_config->use_fsp_mp_init;
}
