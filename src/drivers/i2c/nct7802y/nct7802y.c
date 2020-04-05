/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
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
