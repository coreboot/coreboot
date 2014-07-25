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
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra132/pinmux.h>
#include <soc/nvidia/tegra132/gpio.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static void configure_tpm_i2c_bus(void)
{
	clock_configure_i2c_scl_freq(i2c3, PLLP, 19);

	i2c_init(2);
}

void mainboard_init_tpm_i2c(void)
{
	clock_enable_clear_reset(0, 0, CLK_U_I2C3, 0, 0, 0);

	gpio_output(GPIO(I5), 1);

	// I2C3 (cam) clock.
	pinmux_set_config(PINMUX_CAM_I2C_SCL_INDEX,
			  PINMUX_CAM_I2C_SCL_FUNC_I2C3 | PINMUX_INPUT_ENABLE);
	// I2C3 (cam) data.
	pinmux_set_config(PINMUX_CAM_I2C_SDA_INDEX,
			  PINMUX_CAM_I2C_SDA_FUNC_I2C3 | PINMUX_INPUT_ENABLE);


	configure_tpm_i2c_bus();
}

void mainboard_configure_pmc(void)
{
}

void mainboard_enable_vdd_cpu(void)
{
	/* VDD_CPU is already enabled in bootblock. */
}
