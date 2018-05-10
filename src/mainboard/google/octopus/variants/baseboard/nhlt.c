/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Intel Corp.
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
	if (!nhlt_soc_add_dmic_array(nhlt, 2))
		printk(BIOS_ERR, "Added 2CH DMIC array.\n");

	/* 4 Channel DMIC array. */
	if (!nhlt_soc_add_dmic_array(nhlt, 4))
		printk(BIOS_ERR, "Added 4CH DMIC arrays.\n");

	/*
	 * Headset codec is bi-directional but uses the same configuration
	 * settings for render and capture endpoints.
	 */
	if (IS_ENABLED(CONFIG_NHLT_DA7219)) {
		/* Dialog for Headset codec */
		if (!nhlt_soc_add_da7219(nhlt, AUDIO_LINK_SSP2))
			printk(BIOS_ERR, "Added Dialog_7219 codec.\n");
	}

	if (IS_ENABLED(CONFIG_NHLT_RT5682)) {
		/* Realtek for Headset codec */
		if (!nhlt_soc_add_rt5682(nhlt, AUDIO_LINK_SSP2))
			printk(BIOS_ERR, "Added ALC5682 codec.\n");
	}

	/* MAXIM Smart Amps for left and right speakers. */
	if (!nhlt_soc_add_max98357(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Added Maxim_98357 codec.\n");
}
