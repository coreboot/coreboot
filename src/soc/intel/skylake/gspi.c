/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <device/device.h>
#include <intelblocks/chip.h>
#include <intelblocks/gspi.h>
#include <intelblocks/spi.h>
#include <soc/iomap.h>
#include "chip.h"

const struct gspi_cfg *gspi_get_soc_cfg(void)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return &common_config->gspi[0];
}

uintptr_t gspi_get_soc_early_base(void)
{
	return EARLY_GSPI_BASE_ADDRESS;
}

/*
 * SPI Bus 0 is Fast SPI and GSPI starts from SPI bus # 1 onwards. Thus, adjust
 * the bus # accordingly when referring to SPI / GSPI bus numbers.
 */
#define GSPI_TO_SPI_BUS(x)	(x + 1)
#define SPI_TO_GSPI_BUS(x)	(x - 1)

int gspi_soc_spi_to_gspi_bus(unsigned int spi_bus, unsigned int *gspi_bus)
{
	if (spi_bus == 0)
		return -1;

	*gspi_bus = SPI_TO_GSPI_BUS(spi_bus);
	if (*gspi_bus >= CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX)
		return -1;

	return 0;
}

int gspi_soc_bus_to_devfn(unsigned int gspi_bus)
{
	if (gspi_bus >= CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX)
		return -1;

	return spi_soc_bus_to_devfn(GSPI_TO_SPI_BUS(gspi_bus));
}
