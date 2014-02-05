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
#include <pc80/keyboard.h>
#include "chip.h"
#include "sio10n268.h"

static void init(device_t dev)
{
	struct superio_smsc_sio10n268_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case SIO10N268_FDC: /* TODO. */
		break;
	case SIO10N268_PP: /* TODO. */
		break;
	case SIO10N268_KBDC:
		/* TODO: This is still hardcoded. */
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

/* TODO: FDC, PP, AUX. */
static struct pnp_info pnp_dev_info[] = {
	{ &ops, SIO10N268_KBDC, PNP_IO0 | PNP_IO1, {0x07f8, 0}, {0x07f8, 4}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &pnp_ops,
		ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_sio10n268_ops = {
	CHIP_NAME("SMSC SIO10N268 Super I/O")
	.enable_dev = enable_dev,
};

