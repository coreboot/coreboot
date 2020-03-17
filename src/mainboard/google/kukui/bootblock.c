/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
