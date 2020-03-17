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
 *
 */

#include <console/console.h>
#include <security/vboot/vboot_common.h>
#include <soc/clock.h>
#include <spi-generic.h>

void verstage_mainboard_init(void)
{
	struct spi_slave spi;

	printk(BIOS_ERR, "Trying to initialize TPM SPI bus\n");
	if (spi_setup_slave(CONFIG_DRIVER_TPM_SPI_BUS,
			    CONFIG_DRIVER_TPM_SPI_CHIP, &spi)) {
		printk(BIOS_ERR, "Failed to setup TPM SPI slave\n");
	}
}
