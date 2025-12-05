/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>

static void mainboard_enable(struct device *dev)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
