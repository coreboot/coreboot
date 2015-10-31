/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google, Inc.
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
#include <device/i2c.h>
#include <gpio.h>
#include <soc/grf.h>
#include <soc/i2c.h>
#include <soc/pmu.h>

static struct {
	gpio_t scl;
	gpio_t sda;
} pins[] = {
	[0]{.scl = GPIO(0, C, 0), .sda = GPIO(0, B, 7)},
	[1]{.scl = GPIO(8, A, 5), .sda = GPIO(8, A, 4)},
	[2]{.scl = GPIO(6, B, 2), .sda = GPIO(6, B, 1)},
	[3]{.scl = GPIO(2, C, 0), .sda = GPIO(2, C, 1)},
	[4]{.scl = GPIO(7, C, 2), .sda = GPIO(7, C, 1)},
	[5]{.scl = GPIO(7, C, 4), .sda = GPIO(7, C, 3)},
};

static int get_scl(unsigned bus)
{
	return gpio_get(pins[bus].scl);
}

static int get_sda(unsigned bus)
{
	return gpio_get(pins[bus].sda);
}

static void set_scl(unsigned bus, int high)
{
	if (high)
		gpio_input_pullup(pins[bus].scl);
	else
		gpio_output(pins[bus].scl, 0);
}

static void set_sda(unsigned bus, int high)
{
	if (high)
		gpio_input_pullup(pins[bus].sda);
	else
		gpio_output(pins[bus].sda, 0);
}

static struct software_i2c_ops rk_ops = {
	.get_scl = get_scl,
	.get_sda = get_sda,
	.set_scl = set_scl,
	.set_sda = set_sda,
};

void software_i2c_attach(unsigned bus)
{
	software_i2c[bus] = &rk_ops;

	/* Mux pins to GPIO function for software I2C emulation. */
	switch (bus) {
	case 0:
		clrbits_le32(&rk3288_pmu->iomux_i2c0scl, IOMUX_I2C0SCL);
		clrbits_le32(&rk3288_pmu->iomux_i2c0sda, IOMUX_I2C0SDA);
		break;
	case 1:
		write32(&rk3288_grf->iomux_i2c1, IOMUX_GPIO(IOMUX_I2C1));
		break;
	case 2:
		write32(&rk3288_grf->iomux_i2c2, IOMUX_GPIO(IOMUX_I2C2));
		break;
	case 3:
		write32(&rk3288_grf->iomux_i2c3, IOMUX_GPIO(IOMUX_I2C3));
		break;
	case 4:
		write32(&rk3288_grf->iomux_i2c4, IOMUX_GPIO(IOMUX_I2C4));
		break;
	case 5:
		write32(&rk3288_grf->iomux_i2c5scl, IOMUX_GPIO(IOMUX_I2C5SCL));
		write32(&rk3288_grf->iomux_i2c5sda, IOMUX_GPIO(IOMUX_I2C5SDA));
		break;
	default:
		die("Unknown I2C bus number!");
	}

	/* Initialize bus to idle state. */
	set_scl(bus, 1);
	set_sda(bus, 1);
}

void software_i2c_detach(unsigned bus)
{
	software_i2c[bus] = NULL;

	/* Mux pins back to hardware I2C controller. */
	switch (bus) {
	case 0:
		setbits_le32(&rk3288_pmu->iomux_i2c0scl, IOMUX_I2C0SCL);
		setbits_le32(&rk3288_pmu->iomux_i2c0sda, IOMUX_I2C0SDA);
		break;
	case 1:
		write32(&rk3288_grf->iomux_i2c1, IOMUX_I2C1);
		break;
	case 2:
		write32(&rk3288_grf->iomux_i2c2, IOMUX_I2C2);
		break;
	case 3:
		write32(&rk3288_grf->iomux_i2c3, IOMUX_I2C3);
		break;
	case 4:
		write32(&rk3288_grf->iomux_i2c4, IOMUX_I2C4);
		break;
	case 5:
		write32(&rk3288_grf->iomux_i2c5scl, IOMUX_I2C5SCL);
		write32(&rk3288_grf->iomux_i2c5sda, IOMUX_I2C5SDA);
		break;
	default:
		die("Unknown I2C bus number!");
	}
}
