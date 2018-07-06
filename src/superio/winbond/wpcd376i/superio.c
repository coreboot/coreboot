/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
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

#include <device/device.h>
#include <device/pnp.h>
#include <drivers/uart/uart8250reg.h>
#include <pc80/keyboard.h>
#include <arch/io.h>
#include <stdlib.h>
#include "chip.h"
#include "wpcd376i.h"

static void init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {

	case WPCD376I_FDC:
	case WPCD376I_LPT:
	case WPCD376I_IR:
	case WPCD376I_SP1:
	case WPCD376I_SWC:
	case WPCD376I_KBCM:
	case WPCD376I_GPIO:
		break;

	case WPCD376I_KBCK:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources		= pnp_read_resources,
	.set_resources		= pnp_set_resources,
	.enable_resources	= pnp_enable_resources,
	.enable			= pnp_enable,
	.init			= init,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, WPCD376I_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07fa, },
	{ NULL, WPCD376I_LPT,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x04f8, },
	{ NULL, WPCD376I_IR,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1,
		0x07f8, },
	{ NULL, WPCD376I_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, WPCD376I_KBCM, PNP_IRQ0, },
	{ NULL, WPCD376I_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x07f8, 0x07f8, },
	{ NULL, WPCD376I_GPIO, PNP_IO0 | PNP_IRQ0, 0xfff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_wpcd376i_ops = {
	CHIP_NAME("Winbond WPCD376I Super I/O")
	.enable_dev = enable_dev,
};
