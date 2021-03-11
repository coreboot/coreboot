/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/gpio_banks.h>
#include <baseboard/variants.h>
#include <security/vboot/vboot_common.h>

static void setup_gpio(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		gpios = variant_early_gpio_table(&num_gpios);
		program_gpios(gpios, num_gpios);
	}
}

void verstage_mainboard_early_init(void)
{
	setup_gpio();
}

void verstage_mainboard_init(void)
{
}
