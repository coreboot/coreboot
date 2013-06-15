/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/device.h>
#include <device/pnp.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <arch/io.h>
#include <stdlib.h>
#include "chip.h"
#include "it8712f.h"

static void pnp_enter_ext_func_mode(device_t dev)
{
	u16 port = dev->path.pnp.port;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	pnp_write_config(dev, 0x02, 0x02);
}

static void it8712f_init(device_t dev)
{
	struct superio_ite_it8712f_config *conf = dev->chip_info;

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
		pc_keyboard_init(&conf->keyboard);
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

static const struct pnp_mode_ops pnp_conf_mode_ops = {
	.enter_conf_mode  = pnp_enter_ext_func_mode,
	.exit_conf_mode   = pnp_exit_ext_func_mode,
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8712f_init,
	.ops_pnp_mode     = &pnp_conf_mode_ops,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, IT8712F_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x0ff8, 0}, },
	{ &ops, IT8712F_SP1, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	{ &ops, IT8712F_SP2, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	{ &ops, IT8712F_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x0ffc, 0}, },
	{ &ops, IT8712F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0x0ff8, 0}, {0x0ff8, 4}, },
	{ &ops, IT8712F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0x0fff, 0}, {0x0fff, 4}, },
	{ &ops, IT8712F_KBCM, PNP_IRQ0, },
	{ &ops, IT8712F_GPIO, PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IRQ0, {0x0fff, 0}, {0x0ff8, 0}, {0x0ff8, 0}, },
	{ &ops, IT8712F_MIDI, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	{ &ops, IT8712F_GAME, PNP_IO0, {0x0fff, 0}, },
	{ &ops, IT8712F_IR, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &pnp_ops,
		ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8712f_ops = {
	CHIP_NAME("ITE IT8712F Super I/O")
	.enable_dev = enable_dev,
};
