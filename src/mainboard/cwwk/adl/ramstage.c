/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <mainboard/gpio.h>
#include <soc/ramstage.h>

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
