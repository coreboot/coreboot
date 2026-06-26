/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <cpu/cpu.h>
#include <cpu/intel/cpu_ids.h>
#include <device/device.h>
#include <drivers/intel/gma/opregion.h>
#include <fw_config.h>
#include <smbios.h>
#include <soc/gpio.h>
#include <stdint.h>
#include <stdio.h>

const char *smbios_system_sku(void)
{
	static char sku_str[7] = "";
	uint8_t sku_id = 1;

	snprintf(sku_str, sizeof(sku_str), "sku%u", sku_id);
	return sku_str;
}

static void mainboard_init(void *chip_info)
{
	const struct pad_config *base_pads, *override_pads;
	size_t base_num, override_num;

	base_pads = baseboard_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num, override_pads, override_num);

	variant_devtree_update();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
