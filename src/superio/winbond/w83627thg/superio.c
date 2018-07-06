/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
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
#include <superio/conf_mode.h>
#include <console/console.h>
#include <string.h>
#include <pc80/keyboard.h>
#include "w83627thg.h"

static void w83627thg_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case W83627THG_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = w83627thg_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, W83627THG_FDC,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, W83627THG_PP,    PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, W83627THG_SP1,   PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, W83627THG_SP2,   PNP_IO0 | PNP_IRQ0 | PNP_MSC1, 0x07f8, },
	{ NULL, W83627THG_KBC,
		PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1 | PNP_MSC0,
		0x07ff, 0x07ff, },
	{ NULL, W83627THG_GAME_MIDI_GPIO1, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x07ff, 0x07fe, },
	{ NULL, W83627THG_GPIO2, },
	{ NULL, W83627THG_GPIO3, PNP_EN | PNP_MSC0 | PNP_MSC1, },
	{ NULL, W83627THG_ACPI,  PNP_IRQ0, },
	{ NULL, W83627THG_HWM,   PNP_IO0 | PNP_IRQ0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83627thg_ops = {
	CHIP_NAME("Winbond W83627THG Super I/O")
	.enable_dev = enable_dev,
};
