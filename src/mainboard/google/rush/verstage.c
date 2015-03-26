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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <soc/addressmap.h>
#include <soc/funitcfg.h>
#include <soc/padconfig.h>
#include <soc/verstage.h>
#include <soc/nvidia/tegra/i2c.h>

static const struct pad_config i2cpad[] = {
	/* TPM I2C */
	PAD_CFG_SFIO(CAM_I2C_SCL, PINMUX_INPUT_ENABLE, I2C3),
	PAD_CFG_SFIO(CAM_I2C_SDA, PINMUX_INPUT_ENABLE, I2C3),
};

static const struct pad_config spipad[] = {
	/* EC on SPI1: mosi, miso, clk, cs */
	PAD_CFG_SFIO(ULPI_CLK, PINMUX_INPUT_ENABLE, SPI1),
	PAD_CFG_SFIO(ULPI_DIR, PINMUX_INPUT_ENABLE, SPI1),
	PAD_CFG_SFIO(ULPI_NXT, PINMUX_INPUT_ENABLE, SPI1),
	PAD_CFG_SFIO(ULPI_STP, PINMUX_INPUT_ENABLE, SPI1),
};

static const struct funit_cfg funitcfgs[] = {
	FUNIT_CFG(SBC1, CLK_M, 3000, spipad, ARRAY_SIZE(spipad)),
	FUNIT_CFG(I2C3, PLLP, 400, i2cpad, ARRAY_SIZE(i2cpad)),
};

void verstage_mainboard_init(void)
{
	soc_configure_funits(funitcfgs, ARRAY_SIZE(funitcfgs));

	/* TPM I2C bus */
	i2c_init(2);
}
