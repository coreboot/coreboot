/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <cbfs.h>
#include <console/console.h>
#include <string.h>
#include <ec/google/chromeec/ec.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <mainboard/google/samus/spd/spd.h>
#include <mainboard/google/samus/gpio.h>
#include <ec/google/chromeec/ec.h>
#include "board_version.h"

void mainboard_romstage_entry(struct romstage_params *rp)
{
	struct pei_data pei_data;

	post_code(0x31);

	if (rp->power_state->prev_sleep_state != SLEEP_STATE_S3)
		google_chromeec_kbbacklight(100);

	printk(BIOS_INFO, "MLB: board version %s\n", samus_board_version());

	/* Ensure the EC and PD are in the right mode for recovery */
	google_chromeec_early_init();

	/* Initialize GPIOs */
	init_gpios(mainboard_gpio_config);

	/* Fill out PEI DATA */
	memset(&pei_data, 0, sizeof(pei_data));
	mainboard_fill_pei_data(&pei_data);
	mainboard_fill_spd_data(&pei_data);
	rp->pei_data = &pei_data;

	/* Initalize memory */
	romstage_common(rp);

	/* Bring SSD out of reset */
	set_gpio(SAMUS_GPIO_SSD_RESET_L, GPIO_OUT_HIGH);

	/*
	 * Enable PP3300_AUTOBAHN_EN after initial GPIO setup
	 * to prevent possible brownout.
	 */
	set_gpio(SAMUS_GPIO_PP3300_AUTOBAHN_EN, GPIO_OUT_HIGH);
}
