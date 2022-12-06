/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <gpio.h>
#include <psp_verstage.h>
#include <security/vboot/vboot_common.h>

static void setup_gpio(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	printk(BIOS_DEBUG, "Setting GPIOs\n");
	gpios = variant_early_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
	printk(BIOS_DEBUG, "GPIOs setup\n");
}

void verstage_mainboard_early_init(void)
{
	setup_gpio();
}

void verstage_mainboard_espi_init(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	if (!CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK))
		return;

	gpios = variant_espi_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}

void verstage_mainboard_tpm_init(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	if (!CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK))
		return;

	gpios = variant_tpm_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}
