/*
 * This file is part of the coreboot project.
 *
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
#include <superio/conf_mode.h>
#include <console/console.h>
#include <stdlib.h>

#include "chip.h"
#include "f81216h.h"


static void pnp_enter_ext_func_mode(struct device *dev)
{
	const struct superio_fintek_f81216h_config *conf = dev->chip_info;

	u8 key;

	/**
	 *  KEY1   KEY0   Enter key
	 *  0      0      0x77 (default)
	 *  0      1      0xA0
	 *  1      0      0x87
	 *  1      1      0x67
	 *
	 *  See page 17 of data sheet.
	 */
	switch(conf->conf_key_mode) {
	case MODE_6767:
	case MODE_7777:
	case MODE_8787:
	case MODE_A0A0:
		key = conf->conf_key_mode;
		break;
	default:
		printk(BIOS_WARNING, "Warning: Undefined F81216 unlock key assignment!\n");
		printk(BIOS_WARNING, "Setting conf_key_mode to default\n");
		key = MODE_7777; /* try the hw default */
		break;
	}

	outb(key, dev->path.pnp.port);
	outb(key, dev->path.pnp.port);
}

static void pnp_exit_ext_func_mode(struct device *dev)
{
	outb(0xaa, dev->path.pnp.port);
}

static const struct pnp_mode_ops pnp_conf_mode_ops = {
	.enter_conf_mode  = pnp_enter_ext_func_mode,
	.exit_conf_mode   = pnp_exit_ext_func_mode,
};


static void f81216h_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case F81216H_SP1:
	case F81216H_SP2:
	case F81216H_SP3:
	case F81216H_SP4:
	case F81216H_WDT:
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = f81216h_init,
	.ops_pnp_mode     = &pnp_conf_mode_ops,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, F81216H_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F81216H_SP2,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F81216H_SP3,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F81216H_SP4,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, F81216H_WDT, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_fintek_f81216h_ops = {
	CHIP_NAME("Fintek F81216H/D/DG/F/FG Super I/O")
	.enable_dev = enable_dev
};
