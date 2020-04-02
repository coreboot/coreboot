/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <mainboard.h>
#include <device/device.h>

__weak void variant_mainboard_init(struct device *dev)
{
}

static void mainboard_init(struct device *dev)
{
	/* do common init */
	// placeholder for common mainboard initialization

	/* do variant init */
	variant_mainboard_init(dev);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
