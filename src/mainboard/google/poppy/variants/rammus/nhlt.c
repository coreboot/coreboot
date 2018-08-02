/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google Inc.
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

#include <baseboard/variants.h>
#include <console/console.h>
#include <nhlt.h>
#include <soc/nhlt.h>

void variant_nhlt_init(struct nhlt *nhlt)
{
	/* 4 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 4))
		printk(BIOS_ERR, "Couldn't add 4CH DMIC array.\n");

	/* Dialog DA7219 Headset codec. */
	if (nhlt_soc_add_da7219(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add Dialog DA7219.\n");

	/* Maxim MAX98927 Smart Amps for left and right channel */
	if (nhlt_soc_add_max98927(nhlt, AUDIO_LINK_SSP0))
		printk(BIOS_ERR, "Couldn't add Maxim MAX98927\n");

}

void variant_nhlt_oem_overrides(const char **oem_id, const char **oem_table_id,
				uint32_t *oem_revision)
{
	*oem_id = "GOOGLE";
	*oem_table_id = "RAMMUSMAX";
	*oem_revision = 0;
}
