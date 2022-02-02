/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>

static void mainboard_init(void *chip_info)
{
	/* TODO: Perform mainboard initialization */
}

static void mainboard_enable(struct device *dev)
{
	/* TODO: Enable mainboard */
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
