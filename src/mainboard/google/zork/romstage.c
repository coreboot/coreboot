/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <soc/romstage.h>
#include <console/console.h>

void mainboard_romstage_entry_s3(int s3_resume)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	gpios = variant_romstage_gpio_table(&num_gpios);
	program_gpios(gpios, num_gpios);
	variant_pcie_power_reset_configure();
}
