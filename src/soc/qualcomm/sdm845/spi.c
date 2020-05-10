/* SPDX-License-Identifier: GPL-2.0-only */

#include <spi-generic.h>
#include <spi_flash.h>
#include <soc/qspi.h>

static const struct spi_ctrlr spi_ctrlr = {
	.claim_bus = sdm845_claim_bus,
	.release_bus = sdm845_release_bus,
	.xfer = sdm845_xfer,
	.xfer_dual = sdm845_xfer_dual,
	.max_xfer_size = QSPI_MAX_PACKET_COUNT,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
		.bus_end = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
