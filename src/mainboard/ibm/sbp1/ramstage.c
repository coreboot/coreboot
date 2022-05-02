/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

#include "include/spr_sbp1_gpio.h"

void mainboard_silicon_init_params(FSPS_UPD *params)
{
	/* configure Emmitsburg PCH GPIO controller after FSP-M */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
