/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/fsp.h>
#include <soc/platform_descriptors.h>

void __weak variant_updm_update(FSP_M_CONFIG *mcfg) {}

void mainboard_updm_update(FSP_M_CONFIG *mcfg)
{
	variant_updm_update(mcfg);
}

void mb_pre_fspm(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	gpios = baseboard_romstage_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}
