/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/espi.h>
#include <bootblock_common.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <soc/espi.h>
#include <spi_flash.h>

void mb_set_up_early_espi(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	variant_espi_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	espi_switch_to_spi1_pads();
}

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	variant_tpm_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	variant_early_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}

void bootblock_mainboard_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	variant_bootblock_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	// TODO: b:285110121 - temp workaround is to add a dummy flash call
	const struct spi_flash *spi = boot_device_spi_flash();
	printk(BIOS_DEBUG, "Initialized spi flash %p\n", spi);
}
