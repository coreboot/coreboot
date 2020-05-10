/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include "gpio.h"

static void mainboard_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;

	/* Configure GPIOs in Ramstage */
	pads = gpio_table(&num);
	gpio_configure_pads(pads, num);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
