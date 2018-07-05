/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Felix Held <felix-coreboot@felixheld.de>
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * Copyright (C) 2018 Patrick Rudolph <siro@das-labor.org>
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

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include <superio/conf_mode.h>

#include "npcd378.h"

uint8_t npcd378_hwm_read(const uint16_t iobase, const uint16_t reg)
{
	outb((reg >> 8) & 0xf, iobase + 0xff);
	uint8_t reg8 = inb(iobase + (reg & 0xff));
	if (reg8 == 0xff)
		reg8 = inb(iobase + (reg & 0xff));

	outb(0, iobase + 0xff);
	return reg8;
}

void npcd378_hwm_write(const uint16_t iobase, const uint16_t reg,
		       const uint8_t val)
{
	outb((reg >> 8) & 0xf, iobase + 0xff);
	outb(val, iobase + (reg & 0xff));

	outb(0, iobase + 0xff);
}

void npcd378_hwm_write_start(const uint16_t iobase)
{
	u8 reg8 = npcd378_hwm_read(iobase, NPCD837_HWM_WRITE_LOCK_CTRL);
	reg8 &= ~NPCD837_HWM_WRITE_LOCK_BIT;
	npcd378_hwm_write(iobase, NPCD837_HWM_WRITE_LOCK_CTRL, reg8);
}

void npcd378_hwm_write_finished(const uint16_t iobase)
{
	u8 reg8 = npcd378_hwm_read(iobase, NPCD837_HWM_WRITE_LOCK_CTRL);
	reg8 |= NPCD837_HWM_WRITE_LOCK_BIT;
	npcd378_hwm_write(iobase, NPCD837_HWM_WRITE_LOCK_CTRL, reg8);
}

static void npcd378_init(struct device *dev)
{
	struct resource *res;
	uint8_t pwm, fan_lvl;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for FDC etc. */
	case NPCD378_KBC:
		pc_keyboard_init(PROBE_AUX_DEVICE);
		break;
	case NPCD378_HWM:
		res = find_resource(dev, PNP_IDX_IO0);
		if (!res || !res->base) {
			printk(BIOS_ERR, "NPCD378: LDN%u IOBASE not set.\n",
			       NPCD378_HWM);
			break;
		}

		npcd378_hwm_write_start(res->base);

		if (!get_option(&fan_lvl, "psu_fan_lvl") || fan_lvl > 7)
			fan_lvl = 3;

		pwm = NPCD378_HWM_PSU_FAN_MIN +
		    (NPCD378_HWM_PSU_FAN_MAX - NPCD378_HWM_PSU_FAN_MIN) *
		    fan_lvl / 7;

		/* Set PSU fan PWM lvl */
		npcd378_hwm_write(res->base, NPCD378_HWM_PSU_FAN_PWM_CTRL, pwm);
		printk(BIOS_INFO, "NPCD378: PSU fan PWM 0x%02x\n", pwm);

		npcd378_hwm_write_finished(res->base);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = npcd378_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, NPCD378_FDC, PNP_IO0|PNP_IRQ0|PNP_DRQ0, 0x0ff8, },
	{ &ops, NPCD378_PP, PNP_IO0|PNP_IRQ0|PNP_DRQ0, 0x0ff8, },
	{ &ops, NPCD378_SP1, PNP_IO0|PNP_IRQ0, 0x0ff8, },
	{ &ops, NPCD378_SP2, PNP_IO0|PNP_IRQ0, 0x0ff8, },
	{ &ops, NPCD378_PWR, PNP_IO0|PNP_IO1|PNP_IRQ0|PNP_MSC0|
		PNP_MSC1|PNP_MSC2|PNP_MSC3|PNP_MSC4|PNP_MSC5|PNP_MSC6|PNP_MSC7|
		PNP_MSC8|PNP_MSC9|PNP_MSCA|PNP_MSCB|PNP_MSCC|PNP_MSCD|PNP_MSCE,
		0x0ff8, 0x0ff8},
	{ &ops, NPCD378_AUX, PNP_IRQ0, 0x0fff, 0x0fff, },
	{ &ops, NPCD378_KBC, PNP_IO0|PNP_IO1|PNP_IRQ0,
		0x0fff, 0x0fff, },
	{ &ops, NPCD378_WDT1, PNP_IO0|PNP_MSC8|PNP_MSC9|
		PNP_MSCA|PNP_MSCB|PNP_MSCC|PNP_MSCD|PNP_MSCE, 0x0ff8},
	{ &ops, NPCD378_HWM, PNP_IO0|PNP_MSC0|PNP_MSC1|PNP_MSC2|PNP_MSC3|
		PNP_MSC4|PNP_MSC5|PNP_MSC6|PNP_MSC7|PNP_IRQ0, 0x0ff8},
	{ &ops, NPCD378_GPIO_PP_OD, PNP_MSC0|PNP_MSC1|PNP_MSC2|PNP_MSC3|
		PNP_MSC4|PNP_MSC5|PNP_MSC6|PNP_MSC7|PNP_MSC8|PNP_MSC9|PNP_MSCA|
		PNP_MSCB|PNP_MSCC|PNP_MSCD|PNP_MSCE},
	{ &ops, NPCD378_I2C, PNP_IO0|PNP_IO1|PNP_IRQ0|PNP_MSC0|
		PNP_MSC1|PNP_MSC2|PNP_MSC3|PNP_MSC4|PNP_MSC5|PNP_MSC6|PNP_MSC7|
		PNP_MSC8|PNP_MSC9|PNP_MSCA|PNP_MSCB|PNP_MSCC|PNP_MSCD|PNP_MSCE,
		0x0ff8, 0x0ff8},
	{ &ops, NPCD378_SUSPEND, PNP_IO0, 0x0ff8 },
	{ &ops, NPCD378_GPIOA, PNP_IO0|PNP_MSC0|PNP_MSC1|PNP_MSC2|PNP_MSC3|
		PNP_MSC4, 0x0ff8},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_npcd378_ops = {
	CHIP_NAME("NUVOTON NPCD378 Super I/O")
	.enable_dev = enable_dev,
};
