/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/gpio.h>
#include <stdint.h>
#include <stdio.h>

static void mainboard_init(void *chip_info)
{
	variant_configure_gpio_pads();

	variant_devtree_update();
}

void __weak variant_devtree_update(void)
{
	/* Override dev tree settings per board */
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
