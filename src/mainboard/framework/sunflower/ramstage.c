/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <ec/ec.h>
#include <mainboard/ramstage.h>

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
}

struct chip_operations mainboard_ops = {.init = mainboard_init};
