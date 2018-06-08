/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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
#include <compiler.h>
#include <console/console.h>
#include <nhlt.h>
#include <soc/nhlt.h>

void __weak variant_nhlt_init(struct nhlt *nhlt)
{
	/* 2 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 2))
		printk(BIOS_ERR, "Couldn't add 2CH DMIC array.\n");

	/* 4 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 4))
		printk(BIOS_ERR, "Couldn't add 4CH DMIC arrays.\n");

	/* Maxim MAX98927 Smart Amps for left and right channel */
	if (nhlt_soc_add_max98927(nhlt, AUDIO_LINK_SSP0))
		printk(BIOS_ERR, "Couldn't add Maxim MAX98927\n");

	/* Realtek RT5663 Headset codec. */
	if (nhlt_soc_add_rt5663(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add Realtek RT5663.\n");
}

void __weak variant_nhlt_oem_overrides(const char **oem_id,
						const char **oem_table_id,
						uint32_t *oem_revision)
{
	*oem_id = "GOOGLE";
	*oem_table_id = "POPPYMAX";
	*oem_revision = 0;
}
