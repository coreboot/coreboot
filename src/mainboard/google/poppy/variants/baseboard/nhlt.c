/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
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
	/* Render time_slot is 0 and feedback time_slot is 2 */
	if (nhlt_soc_add_max98927(nhlt, AUDIO_LINK_SSP0, 0, 2))
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
