/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/platform_descriptors.h>

void mb_pre_fspm(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	gpios = baseboard_romstage_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}
