/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 13.9
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/flash_controller_common.h>
#include <soc/spi.h>
#include <spi_flash.h>

static const struct pad_func nor_pinmux[4] = {
	PAD_FUNC(SDA10, SF_CK, GPIO_PULL_DOWN),
	PAD_FUNC(SCL10, SF_CS, GPIO_PULL_UP),
	PAD_FUNC(PERIPHERAL_EN5, SF_D0, GPIO_PULL_DOWN),
	PAD_FUNC(PERIPHERAL_EN6, SF_D1, GPIO_PULL_DOWN),
};

void mtk_snfc_init(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux); i++)
		mtk_snfc_init_pad_func(&nor_pinmux[i], GPIO_DRV_14_MA);
}

static const struct spi_ctrlr spi_flash_ctrlr = {
	.max_xfer_size = 65535,
	.flash_probe = mtk_spi_flash_probe,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_flash_ctrlr,
		.bus_start = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
		.bus_end = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
