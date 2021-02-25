/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <device/device.h>

static void init_mainboard(void *chip_info)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = init_mainboard,
};
