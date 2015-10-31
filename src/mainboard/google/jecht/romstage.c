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
 */

#include <cbfs.h>
#include <console/console.h>
#include <string.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/romstage.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>
#include <mainboard/google/jecht/spd/spd.h>
#include "gpio.h"

#define DUMMY_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, IT8772F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8772F_GPIO)

void mainboard_romstage_entry(struct romstage_params *rp)
{
	struct pei_data pei_data;

	post_code(0x32);

	/* Initialize GPIOs */
	init_gpios(mainboard_gpio_config);

	/* Fill out PEI DATA */
	memset(&pei_data, 0, sizeof(pei_data));
	mainboard_fill_pei_data(&pei_data);
	mainboard_fill_spd_data(&pei_data);
	rp->pei_data = &pei_data;

	/* Call into the real romstage main with this board's attributes. */
	romstage_common(rp);
}

void mainboard_pre_console_init(void)
{
	/* Early SuperIO setup */
	it8772f_ac_resume_southbridge(DUMMY_DEV);
	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
