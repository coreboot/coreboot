/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Felix Held <felix-coreboot@felixheld.de>
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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

#include "nct6776.h"

/* Both NCT6776D and NCT6776F package variants are supported. */

static void nct6776_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for HWM or FDC etc. */
	case NCT6776_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nct6776_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT6776_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0,
		0x0ff8, },
	{ NULL, NCT6776_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0,
		0x0ff8, },
	{ NULL, NCT6776_SP1, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6776_SP2, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6776_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	{ NULL, NCT6776_CIR, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6776_ACPI},
	{ NULL, NCT6776_HWM_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x0ffe, 0x0ffe, },
	{ NULL, NCT6776_VID},
	{ NULL, NCT6776_CIRWKUP, PNP_IO0 | PNP_IRQ0,
		0x0ff8, },
	{ NULL, NCT6776_GPIO_PP_OD},
	{ NULL, NCT6776_SVID},
	{ NULL, NCT6776_DSLP},
	{ NULL, NCT6776_GPIOA_LDN},
	{ NULL, NCT6776_WDT1},
	{ NULL, NCT6776_GPIOBASE, PNP_IO0,
		0x0ff8, },
	{ NULL, NCT6776_GPIO0},
	{ NULL, NCT6776_GPIO1},
	{ NULL, NCT6776_GPIO2},
	{ NULL, NCT6776_GPIO3},
	{ NULL, NCT6776_GPIO4},
	{ NULL, NCT6776_GPIO5},
	{ NULL, NCT6776_GPIO6},
	{ NULL, NCT6776_GPIO7},
	{ NULL, NCT6776_GPIO8},
	{ NULL, NCT6776_GPIO9},
	{ NULL, NCT6776_GPIOA},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct6776_ops = {
	CHIP_NAME("NUVOTON NCT6776 Super I/O")
	.enable_dev = enable_dev,
};
