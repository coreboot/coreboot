/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/ramstage.h>
#include "gpio.h"

static void mainboard_init(void *chip_info)
{

}

static void mainboard_enable(struct device *dev)
{

}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
