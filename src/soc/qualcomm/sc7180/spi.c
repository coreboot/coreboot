/*
 * This file is part of the coreboot project.
 *
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
#include <soc/qspi.h>
#include <soc/qupv3_spi.h>

static const struct spi_ctrlr qspi_ctrlr = {
	.claim_bus = sc7180_claim_bus,
	.release_bus = sc7180_release_bus,
	.xfer = sc7180_xfer,
	.xfer_dual = sc7180_xfer_dual,
	.max_xfer_size = QSPI_MAX_PACKET_COUNT,
};

const struct spi_ctrlr spi_qup_ctrlr = {
	.claim_bus = qup_spi_claim_bus,
	.release_bus = qup_spi_release_bus,
	.xfer = qup_spi_xfer,
	.max_xfer_size = 65535,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &qspi_ctrlr,
		.bus_start = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
		.bus_end = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
	},
	{
		.ctrlr = &spi_qup_ctrlr,
		.bus_start = 0,
		.bus_end = 11,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
