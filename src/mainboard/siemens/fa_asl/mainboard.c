/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>

static void mainboard_init(void *chip_info)
{
	variant_configure_gpio_pads();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
