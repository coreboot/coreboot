/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>

void variant_configure_pads(void)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	struct pad_config *padbased_table;
	size_t base_num, override_num;

	padbased_table = new_padbased_table();
	base_pads = variant_gpio_table(&base_num);
	gpio_padbased_override(padbased_table, base_pads, base_num);
	override_pads = variant_gpio_override_table(&override_num);
	gpio_padbased_override(padbased_table, override_pads, override_num);
	fw_config_gpio_padbased_override(padbased_table);
	gpio_configure_pads_with_padbased(padbased_table);
	free(padbased_table);
}
