/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <bootstate.h>
#include <soc/ramstage.h>

#include "gpio.h"

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	/*
	 * Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization.
	 */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	params->TurboMode = 1;
	params->PchThermalDeviceEnable = 0;
	params->PchPort61hEnable = 1;
	params->CdClock = 3;
}
