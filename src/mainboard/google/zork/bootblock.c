/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <baseboard/variants.h>
#include <acpi/acpi.h>
#include <amdblocks/espi.h>

void mb_set_up_early_espi(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	gpios = variant_espi_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	/*
	 * The GPIOs below would normally be programmed in verstage, but
	 * if we're not using PSP verstage, need to set them up here instead.
	 */
	if (!CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		gpios = variant_early_gpio_table(&num_gpios);
		gpio_configure_pads(gpios, num_gpios);

		gpios = variant_tpm_gpio_table(&num_gpios);
		gpio_configure_pads(gpios, num_gpios);

	}

	gpios = variant_bootblock_gpio_table(&num_gpios, acpi_get_sleep_type());
	gpio_configure_pads(gpios, num_gpios);

	baseboard_pcie_gpio_configure();
}
