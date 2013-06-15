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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/device.h>
#include <device/pnp.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <arch/io.h>
#include <stdlib.h>
#include "chip.h"
#include "it8772f.h"

static void pnp_enter_ext_func_mode(device_t dev)
{
	u16 port = dev->path.pnp.port;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	pnp_write_config(dev, 0x02, 0x02);
}

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
 * Setup a FAN PWM interface for software control
 */
static void it8772f_enable_fan(struct resource *res, int fan)
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
		/* Set a default medium fan speed */
		it8772f_envc_write(res, IT8772F_FAN_CTL2_PWM_START, 0x80);
		break;
	case 3:
		/* Enable software operation */
		it8772f_envc_write(res, IT8772F_FAN_CTL3_PWM_MODE,
				   IT8772F_FAN_CTL_PWM_MODE_SOFTWARE);
		/* Disable Smoothing */
		it8772f_envc_write(res, IT8772F_FAN_CTL3_AUTO_MODE,
				   IT8772F_FAN_CTL_AUTO_SMOOTHING_DIS);
		/* Set a default medium fan speed */
		it8772f_envc_write(res, IT8772F_FAN_CTL3_PWM_START, 0x80);
		break;
	}
}

static void it8772f_init(device_t dev)
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

		/* Enable FANx if configured */
		if (conf->fan1_enable)
			it8772f_enable_fan(res, 1);
		if (conf->fan2_enable)
			it8772f_enable_fan(res, 2);
		if (conf->fan3_enable)
			it8772f_enable_fan(res, 3);
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
			pc_keyboard_init(&conf->keyboard);
		}
		break;
	case IT8772F_KBCM:
		break;
	case IT8772F_IR:
		break;
	}
}

static const struct pnp_mode_ops pnp_conf_mode_ops = {
	.enter_conf_mode  = pnp_enter_ext_func_mode,
	.exit_conf_mode   = pnp_exit_ext_func_mode,
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8772f_init,
	.ops_pnp_mode     = &pnp_conf_mode_ops,
};

static struct pnp_info pnp_dev_info[] = {
	/* Floppy Disk Controller */
	{ &ops, IT8772F_FDC, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	/* Serial Port 1 */
	{ &ops, IT8772F_SP1, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	/* Environmental Controller */
	{ &ops, IT8772F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
	  {0x0ff8, 0}, {0x0ffc, 4}, },
	/* KBC Keyboard */
	{ &ops, IT8772F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
	  {0x0fff, 0}, {0x0fff, 4}, },
	/* KBC Mouse */
	{ &ops, IT8772F_KBCM, PNP_IRQ0, },
	/* 27 GPIOs */
	{ &ops, IT8772F_GPIO, PNP_IO0 | PNP_IO1,
	  {0x0fff, 0}, {0x0ff8, 0}, },
	/* Infrared */
	{ &ops, IT8772F_IR, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &pnp_ops,
		ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8772f_ops = {
	CHIP_NAME("ITE IT8772F Super I/O")
	.enable_dev = enable_dev,
};
