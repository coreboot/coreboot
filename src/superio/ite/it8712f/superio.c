/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <device/device.h>
#include <device/pnp.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>

#include "it8712f.h"

static void it8712f_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8712F_FDC: /* TODO. */
		break;
	case IT8712F_PP: /* TODO. */
		break;
	case IT8712F_EC: /* TODO. */
		break;
	case IT8712F_KBCK:
		set_kbc_ps2_mode();
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case IT8712F_KBCM: /* TODO. */
		break;
	case IT8712F_MIDI: /* TODO. */
		break;
	case IT8712F_GAME: /* TODO. */
		break;
	case IT8712F_IR: /* TODO. */
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8712f_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, IT8712F_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8, },
	{ NULL, IT8712F_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, IT8712F_SP2, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, IT8712F_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ffc, },
	{ NULL, IT8712F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x0ff8, 0x0ff8, },
	{ NULL, IT8712F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x0fff, 0x0fff, },
	{ NULL, IT8712F_KBCM, PNP_IRQ0, },
	{ NULL, IT8712F_GPIO, PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IRQ0,
		0x0fff, 0x0ff8, 0x0ff8, },
	{ NULL, IT8712F_MIDI, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, IT8712F_GAME, PNP_IO0, 0x0fff, },
	{ NULL, IT8712F_IR, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8712f_ops = {
	CHIP_NAME("ITE IT8712F Super I/O")
	.enable_dev = enable_dev,
};
