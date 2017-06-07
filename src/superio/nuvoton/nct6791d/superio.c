/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Felix Held <felix-coreboot@felixheld.de>
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * Copyright (C) 2015 Matt DeVillier <matt.devillier@gmail.com>
 * Copyright (C) 2016 Omar Pakker <omarpakker+coreboot@gmail.com>
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

#include "nct6791d.h"


static void nct6791d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case NCT6791D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct6791d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, NCT6791D_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0,
		0x0ff8, },
	{ &ops, NCT6791D_SP1, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ &ops, NCT6791D_SP2, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ &ops, NCT6791D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ &ops, NCT6791D_CIR, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ &ops, NCT6791D_ACPI},
	{ &ops, NCT6791D_HWM_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x0ffe, 0x0ffe, },
	{ &ops, NCT6791D_BCLK_WDT2_WDTMEM},
	{ &ops, NCT6791D_CIRWUP, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ &ops, NCT6791D_GPIO_PP_OD},
	{ &ops, NCT6791D_PORT80},
	{ &ops, NCT6791D_WDT1},
	{ &ops, NCT6791D_WDTMEM},
	{ &ops, NCT6791D_GPIOBASE, PNP_IO0,
		0x0ff8, },
	{ &ops, NCT6791D_GPIO0},
	{ &ops, NCT6791D_GPIO1},
	{ &ops, NCT6791D_GPIO2},
	{ &ops, NCT6791D_GPIO3},
	{ &ops, NCT6791D_GPIO4},
	{ &ops, NCT6791D_GPIO5},
	{ &ops, NCT6791D_GPIO6},
	{ &ops, NCT6791D_GPIO7},
	{ &ops, NCT6791D_GPIO8},
	{ &ops, NCT6791D_DS5},
	{ &ops, NCT6791D_DS3},
	{ &ops, NCT6791D_PCHDSW},
	{ &ops, NCT6791D_DSWWOPT},
	{ &ops, NCT6791D_DS3OPT},
	{ &ops, NCT6791D_DSDSS},
	{ &ops, NCT6791D_DSPU},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct6791d_ops = {
	CHIP_NAME("NUVOTON NCT6791D Super I/O")
	.enable_dev = enable_dev,
};
