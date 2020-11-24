/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard.h>
#include <device/device.h>
#include <mainboard/gpio.h>

__weak void variant_mainboard_init(void *chip_info)
{
}

static void mainboard_chip_init(void *chip_info)
{
	/* do common init */
	mainboard_configure_gpios();

	/* do variant init */
	variant_mainboard_init(chip_info);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
};
