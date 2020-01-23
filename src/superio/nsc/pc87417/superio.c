/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>

#include "pc87417.h"

static void init(struct device *dev)
{
	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case PC87417_KBCK:
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
	{ NULL, PC87417_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, PC87417_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, PC87417_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1,
		0x07f8, },
	{ NULL, PC87417_SP1,  PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, PC87417_SWC,  PNP_IO0 | PNP_IRQ0, 0xfff0, },
	{ NULL, PC87417_KBCM, PNP_IRQ0, },
	{ NULL, PC87417_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x07f8, 0x07f8, },
	{ NULL, PC87417_GPIO, PNP_IO0 | PNP_IRQ0, 0xfff8, },
	{ NULL, PC87417_XBUS, PNP_IO0 | PNP_IRQ0, 0xffe0, },
	{ NULL, PC87417_RTC,  PNP_IO0 | PNP_IO1, 0xfffe, 0xfffe, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nsc_pc87417_ops = {
	CHIP_NAME("NSC PC87417 Super I/O")
	.enable_dev = enable_dev,
};
