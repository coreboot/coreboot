/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Felix Held <felix-coreboot@felixheld.de>
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * Copyright (C) 2015 Matt DeVillier <matt.devillier@gmail.com>
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

#include "nct6779d.h"


static void nct6779d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case NCT6779D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct6779d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT6779D_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8, },
	{ NULL, NCT6779D_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT6779D_SP2, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT6779D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ NULL, NCT6779D_CIR, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT6779D_ACPI},
	{ NULL, NCT6779D_HWM_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x0ffe, 0x0ffe, },
	{ NULL, NCT6779D_WDT1},
	{ NULL, NCT6779D_CIRWKUP, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT6779D_GPIO_PP_OD},
	{ NULL, NCT6779D_PRT80},
	{ NULL, NCT6779D_DSLP},
	{ NULL, NCT6779D_GPIOBASE, PNP_IO0, 0x0ff8, },
	{ NULL, NCT6779D_GPIO0},
	{ NULL, NCT6779D_GPIO1},
	{ NULL, NCT6779D_GPIO2},
	{ NULL, NCT6779D_GPIO3},
	{ NULL, NCT6779D_GPIO4},
	{ NULL, NCT6779D_GPIO5},
	{ NULL, NCT6779D_GPIO6},
	{ NULL, NCT6779D_GPIO7},
	{ NULL, NCT6779D_GPIO8},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct6779d_ops = {
	CHIP_NAME("NUVOTON NCT6779D Super I/O")
	.enable_dev = enable_dev,
};
