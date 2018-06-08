/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <spi-generic.h>
#include <spi_flash.h>

static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	return 0;
}

static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{

}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			  size_t bytes_out, void *din, size_t bytes_in)
{
	return 0;
}

static const struct spi_ctrlr spi_ctrlr = {
	.claim_bus = spi_ctrlr_claim_bus,
	.release_bus = spi_ctrlr_release_bus,
	.xfer = spi_ctrlr_xfer,
	.max_xfer_size = 65535,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = 0,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
