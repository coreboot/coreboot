/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <soc/ramstage.h>

#include "gpio.h"

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	/*
	 * Configure pads prior to SiliconInit() in case there are any
	 * dependencies during hardware initialization.
	 */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
