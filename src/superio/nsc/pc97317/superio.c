/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
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
#include <stdlib.h>
#include <pc80/keyboard.h>
#include "pc97317.h"

static void init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case PC97317_KBCK:
		pnp_set_logical_device(dev);
		pnp_set_enable(dev, 0);		   /* Disable keyboard */
		pnp_write_config(dev, 0xf0, 0x40); /* Set KBC clock to 8 MHz. */
		pnp_set_enable(dev, 1);		   /* Enable keyboard */
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
	.init             = init,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, PC97317_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x0ffb, 0x0ffb, },
	{ NULL, PC97317_KBCM, PNP_IRQ0, },
	{ NULL, PC97317_RTC,  PNP_IO0 | PNP_IRQ0, 0xfffe, },
	{ NULL, PC97317_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0xfffa, },
	{ NULL, PC97317_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x03fc, },
	{ NULL, PC97317_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1,
		0xfff8, },
	{ NULL, PC97317_SP1,  PNP_IO0 | PNP_IRQ0, 0xfff8, },
	{ NULL, PC97317_GPIO, PNP_IO0, 0xfff8, },
	{ NULL, PC97317_PM,   PNP_IO0, 0xfffe, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc97317_ops = {
	CHIP_NAME("NSC PC97317 Super I/O")
	.enable_dev = enable_dev,
};
