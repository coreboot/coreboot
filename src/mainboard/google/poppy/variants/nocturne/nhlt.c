/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <nhlt.h>
#include <soc/nhlt.h>

void variant_nhlt_init(struct nhlt *nhlt)
{
	/* 4 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 4))
		printk(BIOS_ERR, "Couldn't add 4CH DMIC array.\n");

	/* MAXIM Smart Amps for left and right speakers. */
	/* Render time_slot is 0 and feedback time_slot is 2 */
	if (nhlt_soc_add_max98373(nhlt, AUDIO_LINK_SSP0, 0, 2))
		printk(BIOS_ERR, "Couldn't add Maxim 98373 codec.\n");
}

void variant_nhlt_oem_overrides(const char **oem_id, const char **oem_table_id,
				uint32_t *oem_revision)
{
	*oem_id = "GOOGLE";
	*oem_table_id = "NOCTURNE";
	*oem_revision = 0;
}
