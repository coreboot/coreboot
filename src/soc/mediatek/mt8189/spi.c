/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.17
 */

#include <gpio.h>
#include <soc/flash_controller_common.h>
#include <soc/spi.h>
#include <spi_flash.h>

static const struct pad_func nor_pinmux[4] = {
	PAD_FUNC(SPINOR_CK, SPINOR_CK, GPIO_PULL_DOWN),
	PAD_FUNC(SPINOR_CS, SPINOR_CS, GPIO_PULL_UP),
	PAD_FUNC(SPINOR_IO0, SPINOR_IO0, GPIO_PULL_DOWN),
	PAD_FUNC(SPINOR_IO1, SPINOR_IO1, GPIO_PULL_DOWN),
};

void mtk_snfc_init(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux); i++)
		mtk_snfc_init_pad_func(&nor_pinmux[i], GPIO_DRV_8_MA);
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
