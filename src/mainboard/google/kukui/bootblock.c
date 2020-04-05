/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <soc/spi.h>
#include <soc/gpio.h>

void bootblock_mainboard_init(void)
{
	mtk_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, SPI_PAD0_MASK, 6 * MHz,
		     0);
	mtk_spi_init(CONFIG_BOOT_DEVICE_SPI_FLASH_BUS, SPI_PAD0_MASK, 56 * MHz,
		     2);
	gpio_set_spi_driving(CONFIG_BOOT_DEVICE_SPI_FLASH_BUS, SPI_PAD0_MASK,
			     10);

}
