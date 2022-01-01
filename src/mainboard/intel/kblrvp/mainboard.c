/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <gpio.h>
#include <soc/nhlt.h>
#include "ec.h"
#include <variant/gpio.h>

static void mainboard_init(struct device *dev)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		mainboard_ec_init();
}

/*
 * mainboard_enable is executed as first thing after
 * enumerate_buses().
 */
static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
