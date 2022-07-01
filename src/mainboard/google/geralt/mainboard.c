/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

static void mainboard_init(struct device *dev)
{
	/* TODO: add mainboard init */
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
