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
#include <stdlib.h>
#include "w83977tf.h"

static void w83977tf_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case W83977TF_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_enable,
	.init             = w83977tf_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, W83977TF_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, W83977TF_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, W83977TF_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, W83977TF_SP2,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, W83977TF_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x07ff, 0x07ff, },
	{ NULL, W83977TF_CIR,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, W83977TF_GAME_MIDI_GPIO1, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x07ff, 0x07fe, },
	{ NULL, W83977TF_ACPI, PNP_IRQ0, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83977tf_ops = {
	CHIP_NAME("Winbond W83977TF Super I/O")
	.enable_dev = enable_dev,
};
