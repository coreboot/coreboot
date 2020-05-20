/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/ec.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/romstage.h>
#include <variant/ec.h>
#include <console/console.h>

void __weak variant_romstage_entry(void)
{
	/* By default, don't do anything */
}

void mainboard_romstage_entry_s3(int s3_resume)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	gpios = variant_romstage_gpio_table(&num_gpios);
	program_gpios(gpios, num_gpios);
	gpios = variant_wifi_romstage_gpio_table(&num_gpios);
	program_gpios(gpios, num_gpios);

	mainboard_ec_init();

	variant_romstage_entry();
}
