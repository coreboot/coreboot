/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard.h>
#include <device/device.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>

__weak void variant_mainboard_init(struct device *dev)
{
}

static void mainboard_init(struct device *dev)
{
	/* do common init */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

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
