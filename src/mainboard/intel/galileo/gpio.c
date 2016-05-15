/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
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

#include <arch/io.h>
#include <console/console.h>
#include <soc/ramstage.h>
#include "reg_access.h"
#include "gen1.h"
#include "gen2.h"

void mainboard_gpio_i2c_init(device_t dev)
{
	const struct reg_script *script;

	printk(BIOS_INFO, "Galileo I2C chip initialization\n");

	/* Determine the correct script for the board */
	if (IS_ENABLED(CONFIG_GALILEO_GEN2))
		script = gen2_i2c_init;
	else
		/* Determine which I2C address is in use */
		script = (reg_legacy_gpio_read (R_QNC_GPIO_RGLVL_RESUME_WELL)
			& GALILEO_DETERMINE_IOEXP_SLA_RESUMEWELL_GPIO)
			? gen1_i2c_0x20_init : gen1_i2c_0x21_init;

	/* Initialize the I2C chips */
	reg_script_run(script);
}

void mainboard_gpio_init(void)
{
	const struct reg_script *script;

	/* Initialize the GPIO controllers */
	if (IS_ENABLED(CONFIG_GALILEO_GEN2))
		script = gen2_gpio_init;
	else
		script = gen1_gpio_init;
	reg_script_run(script);
}
