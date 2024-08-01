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
	variant_configure_gpio_pads();
	variant_devtree_update();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
