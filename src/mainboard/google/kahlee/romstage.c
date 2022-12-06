/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/dimm_spd.h>
#include <arch/romstage.h>
#include <baseboard/variants.h>
#include <gpio.h>

int mainboard_read_spd(uint8_t spdAddress, char *buf, size_t len)
{
	return variant_mainboard_read_spd(spdAddress, buf, len);
}

void __weak variant_romstage_entry(void)
{
	/* By default, don't do anything */
}

void mainboard_romstage_entry(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	gpios = baseboard_romstage_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	variant_romstage_entry();
}
