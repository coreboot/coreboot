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
#include <spi-generic.h>

/* SPI controller managing the fast SPI. */
static int fast_spi_ctrlr_setup(const struct spi_slave *dev)
{
	if ((dev->bus != 0) && (dev->cs != 0)) {
		printk(BIOS_ERR, "%s: Unsupported device "
		       "bus=0x%x,cs=0x%x!\n", __func__, dev->bus, dev->cs);
		return -1;
	}

	printk(BIOS_INFO, "%s: Found controller for device "
	       "(bus=0x%x,cs=0x%x)!!\n", __func__, dev->bus, dev->cs);
	return 0;
}

static const struct spi_ctrlr fast_spi_ctrlr = {
	.setup = fast_spi_ctrlr_setup,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{ .ctrlr = &fast_spi_ctrlr, .bus_start = 0, .bus_end = 0 },
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
