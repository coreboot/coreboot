/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/gpio.h>
#include <smbios.h>

static void mainboard_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

const char *smbios_system_sku(void)
{
	static const char *sku_str = "sku2147483647"; /* sku{0-1} */
	return sku_str;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
