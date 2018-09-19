/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi_device.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/i2c/generic/chip.h>
#include <drivers/i2c/hid/chip.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <string.h>
#include <ec/google/chromeec/ec.h>

#define SKU_UNKNOWN     0xFFFFFFFF

extern struct chip_operations drivers_i2c_generic_ops;
extern struct chip_operations drivers_i2c_hid_ops;

void variant_update_devtree(struct device *dev)
{
	uint32_t bid;
	uint32_t sku_id = SKU_UNKNOWN;
	struct device *touchscreen_i2c_host;
	struct device *child;
	const struct bus *children_bus;
	static const struct acpi_gpio new_enable_gpio =
		ACPI_GPIO_OUTPUT_ACTIVE_HIGH(GPIO_146);

	bid = board_id();

	/* Nothing to update. */
	if (bid == UNDEFINED_STRAPPING_ID || bid < 1)
		return;

	touchscreen_i2c_host = dev_find_slot(0, PCH_DEVFN_I2C7);

	if (touchscreen_i2c_host == NULL)
		return;

	/* According to the sku id decide whether update touch
	 * screen device information:
	 * 1. sku id is 1 then dev->enabled = 0.
	 */
	google_chromeec_cbi_get_sku_id(&sku_id);
	if (sku_id == 1) {
		touchscreen_i2c_host->enabled = 0;
		return;
	}

	children_bus = touchscreen_i2c_host->link_list;
	child = NULL;

	/* Find all children on bus to update touchscreen enable gpio. */
	while ((child = dev_bus_each_child(children_bus, child)) != NULL) {
		struct drivers_i2c_generic_config *cfg;

		/* No configration to change. */
		if (child->chip_info == NULL)
			continue;

		if (child->chip_ops == &drivers_i2c_generic_ops)
			cfg = child->chip_info;
		else if (child->chip_ops == &drivers_i2c_hid_ops) {
			struct drivers_i2c_hid_config *hid_cfg;
			hid_cfg = child->chip_info;
			cfg = &hid_cfg->generic;
		} else
			continue;

		/* Update the enable gpio. */
		memcpy(&cfg->enable_gpio, &new_enable_gpio,
			sizeof(new_enable_gpio));
	}
}
