/*
 * This file is part of the coreboot project.
 *
 * Copyright 2020 The coreboot project Authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <console/console.h>
#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/ec.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <smbios.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <variant/gpio.h>

#define SKU_UNKNOWN		0xFFFFFFFF
#define SKU_MAX			0x7FFFFFFF

static uint32_t get_board_sku(void)
{
	static uint32_t sku_id = SKU_UNKNOWN;

	if (sku_id != SKU_UNKNOWN)
		return sku_id;

	if (google_chromeec_cbi_get_sku_id(&sku_id))
		sku_id = SKU_UNKNOWN;

	return sku_id;
}

const char *smbios_system_sku(void)
{
	static char sku_str[14]; /* sku{0..2147483647} */
	uint32_t sku_id = get_board_sku();

	if ((sku_id == SKU_UNKNOWN) || (sku_id > SKU_MAX)) {
		printk(BIOS_ERR, "%s: Unexpected SKU ID %u\n",
			__func__, sku_id);
		return "";
	}

	snprintf(sku_str, sizeof(sku_str), "sku%u", sku_id);

	return sku_str;
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
}

static void mainboard_chip_init(void *chip_info)
{
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	base_pads = variant_base_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num,
		override_pads, override_num);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
