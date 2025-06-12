/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <nhlt.h>
#include <soc/nhlt.h>

void variant_nhlt_init(struct nhlt *nhlt)
{
	/* 2 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 2))
		printk(BIOS_ERR, "Couldn't add 2CH DMIC array.\n");

	/* Dialog DA7219 Headset codec. */
	if (nhlt_soc_add_da7219(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add Dialog DA7219.\n");

	/* MAXIM Smart Amps for left and right speakers. */
	if (nhlt_soc_add_max98357(nhlt, AUDIO_LINK_SSP0))
		printk(BIOS_ERR, "Couldn't add  Maxim_98357 codec.\n");
}

void variant_nhlt_oem_overrides(const char **oem_id,
					const char **oem_table_id,
					uint32_t *oem_revision)
{
	*oem_id = "GOOGLE";
	*oem_table_id = "KALISTA";
	*oem_revision = 0;
}
