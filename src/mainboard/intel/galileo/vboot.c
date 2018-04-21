/*
 * Copyright (C) 2016-2017 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <bootmode.h>
#include <compiler.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <lib.h>
#include <soc/i2c.h>
#include <soc/reg_access.h>
#include "reg_access.h"
#include "gen1.h"
#include "gen2.h"
#include <spi_flash.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/vbnv.h>

int clear_recovery_mode_switch(void)
{
	/* Nothing to do */
	return 0;
}

int get_recovery_mode_switch(void)
{
	return 0;
}

int get_write_protect_state(void)
{
	/* Not write protected */
	return 0;
}

void log_recovery_mode_switch(void)
{
}

void verstage_mainboard_init(void)
{
	const struct reg_script *script;

	/* Crypto Shield I2C Addresses:
	 *
	 * 0x29: AT97S3204T - TPM 1.2
	 * 0x50: ATAES132 - AES-128
	 * 0x60: ATECC108 - Elliptical Curve
	 * 0x64: ATSHA204 - SHA-256
	 * 0x68: DS3231M - RTC
	 */

	/* Determine the correct script for the board */
	if (IS_ENABLED(CONFIG_GALILEO_GEN2))
		script = gen2_i2c_init;
	else
		/* Determine which I2C address is in use */
		script = (reg_legacy_gpio_read (R_QNC_GPIO_RGLVL_RESUME_WELL)
			& GALILEO_DETERMINE_IOEXP_SLA_RESUMEWELL_GPIO)
			? gen1_i2c_0x20_init : gen1_i2c_0x21_init;

	/* Direct the I2C SDA and SCL signals to the Arduino connector */
	reg_script_run(script);
}

void __weak vboot_platform_prepare_reboot(void)
{
	const struct reg_script *script;

	/* Crypto Shield I2C Addresses:
	 *
	 * 0x29: AT97S3204T - TPM 1.2
	 * 0x50: ATAES132 - AES-128
	 * 0x60: ATECC108 - Elliptical Curve
	 * 0x64: ATSHA204 - SHA-256
	 * 0x68: DS3231M - RTC
	 */

	/* Determine the correct script for the board */
	if (IS_ENABLED(CONFIG_GALILEO_GEN2))
		script = gen2_tpm_reset;
	else
		/* Determine which I2C address is in use */
		script = (reg_legacy_gpio_read (R_QNC_GPIO_RGLVL_RESUME_WELL)
			& GALILEO_DETERMINE_IOEXP_SLA_RESUMEWELL_GPIO)
			? gen1_tpm_reset_0x20 : gen1_tpm_reset_0x21;

	/* Reset the TPM */
	reg_script_run(script);
}

int vbnv_cmos_failed(void)
{
	/* Indicate no failure until RTC failure bits are supported. */
	return 0;
}
