/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
#include <baseboard/variants.h>
#include <console/console.h>
#include <nhlt.h>
#include <soc/nhlt.h>

void __weak variant_nhlt_init(struct nhlt *nhlt)
{

	/* RT5663 Headset codec */
	if (nhlt_soc_add_rt5663(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add headset codec.\n");

}

void __weak variant_nhlt_oem_overrides(const char **oem_id,
						const char **oem_table_id,
						uint32_t *oem_revision)
{
	*oem_id = "GOOGLE";
	*oem_table_id = "FIZZ";
	*oem_revision = 0;
}
