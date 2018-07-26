/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 secunet Security Networks AG
 * Copyright (C) 2017 Samuel Holland <samuel@sholland.org>
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
#include <pc80/keyboard.h>
#include <pc80/mc146818rtc.h>
#include <superio/ite/common/env_ctrl.h>
#include <superio/conf_mode.h>
#include <types.h>

#include "chip.h"
#include "it8720f.h"

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

static void power_control_init(struct device *dev)
{
	int power_on = MAINBOARD_POWER_OFF;
	u8 value;

	get_option(&power_on, "power_on_after_fail");

	pnp_enter_conf_mode(dev);
	pnp_set_logical_device(dev);

	value = pnp_read_config(dev, IT8720F_EC_PCR1);
	if (power_on == MAINBOARD_POWER_KEEP)
		value |= (1 << 5);
	else
		value &= ~(1 << 5);
	pnp_write_config(dev, IT8720F_EC_PCR1, value);

	value = pnp_read_config(dev, IT8720F_EC_PCR2);
	if (power_on == MAINBOARD_POWER_ON)
		value |= (1 << 5);
	else
		value &= ~(1 << 5);
	pnp_write_config(dev, IT8720F_EC_PCR2, value);

	pnp_exit_conf_mode(dev);
}

static void it8720f_init(struct device *dev)
{
	const struct superio_ite_it8720f_config *conf;
	const struct resource *res;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8720F_EC:
		conf = dev->chip_info;
		res = find_resource(dev, PNP_IDX_IO0);
		if (!conf || !res)
			break;
		ite_ec_init(res->base, &conf->ec);
		power_control_init(dev);
		break;
	case IT8720F_KBCK:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	default:
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = it8720f_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, IT8720F_FDC,
		PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_MSC0 | PNP_MSC1,
		0x0ff8, },
	{ NULL, IT8720F_SP1,
		PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1,
		0x0ff8, },
	{ NULL, IT8720F_SP2,
		PNP_IO0 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1,
		0x0ff8, },
	{ NULL, IT8720F_PP,
		PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_DRQ0 | PNP_MSC0,
		0x0ff8, 0x0ffc, },
	{ NULL, IT8720F_EC,
		PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0 | PNP_MSC1 | PNP_MSC2 |
		PNP_MSC3 | PNP_MSC4 | PNP_MSC5 | PNP_MSC6,
		0x0ff8, 0x0ffc, },
	{ NULL, IT8720F_KBCK,
		PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_MSC0,
		0x0fff, 0x0fff, },
	{ NULL, IT8720F_KBCM,
		PNP_IRQ0 | PNP_MSC0, },
	{ NULL, IT8720F_GPIO,
		PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_MSC0 | PNP_MSC1 | PNP_MSC2 |
		PNP_MSC3 | PNP_MSC4 | PNP_MSC5 | PNP_MSC6 | PNP_MSC7 |
		PNP_MSC8 | PNP_MSC9 | PNP_MSCA | PNP_MSCB | PNP_MSCD | PNP_MSCE,
		0x0ff8, 0x0ff8, 0x0ff8, },
	{ NULL, IT8720F_CIR,
		PNP_IO0 | PNP_IRQ0 | PNP_MSC0,
		0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8720f_ops = {
	CHIP_NAME("ITE IT8720F Super I/O")
	.enable_dev = enable_dev,
};
