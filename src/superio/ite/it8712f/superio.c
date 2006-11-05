/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <uart8250.h>
#include <pc80/keyboard.h>
#include "chip.h"
#include "it8712f.h"

static void init(device_t dev)
{
	struct superio_ite_it8712f_config *conf;
	struct resource *res0, *res1;

	if (!dev->enabled) {
		return;
	}

	conf = dev->chip_info;

	switch (dev->path.u.pnp.device) {
	case IT8712F_FDC: /* TODO. */
		break;
	case IT8712F_SP1:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case IT8712F_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case IT8712F_PP: /* TODO. */
		break;
	case IT8712F_EC: /* TODO. */
		break;
	case IT8712F_KBCK:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
		init_pc_keyboard(res0->base, res1->base, &conf->keyboard);
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
	.enable           = pnp_enable,
	.init             = init,
};

/* TODO: FDC, PP, EC, KBCM, MIDI, GAME, IR. */
static struct pnp_info pnp_dev_info[] = {
 { &ops, IT8712F_SP1,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
 { &ops, IT8712F_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1, { 0x7f8, 0 }, },
 { &ops, IT8712F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7f8, 0 }, { 0x7f8, 0x4}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &pnp_ops,
		sizeof(pnp_dev_info)/sizeof(pnp_dev_info[0]), pnp_dev_info);
}

struct chip_operations superio_ite_it8712f_ops = {
	CHIP_NAME("ITE IT8712F Super I/O")
	.enable_dev = enable_dev,
};

