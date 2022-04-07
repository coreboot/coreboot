/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

static void mainboard_init(void *chip_info)
{

}

static void mainboard_enable(struct device *dev)
{

}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
