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

#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/padconfig.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/romstage.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static const struct pad_config padcfgs[] = {
	/* AP_SYS_RESET_L */
	PAD_CFG_GPIO_OUT1(GPIO_PI5, PINMUX_PULL_UP),
	/* TPM on I2C3 */
	PAD_CFG_SFIO(CAM_I2C_SCL, PINMUX_INPUT_ENABLE, I2C3),
	PAD_CFG_SFIO(CAM_I2C_SDA, PINMUX_INPUT_ENABLE, I2C3),
	/* EC on I2C2 */
	PAD_CFG_SFIO(GEN2_I2C_SCL, PINMUX_INPUT_ENABLE, I2C2),
	PAD_CFG_SFIO(GEN2_I2C_SDA, PINMUX_INPUT_ENABLE, I2C2),
	/* WP_L */
	PAD_CFG_GPIO_INPUT(KB_ROW1, PINMUX_PULL_NONE),
};

static void configure_clocks(void)
{
	/* TPM on I2C3 */
	clock_enable_clear_reset(0, 0, CLK_U_I2C3, 0, 0, 0);
	clock_configure_i2c_scl_freq(i2c3, PLLP, 400);

	/* EC on I2C2 */
	clock_enable_clear_reset(0, CLK_H_I2C2, 0, 0, 0, 0);
	clock_configure_i2c_scl_freq(i2c2, PLLP, 100);
}

void romstage_mainboard_init(void)
{
	configure_clocks();

	/* Bring up controller interfaces for ramstage loading. */
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));

	/* TPM */
	i2c_init(2);
	/* EC */
	i2c_init(1);
}

void mainboard_configure_pmc(void)
{
}

void mainboard_enable_vdd_cpu(void)
{
	/* VDD_CPU is already enabled in bootblock. */
}
