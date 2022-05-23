/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>

static void mainboard_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;
	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

static void mainboard_enable(struct device *dev)
{
	/* TODO: Enable mainboard */
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
