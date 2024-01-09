/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>
#include "gpio.h"

static void mainboard_init(void *chip_info)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
