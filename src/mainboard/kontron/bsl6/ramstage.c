/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <mainboard/gpio.h>

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
