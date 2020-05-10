/* SPDX-License-Identifier: GPL-2.0-or-later */
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
