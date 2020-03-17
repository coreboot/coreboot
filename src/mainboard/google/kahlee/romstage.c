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
 */

#include <amdblocks/dimm_spd.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <soc/romstage.h>

int mainboard_read_spd(uint8_t spdAddress, char *buf, size_t len)
{
	return variant_mainboard_read_spd(spdAddress, buf, len);
}

void __weak variant_romstage_entry(int s3_resume)
{
	/* By default, don't do anything */
}

void mainboard_romstage_entry_s3(int s3_resume)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	gpios = variant_romstage_gpio_table(&num_gpios);
	program_gpios(gpios, num_gpios);

	variant_romstage_entry(s3_resume);
}
