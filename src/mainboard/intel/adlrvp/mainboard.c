/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/ec.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <smbios.h>
#include <stdint.h>
#include <string.h>

#include "board_id.h"

const char *smbios_system_sku(void)
{
	static char sku_str[7] = "";
	uint8_t sku_id = get_board_id();

	snprintf(sku_str, sizeof(sku_str), "sku%u", sku_id);
	return sku_str;
}

static void mainboard_init(void *chip_info)
{
	variant_configure_gpio_pads();

	if (CONFIG(EC_GOOGLE_CHROMEEC))
		mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
