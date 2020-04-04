/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/i2c_simple.h>
#include <gpio.h>
#include <soc/pinmux.h>

#include "i2c.h"

static struct {
	int pinmux_func;
	gpio_t sda;
	gpio_t scl;
} pins[] = {
	[0]{.pinmux_func = PINMUX_GEN1_I2C_SCL_FUNC_I2C1,
	    .sda = GPIO(C5), .scl = GPIO(C4)},
	[1]{.pinmux_func = PINMUX_GEN2_I2C_SCL_FUNC_I2C2,
	    .sda = GPIO(T6), .scl = GPIO(T5)},
	[2]{.pinmux_func = PINMUX_CAM_I2C_SCL_FUNC_I2C3,
	    .sda = GPIO(BB2), .scl = GPIO(BB1)},
	[3]{.pinmux_func = PINMUX_DDC_SCL_FUNC_I2C4,
	    .sda = GPIO(V5), .scl = GPIO(V4)},
	[4]{.pinmux_func = PINMUX_PWR_I2C_SCL_FUNC_I2CPMU,
	    .sda = GPIO(Z7), .scl = GPIO(Z6)},
};

static void tegra_set_sda(unsigned int bus, int high)
{
	if (high)
		gpio_input_pullup(pins[bus].sda);
	else
		gpio_output(pins[bus].sda, 0);
}

static void tegra_set_scl(unsigned int bus, int high)
{
	if (high)
		gpio_input_pullup(pins[bus].scl);
	else
		gpio_output(pins[bus].scl, 0);
}

static int tegra_get_sda(unsigned int bus)
{
	return gpio_get(pins[bus].sda);
}

static int tegra_get_scl(unsigned int bus)
{
	return gpio_get(pins[bus].scl);
}

static struct software_i2c_ops tegra_ops = {
	.set_sda = tegra_set_sda,
	.set_scl = tegra_set_scl,
	.get_sda = tegra_get_sda,
	.get_scl = tegra_get_scl,
};

void tegra_software_i2c_init(unsigned int bus)
{
	software_i2c[bus] = &tegra_ops;

	/* Initialize bus to idle state. */
	tegra_set_sda(bus, 1);
	tegra_set_scl(bus, 1);
}

void tegra_software_i2c_disable(unsigned int bus)
{
	software_i2c[bus] = NULL;

	/* Return pins to I2C controller. */
	pinmux_set_config(pins[bus].sda >> GPIO_PINMUX_SHIFT,
			  pins[bus].pinmux_func | PINMUX_INPUT_ENABLE);
	pinmux_set_config(pins[bus].scl >> GPIO_PINMUX_SHIFT,
			  pins[bus].pinmux_func | PINMUX_INPUT_ENABLE);
	gpio_set_mode(pins[bus].sda, GPIO_MODE_SPIO);
	gpio_set_mode(pins[bus].scl, GPIO_MODE_SPIO);
}
