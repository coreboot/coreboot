/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <soc/addressmap.h>
#include <soc/funitcfg.h>
#include <soc/padconfig.h>
#include <soc/verstage.h>
#include <soc/nvidia/tegra/i2c.h>
#include "gpio.h"
#include "pmic.h"

static const struct pad_config tpm_pads[] = {
	PAD_CFG_SFIO(CAM_I2C_SCL, PINMUX_INPUT_ENABLE, I2C3),
	PAD_CFG_SFIO(CAM_I2C_SDA, PINMUX_INPUT_ENABLE, I2C3),
};

static const struct pad_config ec_i2c_pads[] = {
	PAD_CFG_SFIO(GEN2_I2C_SCL, PINMUX_OPEN_DRAIN|PINMUX_INPUT_ENABLE, I2C2),
	PAD_CFG_SFIO(GEN2_I2C_SDA, PINMUX_OPEN_DRAIN|PINMUX_INPUT_ENABLE, I2C2),
};

static const struct funit_cfg funits[] = {
	/* TPM on I2C3  @ 400kHz */
	FUNIT_CFG(I2C3, PLLP, 400, tpm_pads, ARRAY_SIZE(tpm_pads)),
	/* EC on I2C2 - pulled to 3.3V @ 100kHz */
	FUNIT_CFG(I2C2, PLLP, 100, ec_i2c_pads, ARRAY_SIZE(ec_i2c_pads)),
};

void verstage_mainboard_init(void)
{
	soc_configure_funits(funits, ARRAY_SIZE(funits));

	/* TPM */
	i2c_init(I2C3_BUS);
	/* EC */
	i2c_init(I2C2_BUS);
}
