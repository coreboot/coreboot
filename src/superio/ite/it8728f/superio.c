/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * Copyright (C) 2013 Rudolf Marek <r.marek@assembler.cz>
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
#include "it8728f.h"

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

static u8 hwm_read(u16 base, u8 reg)
{
	outb(reg, base + 5);
	return inb(base + 6);
}

static void hwm_write(u16 base, u8 reg, u8 val)
{
	outb(reg, base + 5);
	outb(val, base + 6);
}

static void it8728f_init(device_t dev)
{
	struct superio_ite_it8728f_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case IT8728F_FDC: /* TODO. */
		break;
	case IT8728F_PP: /* TODO. */
		break;
	case IT8728F_EC:
		{
		/* keep it simple for now, configuration options may come later */
		u8 tmp;
		u16 base;
		pnp_enter_ext_func_mode(dev);
		pnp_set_logical_device(dev);
		/* there must be better way to read the I/O base */
		base = ((u16) pnp_read_config(dev, 0x60) << 8);
		base |= pnp_read_config(dev, 0x61);
		tmp = hwm_read(base, 0x14);
		/* setup fan polarity */
		hwm_write(base, 0x14, tmp | (1 << 7));
		/* enable fans 1-3 */
		tmp = hwm_read(base, 0x13);
		hwm_write(base, 0x13, tmp | (7 << 4));
		/* enable termistor temps for temp1-temp3 */
		tmp = hwm_read(base, 0x51);
		hwm_write(base, 0x51, tmp | (7 << 3));
		/* fan2 is using temp1 */
		hwm_write(base, 0x16, 0x80);
		pnp_exit_ext_func_mode(dev);
		break;
		}
	case IT8728F_KBCK:
		set_kbc_ps2_mode();
		pc_keyboard_init(&conf->keyboard);
		break;
	case IT8728F_KBCM: /* TODO. */
		break;
	case IT8728F_IR: /* TODO. */
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
	.init             = it8728f_init,
	.ops_pnp_mode     = &pnp_conf_mode_ops,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, IT8728F_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x0ff8, 0}, },
	{ &ops, IT8728F_SP1, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	{ &ops, IT8728F_SP2, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	{ &ops, IT8728F_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x0ffc, 0}, },
	{ &ops, IT8728F_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0x0ff8, 0}, {0x0ff8, 4}, },
	{ &ops, IT8728F_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0x0fff, 0}, {0x0fff, 4}, },
	{ &ops, IT8728F_KBCM, PNP_IRQ0, },
	{ &ops, IT8728F_GPIO, PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IRQ0, {0x0fff, 0}, {0x0ff8, 0}, {0x0ff8, 0}, },
	{ &ops, IT8728F_IR, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &pnp_ops,
		ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8728f_ops = {
	CHIP_NAME("ITE IT8728F Super I/O")
	.enable_dev = enable_dev,
};
