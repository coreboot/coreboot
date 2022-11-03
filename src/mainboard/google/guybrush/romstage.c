/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/platform_descriptors.h>

void mb_pre_fspm(void)
{
	size_t base_num_gpios, override_num_gpios;
	const struct soc_amd_gpio *base_gpios, *override_gpios;

	/* Initialize PCIe reset. */
	base_gpios = baseboard_pcie_gpio_table(&base_num_gpios);
	override_gpios = variant_pcie_override_gpio_table(&override_num_gpios);

	gpio_configure_pads_with_override(base_gpios, base_num_gpios,
			override_gpios, override_num_gpios);

}
