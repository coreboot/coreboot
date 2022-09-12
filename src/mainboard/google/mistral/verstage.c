/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <security/vboot/vboot_common.h>
#include <soc/clock.h>
#include <spi-generic.h>

void verstage_mainboard_init(void)
{
	struct spi_slave spi;

	printk(BIOS_INFO, "Trying to initialize TPM SPI bus\n");
	if (spi_setup_slave(CONFIG_DRIVER_TPM_SPI_BUS,
			    CONFIG_DRIVER_TPM_SPI_CHIP, &spi)) {
		printk(BIOS_ERR, "Failed to setup TPM SPI slave\n");
	}
}
