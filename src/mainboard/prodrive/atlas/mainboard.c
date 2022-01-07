/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <stdint.h>

#include "gpio.h"

static void mainboard_init(void *chip_info)
{
	configure_gpio_pads();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
