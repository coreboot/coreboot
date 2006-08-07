/*
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

// #include <arch/io.h>
// #include <device/device.h>
// #include <device/pnp.h>
// #include <console/console.h>
// #include <string.h>
// #include <bitops.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include "chip.h"
#include "it8671f.h"

static void init(device_t dev)
{
	struct superio_ITE_it8671f_config *conf;
	struct resource *res0, *res1;

	if (!dev->enabled) {
		return;
	}

	conf = dev->chip_info;

	switch(dev->path.u.pnp.device) {
	case IT8671F_SP1:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com1);
		break;
	case IT8671F_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
		init_uart8250(res0->base, &conf->com2);
		break;
	case IT8671F_KBCK:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
		init_pc_keyboard(res0->base, res1->base, &conf->keyboard);
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

// TODO.
static struct pnp_info pnp_dev_info[] = {
// { &ops, IT8671F_FDC,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x07fa, 0}, },
// { &ops, IT8671F_PP,   PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, { 0x04f8, 0}, },
 { &ops, IT8671F_SP2,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0 | PNP_DRQ1, { 0x7f8, 0 }, },
 { &ops, IT8671F_SP1,  PNP_IO0 | PNP_IRQ0, { 0x7f8, 0 }, },
// { &ops, IT8671F_SWC,  PNP_IO0 | PNP_IRQ0, { 0xfff0, 0 }, },
// { &ops, IT8671F_KBCM, PNP_IRQ0 },
{ &ops, IT8671F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, { 0x7f8, 0 }, { 0x7f8, 0x4}, },
// { &ops, IT8671F_GPIO, PNP_IO0 | PNP_IRQ0, { 0xfff8, 0 } },
// { &ops, IT8671F_XBUS, PNP_IO0 | PNP_IRQ0, { 0xffe0, 0 } },
// { &ops, IT8671F_RTC,  PNP_IO0 | PNP_IO1, { 0xfffe, 0 }, {0xfffe, 0x4} },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &pnp_ops,
		sizeof(pnp_dev_info)/sizeof(pnp_dev_info[0]), pnp_dev_info);
}

struct chip_operations superio_ITE_it8671f_ops = {
	CHIP_NAME("ITE it8671f")
	.enable_dev = enable_dev,
};

