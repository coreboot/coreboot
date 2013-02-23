/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <device/device.h>
#include <device/pnp.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include "chip.h"
#include "fdc37n972.h"

static void init(device_t dev)
{
	struct superio_smsc_fdc37n972_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case FDC37N972_FDC: /* TODO. */
		break;
	case FDC37N972_PP: /* TODO. */
		break;
	case FDC37N972_KBDC:
		pc_keyboard_init(&conf->keyboard);
		break;
	// [..] The rest: TODO
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
	{ &ops, FDC37N972_SP1,  PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, FDC37N972_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1, {0x07f8, 0}, },
	{ &ops, FDC37N972_KBDC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0x07f8, 0}, {0x07f8, 4}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &pnp_ops,
		ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_fdc37n972_ops = {
	CHIP_NAME("SMSC FDC37N972 Super I/O")
	.enable_dev = enable_dev,
};

