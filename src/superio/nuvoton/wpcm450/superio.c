/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>

#include "wpcm450.h"

static void init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case WPCM450_KBCK:
		pc_keyboard_init(NO_AUX_DEVICE);
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
	{ NULL, WPCM450_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1,
		0x07f8, },
	{ NULL, WPCM450_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, WPCM450_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x07ff, 0x07ff, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_wpcm450_ops = {
	CHIP_NAME("Nuvoton WPCM450 Super I/O")
	.enable_dev = enable_dev,
};
