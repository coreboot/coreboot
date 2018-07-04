/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016-2017 Intel Corp.
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
#include <soc/car.h>
#include <soc/ramstage.h>
#include "reg_access.h"
#include "gen1.h"
#include "gen2.h"

void car_mainboard_pre_console_init(void)
{
	const struct reg_script *script;

	/* Initialize the GPIO controllers */
	if (IS_ENABLED(CONFIG_GALILEO_GEN2))
		script = gen2_gpio_init;
	else
		script = gen1_gpio_init;
	reg_script_run(script);

	/* Initialize the RXD and TXD paths for UART0 */
	if (IS_ENABLED(CONFIG_ENABLE_BUILTIN_HSUART0)) {
		if (IS_ENABLED(CONFIG_GALILEO_GEN2))
			script = gen2_hsuart0;
		else
			script = (reg_legacy_gpio_read(
				R_QNC_GPIO_RGLVL_RESUME_WELL)
				& GALILEO_DETERMINE_IOEXP_SLA_RESUMEWELL_GPIO)
				? gen1_hsuart0_0x20 : gen1_hsuart0_0x21;
		reg_script_run(script);
	}
}

void mainboard_gpio_i2c_init(struct device *dev)
{
	const struct reg_script *script;

	printk(BIOS_INFO, "Galileo I2C chip initialization\n");

	/* Determine the correct script for the board */
	if (IS_ENABLED(CONFIG_GALILEO_GEN2))
		script = gen2_i2c_init;
	else
		/* Determine which I2C address is in use */
		script = (reg_legacy_gpio_read(R_QNC_GPIO_RGLVL_RESUME_WELL)
			& GALILEO_DETERMINE_IOEXP_SLA_RESUMEWELL_GPIO)
			? gen1_i2c_0x20_init : gen1_i2c_0x21_init;

	/* Initialize the I2C chips */
	reg_script_run(script);
}

void mainboard_gpio_pcie_reset(uint32_t pin_value)
{
	uint32_t pin_number;
	uint32_t value;

	/* Determine the correct PCIe reset pin */
	if (IS_ENABLED(CONFIG_GALILEO_GEN2))
		pin_number = GEN2_PCI_RESET_RESUMEWELL_GPIO;
	else
		pin_number = GEN1_PCI_RESET_RESUMEWELL_GPIO;

	/* Update the PCIe reset value */
	value = reg_legacy_gpio_read(R_QNC_GPIO_RGLVL_RESUME_WELL);
	value = (value & ~(1 << pin_number)) | ((pin_value & 1) << pin_number);
	reg_legacy_gpio_write(R_QNC_GPIO_RGLVL_RESUME_WELL, value);
}
