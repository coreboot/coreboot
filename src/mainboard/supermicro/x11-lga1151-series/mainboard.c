/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <mainboard/gpio.h>

static void mainboard_chip_init(void *chip_info)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
};
