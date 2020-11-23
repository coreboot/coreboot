/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard.h>
#include <device/device.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>

__weak void variant_mainboard_init(void *chip_info)
{
}

static void mainboard_chip_init(void *chip_info)
{
	/* do common init */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	/* do variant init */
	variant_mainboard_init(chip_info);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
};
