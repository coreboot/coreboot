/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * Copyright (C) 2015 BAP - Bruhnspace Advanced Projects
 * (Written by Fabian Kunkel <fabi@adv.bruhnspace.com> for BAP)
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
#include <pc80/keyboard.h>
#include "f81866d.h"
#include "fintek_internal.h"

static void f81866d_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need extra code for serial, wdt etc. */
	case F81866D_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case F81866D_HWM:
		// Fixing temp sensor read out and init Fan control
		f81866d_hwm_init(dev);
		break;
	case F81866D_SP1:
		// Enable Uart1 and IRQ share register
		f81866d_uart_init(dev);
		break;
	case F81866D_SP2:
		// Enable Uart2 and IRQ share register
		f81866d_uart_init(dev);
		break;
	case F81866D_SP3:
		// Enable Uart3 and IRQ share register
		f81866d_uart_init(dev);
		break;
	case F81866D_SP4:
		// Enable Uart4 and IRQ share register
		f81866d_uart_init(dev);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = f81866d_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	/* TODO: Some of the 0x7f8 etc. values may not be correct. */
	{ NULL, F81866D_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, F81866D_SP1,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81866D_SP2,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81866D_SP3,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81866D_SP4,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81866D_SP5,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81866D_SP6,  PNP_IO0 | PNP_IRQ0, 0x7f8, },
	{ NULL, F81866D_KBC,  PNP_IO0 | PNP_IRQ0 | PNP_IRQ1, 0x07ff, },
	{ NULL, F81866D_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, F81866D_HWM,  PNP_IO0 | PNP_IRQ0, 0xff8, },
	{ NULL, F81866D_GPIO, PNP_IRQ0, },
	{ NULL, F81866D_PME, },
	{ NULL, F81866D_WDT, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_fintek_f81866d_ops = {
	CHIP_NAME("Fintek F81866AD-I Super I/O")
	.enable_dev = enable_dev
};
