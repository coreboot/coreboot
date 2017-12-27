/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghai.lu@amd.com> for AMD)
 * Copyright (C) 2007 Ward Vandewege <ward@gnu.org>
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
#include <console/console.h>
#include <pc80/keyboard.h>
#include <arch/io.h>
#include <stdlib.h>
#include <superio/conf_mode.h>

#include "it8716f.h"

#if !IS_ENABLED(CONFIG_SUPERIO_ITE_IT8716F_OVERRIDE_FANCTL)

void init_ec(u16 base)
{
	u8 value;

	/* Read out current value of FAN_CTL (0x14). */
	value = pnp_read_index(base, 0x14);
	printk(BIOS_DEBUG, "FAN_CTL: reg = 0x%04x, read value = 0x%02x\n",
	       base + 0x14, value);

	/* Set FAN_CTL (0x14) polarity to high, activate fans 1, 2 and 3. */
	pnp_write_index(base, 0x14, value | 0x87);
	printk(BIOS_DEBUG, "FAN_CTL: reg = 0x%04x, writing value = 0x%02x\n",
	       base + 0x14, value | 0x87);
}
#endif

static void it8716f_init(struct device *dev)
{
	struct resource *res0;

	if (!dev->enabled)
		return;

	/* TODO: FDC, PP, KBCM, MIDI, GAME, IR. */
	switch (dev->path.pnp.device) {
	case IT8716F_EC:
		res0 = find_resource(dev, PNP_IDX_IO0);
#define EC_INDEX_PORT 5
		init_ec(res0->base + EC_INDEX_PORT);
		break;
	case IT8716F_KBCK:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8716f_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, IT8716F_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, IT8716F_SP1, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, IT8716F_SP2, PNP_IO0 | PNP_IRQ0, 0x07f8, },
	{ NULL, IT8716F_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x07f8, },
	{ NULL, IT8716F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x07f8, 0x07f8, },
	{ NULL, IT8716F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x07ff, 0x07ff, },
	{ NULL, IT8716F_KBCM, PNP_IRQ0, },
	{ NULL, IT8716F_GPIO, PNP_IO1 | PNP_IO2, 0, 0x07f8, 0x07f8, },
	{ NULL, IT8716F_MIDI, PNP_IO0 | PNP_IRQ0, 0x07fe, },
	{ NULL, IT8716F_GAME, PNP_IO0, 0x07ff, },
	{ NULL, IT8716F_IR, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8716f_ops = {
	CHIP_NAME("ITE IT8716F Super I/O")
	.enable_dev = enable_dev,
};
