/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/gpio.h>

static void mainboard_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
