/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <pc80/keyboard.h>

static void mainboard_init(struct device *dev)
{
	pc_keyboard_init(NO_AUX_DEVICE);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
