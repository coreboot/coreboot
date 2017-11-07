/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <console/console.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/spi.h>
#include <soc/pci_devs.h>
#include <spi-generic.h>

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCH_DEVFN_SPI0:
		return 0;
	case PCH_DEVFN_SPI1:
		return 1;
	case PCH_DEVFN_SPI2:
		return 2;
	}
	return -1;
}

int spi_soc_bus_to_devfn(unsigned int bus)
{
	switch (bus) {
	case 0:
		return PCH_DEVFN_SPI0;
	case 1:
		return PCH_DEVFN_SPI1;
	case 2:
		return PCH_DEVFN_SPI2;
	}
	return -1;
}

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{ .ctrlr = &fast_spi_flash_ctrlr, .bus_start = 0, .bus_end = 0 },
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
