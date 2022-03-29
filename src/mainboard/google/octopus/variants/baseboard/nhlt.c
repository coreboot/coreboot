/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/cbi_ssfc.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <nhlt.h>
#include <soc/nhlt.h>

void __weak variant_nhlt_init(struct nhlt *nhlt)
{
	enum ssfc_audio_codec codec = ssfc_get_audio_codec();

	/* 2 Channel DMIC array. */
	if (!nhlt_soc_add_dmic_array(nhlt, 2))
		printk(BIOS_INFO, "Added 2CH DMIC array.\n");

	/* 4 Channel DMIC array. */
	if (!nhlt_soc_add_dmic_array(nhlt, 4))
		printk(BIOS_INFO, "Added 4CH DMIC arrays.\n");

	/*
	 * Headset codec is bi-directional but uses the same configuration
	 * settings for render and capture endpoints.
	 */
	if (CONFIG(NHLT_DA7219) && codec == SSFC_AUDIO_CODEC_DA7219) {
		/* Dialog for Headset codec */
		if (!nhlt_soc_add_da7219(nhlt, AUDIO_LINK_SSP2))
			printk(BIOS_INFO, "Added Dialog_7219 codec.\n");
	}

	if (CONFIG(NHLT_RT5682) &&
			(codec == SSFC_AUDIO_CODEC_RT5682 ||
			codec == SSFC_AUDIO_CODEC_RT5682_VS)) {
		/* Realtek for Headset codec */
		if (!nhlt_soc_add_rt5682(nhlt, AUDIO_LINK_SSP2))
			printk(BIOS_INFO, "Added ALC5682 codec.\n");
	}

	/* MAXIM Smart Amps for left and right speakers. */
	if (!nhlt_soc_add_max98357(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_INFO, "Added Maxim_98357 codec.\n");
}
