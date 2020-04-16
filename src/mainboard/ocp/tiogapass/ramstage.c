/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <soc/ramstage.h>
#include <bootstate.h>
#include <gpio.h>
#include <soc/lewisburg_pch_gpio_defs.h>

void mainboard_silicon_init_params(FSPS_UPD *params)
{
}

static void pull_post_complete_pin(void *unused)
{
	/* Pull Low post complete pin */
	gpio_output(GPP_B20, 0);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, pull_post_complete_pin, NULL);
