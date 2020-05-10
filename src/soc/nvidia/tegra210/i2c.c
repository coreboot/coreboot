/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/i2c.h>

struct tegra_i2c_bus_info tegra_i2c_info[] = {
	{
		.base = (void *)TEGRA_I2C1_BASE,
		.reset_bit = CLK_L_I2C1,
		.reset_func = &clock_reset_l
	},
	{
		.base = (void *)TEGRA_I2C2_BASE,
		.reset_bit = CLK_H_I2C2,
		.reset_func = &clock_reset_h
	},
	{
		.base = (void *)TEGRA_I2C3_BASE,
		.reset_bit = CLK_U_I2C3,
		.reset_func = &clock_reset_u
	},
	{
		.base = (void *)TEGRA_I2C4_BASE,
		.reset_bit = CLK_V_I2C4,
		.reset_func = &clock_reset_v
	},
	{
		.base = (void *)TEGRA_I2C5_BASE,
		.reset_bit = CLK_H_I2C5,
		.reset_func = &clock_reset_h
	},
	{
		.base = (void *)TEGRA_I2C6_BASE,
		.reset_bit = CLK_X_I2C6,
		.reset_func = &clock_reset_x
	}
};

const unsigned int num_i2c_buses = ARRAY_SIZE(tegra_i2c_info);
