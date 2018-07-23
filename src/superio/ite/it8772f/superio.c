/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <pc80/keyboard.h>
#include <arch/io.h>
#include <delay.h>
#include <stdlib.h>
#include <superio/conf_mode.h>

#include "chip.h" /* FIXME */
#include "it8772f.h"

static inline u8 it8772f_envc_read(struct resource *res, u8 addr)
{
	outb(addr, res->base + 5);
	return inb(res->base + 6);
}

static inline void it8772f_envc_write(struct resource *res, u8 addr, u8 value)
{
	outb(addr, res->base + 5);
	outb(value, res->base + 6);
}

static void it8772f_extemp_force_idle_status(struct resource *res)
{
	u8 reg;
	int retries = 10;

	/* Wait up to 10ms for non-busy state. */
	while (retries > 0) {
		reg = it8772f_envc_read(res, IT8772F_EXTEMP_STATUS);

		if ((reg & IT8772F_EXTEMP_STATUS_HOST_BUSY) == 0x0)
			break;

		retries--;

		mdelay(1);
	}

	if (retries == 0 && (reg & IT8772F_EXTEMP_STATUS_HOST_BUSY) == 0x1) {
		/*
		 * SIO is busy due to unfinished peci transaction.
		 * Re-configure Register 0x8E to terminate processes.
		 */
		it8772f_envc_write(res, IT8772F_EXTEMP_CONTROL,
			IT8772F_EXTEMP_CONTROL_AUTO_4HZ |
			IT8772F_EXTEMP_CONTROL_AUTO_START);
	}
}

/*
 * Setup External Temperature to read via PECI into TMPINx register
 */
static void it8772f_enable_peci(struct resource *res, int tmpin)
{
	if (tmpin < 1 || tmpin > 3)
		return;

	/* Enable PECI interface */
	it8772f_envc_write(res, IT8772F_INTERFACE_SELECT,
			   IT8772F_INTERFACE_SEL_PECI |
			   IT8772F_INTERFACE_SPEED_TOLERANCE);

	/* Setup External Temperature using PECI GetTemp */
	it8772f_envc_write(res, IT8772F_EXTEMP_ADDRESS,
			   PECI_CLIENT_ADDRESS);
	it8772f_envc_write(res, IT8772F_EXTEMP_COMMAND,
			   PECI_GETTEMP_COMMAND);
	it8772f_envc_write(res, IT8772F_EXTEMP_WRITE_LENGTH,
			   PECI_GETTEMP_WRITE_LENGTH);
	it8772f_envc_write(res, IT8772F_EXTEMP_READ_LENGTH,
			   PECI_GETTEMP_READ_LENGTH);
	it8772f_envc_write(res, IT8772F_EXTEMP_CONTROL,
			   IT8772F_EXTEMP_CONTROL_AUTO_4HZ |
			   IT8772F_EXTEMP_CONTROL_AUTO_START);

	/* External Temperature reported in TMPINx register */
	it8772f_envc_write(res, IT8772F_ADC_TEMP_CHANNEL_ENABLE,
			   (tmpin & 3) << 6);
}

/*
 * Set up External Temperature to read via thermal diode/resistor
 * into TMPINx register
 */
static void it8772f_enable_tmpin(struct resource *res, int tmpin,
				enum thermal_mode mode)
{
	u8 reg;

	if (tmpin != 1 && tmpin != 2)
		return;

	reg = it8772f_envc_read(res, IT8772F_ADC_TEMP_CHANNEL_ENABLE);

	switch (mode) {
	case THERMAL_DIODE:
		/* Thermal Diode Mode */
		it8772f_envc_write(res, IT8772F_ADC_TEMP_CHANNEL_ENABLE,
				   reg | tmpin);
		break;
	case THERMAL_RESISTOR:
		/* Thermal Resistor Mode */
		it8772f_envc_write(res, IT8772F_ADC_TEMP_CHANNEL_ENABLE,
				   reg | (tmpin << 3));
		break;
	default:
		printk(BIOS_ERR, "Unsupported thermal mode 0x%x on TMPIN%d\n",
			mode, tmpin);
		return;
	}

	reg = it8772f_envc_read(res, IT8772F_CONFIGURATION);

	/* Enable the startup of monitoring operation */
	it8772f_envc_write(res, IT8772F_CONFIGURATION, reg | 0x01);
}

/*
 * Setup a FAN PWM interface for software control
 */
static void it8772f_enable_fan(struct resource *res, int fan, u8 fan_speed)
{
	u8 reg;

	if (fan < 1 || fan > 3)
		return;

	/* Enable 6MHz (23.43kHz PWM) active high output */
	reg = it8772f_envc_read(res, IT8772F_FAN_CTL_MODE);
	reg |= IT8772F_FAN_CTL_ON(fan) |
		IT8772F_FAN_PWM_CLOCK_6MHZ |
		IT8772F_FAN_CTL_POLARITY_HIGH;
	it8772f_envc_write(res, IT8772F_FAN_CTL_MODE, reg);

	/* Enable output in smart mode */
	reg = it8772f_envc_read(res, IT8772F_FAN_MAIN_CTL);
	reg |= IT8772F_FAN_MAIN_CTL_TAC_SMART(fan);
	reg |= IT8772F_FAN_MAIN_CTL_TAC_EN(fan);
	it8772f_envc_write(res, IT8772F_FAN_MAIN_CTL, reg);

	switch (fan) {
	case 2:
		/* Enable software operation */
		it8772f_envc_write(res, IT8772F_FAN_CTL2_PWM_MODE,
				   IT8772F_FAN_CTL_PWM_MODE_SOFTWARE);
		/* Disable Smoothing */
		it8772f_envc_write(res, IT8772F_FAN_CTL2_AUTO_MODE,
				   IT8772F_FAN_CTL_AUTO_SMOOTHING_DIS);
		/* Set a default fan speed */
		if (fan_speed)
			it8772f_envc_write(res, IT8772F_FAN_CTL2_PWM_START,
				   fan_speed);
		break;
	case 3:
		/* Enable software operation */
		it8772f_envc_write(res, IT8772F_FAN_CTL3_PWM_MODE,
				   IT8772F_FAN_CTL_PWM_MODE_SOFTWARE);
		/* Disable Smoothing */
		it8772f_envc_write(res, IT8772F_FAN_CTL3_AUTO_MODE,
				   IT8772F_FAN_CTL_AUTO_SMOOTHING_DIS);
		/* Set a default fan speed */
		if (fan_speed)
			it8772f_envc_write(res, IT8772F_FAN_CTL3_PWM_START,
				   fan_speed);
		break;
	}
}

static void it8772f_init(struct device *dev)
{
	struct superio_ite_it8772f_config *conf = dev->chip_info;
	struct resource *res;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8772F_EC:
		res = find_resource(dev, PNP_IDX_IO0);
		if (!res)
			break;

		/* Enable PECI if configured */
		it8772f_enable_peci(res, conf->peci_tmpin);

		/* Enable HWM if configured */
		if (conf->tmpin1_mode != THERMAL_MODE_DISABLED)
			it8772f_enable_tmpin(res, 1, conf->tmpin1_mode);
		if (conf->tmpin2_mode != THERMAL_MODE_DISABLED)
			it8772f_enable_tmpin(res, 2, conf->tmpin2_mode);

		/* Enable FANx if configured */
		if (conf->fan1_enable)
			it8772f_enable_fan(res, 1, 0);
		if (conf->fan2_enable)
			it8772f_enable_fan(res, 2,
				conf->fan2_speed ? conf->fan2_speed : 0x80);
		if (conf->fan3_enable)
			it8772f_enable_fan(res, 3,
				conf->fan3_speed ? conf->fan3_speed : 0x80);

		/*
		 * System may get wrong temperature data when SIO is in
		 * busy state. Therefore, check the status and terminate
		 * processes if needed.
		 */
		it8772f_extemp_force_idle_status(res);
		break;
	case IT8772F_GPIO:
		/* Set GPIO output levels */
		res = find_resource(dev, PNP_IDX_IO1);
		if (res) {
			if (conf->gpio_set1)
				outb(conf->gpio_set1, res->base + 0);
			if (conf->gpio_set2)
				outb(conf->gpio_set2, res->base + 1);
			if (conf->gpio_set3)
				outb(conf->gpio_set3, res->base + 2);
			if (conf->gpio_set4)
				outb(conf->gpio_set4, res->base + 3);
			if (conf->gpio_set5)
				outb(conf->gpio_set5, res->base + 4);
			if (conf->gpio_set6)
				outb(conf->gpio_set6, res->base + 5);
		}
		break;
	case IT8772F_KBCK:
		if (!conf->skip_keyboard) {
			set_kbc_ps2_mode();
			pc_keyboard_init(NO_AUX_DEVICE);
		}
		break;
	case IT8772F_KBCM:
		break;
	case IT8772F_IR:
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8772f_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* Floppy Disk Controller */
	{ NULL, IT8772F_FDC, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	/* Serial Port 1 */
	{ NULL, IT8772F_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	/* Environmental Controller */
	{ NULL, IT8772F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 |
		PNP_MSC4 | PNP_MSCA,
	  0x0ff8, 0x0ffc, },
	/* KBC Keyboard */
	{ NULL, IT8772F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
	  0x0fff, 0x0fff, },
	/* KBC Mouse */
	{ NULL, IT8772F_KBCM, PNP_IRQ0, },
	/* 27 GPIOs */
	{ NULL, IT8772F_GPIO, PNP_IO0 | PNP_IO1,
	  0x0fff, 0x0ff8, },
	/* Infrared */
	{ NULL, IT8772F_IR, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8772f_ops = {
	CHIP_NAME("ITE IT8772F Super I/O")
	.enable_dev = enable_dev,
};
