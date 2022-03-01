/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>
#include <variant/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	variant_configure_fsps(params);
}

static void init_mainboard(void *chip_info)
{
	variant_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = init_mainboard,
};
