/* SPDX-License-Identifier: GPL-2.0-only */

#include <spi-generic.h>
#include <spi_flash.h>
#include <soc/qcom_qup_se.h>
#include <soc/qspi_common.h>
#include <soc/qupv3_spi_common.h>

static const struct spi_ctrlr qspi_ctrlr = {
	.claim_bus = qspi_claim_bus,
	.release_bus = qspi_release_bus,
	.xfer = qspi_xfer,
	.xfer_dual = qspi_xfer_dual,
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
		.bus_end = QUPV3_SE_MAX - 1,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
