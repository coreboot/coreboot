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
#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include <superio/conf_mode.h>

#include "npcd378.h"

static void npcd378_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case NPCD378_KBC:
		pc_keyboard_init(PROBE_AUX_DEVICE);
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
