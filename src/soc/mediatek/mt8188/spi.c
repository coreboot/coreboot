/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.8
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/spi.h>

static const struct spi_ctrlr spi_flash_ctrlr = {
	.max_xfer_size = 65535,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_flash_ctrlr,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
