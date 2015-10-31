/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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

#include <console/console.h>
#include <stdint.h>
#include <string.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/romstage.h>
#include "gpio.h"

void mainboard_romstage_entry(struct romstage_params *rp)
{
	struct pei_data pei_data;

	post_code(0x32);

	/* Initialize GPIOs */
	init_gpios(mainboard_gpio_config);

	/* Fill out PEI DATA */
	memset(&pei_data, 0, sizeof(pei_data));
	mainboard_fill_pei_data(&pei_data);
	rp->pei_data = &pei_data;

	romstage_common(rp);
}
