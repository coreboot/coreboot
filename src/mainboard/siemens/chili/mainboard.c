/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <mainboard/ec.h>

static void mainboard_dev_init(struct device *dev)
{
	const bool enable_usb =
		CONFIG(BOARD_SIEMENS_CHILI_BASE) || CONFIG(SIEMENS_CHILI_DEBUG_USB);
	ec_enable_devices(enable_usb);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_dev_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
