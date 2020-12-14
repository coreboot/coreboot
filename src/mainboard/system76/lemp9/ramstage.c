/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	cnl_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
