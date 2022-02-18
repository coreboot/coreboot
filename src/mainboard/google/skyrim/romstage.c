/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/platform_descriptors.h>

void mb_pre_fspm(void)
{
	size_t base_num_gpios;
	const struct soc_amd_gpio *base_gpios;

	/* Initialize PCIe reset. */
	variant_pcie_gpio_table(&base_gpios, &base_num_gpios);

	gpio_configure_pads(base_gpios, base_num_gpios);
}
