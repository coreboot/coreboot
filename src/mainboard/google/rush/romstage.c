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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <soc/romstage.h>

#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/padconfig.h>
#include <soc/nvidia/tegra/i2c.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static const struct pad_config padcfgs[] = {
	/* SOC_WARM_RESET_L */
	PAD_CFG_GPIO_OUT1(GPIO_PI5, PINMUX_PULL_UP),
	/* TPM I2C */
	PAD_CFG_SFIO(CAM_I2C_SCL, PINMUX_INPUT_ENABLE, I2C3),
	PAD_CFG_SFIO(CAM_I2C_SDA, PINMUX_INPUT_ENABLE, I2C3),
	/* EC on SPI1: mosi, miso, clk, cs */
	PAD_CFG_SFIO(ULPI_CLK, PINMUX_INPUT_ENABLE, SPI1),
	PAD_CFG_SFIO(ULPI_DIR, PINMUX_INPUT_ENABLE, SPI1),
	PAD_CFG_SFIO(ULPI_NXT, PINMUX_INPUT_ENABLE, SPI1),
	PAD_CFG_SFIO(ULPI_STP, PINMUX_INPUT_ENABLE, SPI1),
};

static void configure_clocks(void)
{
	/* EC on SPI1 controller. */
	clock_enable_clear_reset(0, CLK_H_SBC1, 0, 0, 0, 0);
	clock_configure_source(sbc1, CLK_M, 3000);

	/* TPM on I2C3 controller */
	clock_enable_clear_reset(0, 0, CLK_U_I2C3, 0, 0, 0);
	clock_configure_i2c_scl_freq(i2c3, PLLP, 400);
}

void romstage_mainboard_init(void)
{
	configure_clocks();

	/* Bring up controller interfaces for ramstage loading. */
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));

	/* TPM I2C bus */
	i2c_init(2);
}

void mainboard_configure_pmc(void)
{
}

void mainboard_enable_vdd_cpu(void)
{
	/* VDD_CPU is already enabled in bootblock. */
}
