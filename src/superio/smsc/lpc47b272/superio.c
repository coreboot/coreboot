/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
 * Copyright (C) 2005 Digital Design Corporation
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

/* RAM driver for SMSC LPC47B272 Super I/O chip. */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <console/console.h>
#include <device/smbus.h>
#include <string.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "lpc47b272.h"

/**
 * Initialize the specified Super I/O device.
 *
 * Devices other than COM ports and the keyboard controller are ignored.
 * For COM ports, we configure the baud rate.
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void lpc47b272_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case LPC47B272_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = lpc47b272_init,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, LPC47B272_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, LPC47B272_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, LPC47B272_SP1, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, LPC47B272_SP2, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, LPC47B272_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x07ff, 0x07ff, },
	{ NULL, LPC47B272_RT,  PNP_IO0, 0x0780, },
};

/**
 * Create device structures and allocate resources to devices specified in the
 * pnp_dev_info array (above).
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_lpc47b272_ops = {
	CHIP_NAME("SMSC LPC47B272 Super I/O")
	.enable_dev = enable_dev
};
