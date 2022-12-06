/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/acpimmio.h>
#include <arch/io.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <psp_verstage.h>
#include <security/vboot/vboot_common.h>
#include <soc/espi.h>
#include <soc/southbridge.h>

void verstage_mainboard_early_init(void)
{
	const struct soc_amd_gpio *gpios, *override_gpios;
	size_t num_gpios, override_num_gpios;

	variant_early_gpio_table(&gpios, &num_gpios);
	variant_early_override_gpio_table(&override_gpios, &override_num_gpios);
	gpio_configure_pads_with_override(gpios, num_gpios, override_gpios, override_num_gpios);
}

void verstage_mainboard_espi_init(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	variant_espi_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	espi_switch_to_spi1_pads();
}

void verstage_mainboard_tpm_init(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	variant_tpm_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}
