/*
 * This file is part of the coreboot project.
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

#include <console/console.h>
#include <device/device.h>

#include "nct7802y.h"
#include "chip.h"

static void nct7802y_init(struct device *const dev)
{
	if (!dev->chip_info) {
		printk(BIOS_WARNING,
		       "NCT7802Y driver selected but not configured.");
		return;
	}

	nct7802y_init_peci(dev);
	nct7802y_init_fan(dev);
}

static struct device_operations nct7802y_ops = {
	.read_resources		= DEVICE_NOOP,
	.set_resources		= DEVICE_NOOP,
	.enable_resources	= DEVICE_NOOP,
	.init			= nct7802y_init,
};

static void nct7802y_enable(struct device *const dev)
{
	dev->ops = &nct7802y_ops;
}

struct chip_operations drivers_i2c_nct7802y_ops = {
	CHIP_NAME("NCT7802Y")
	.enable_dev = nct7802y_enable
};
