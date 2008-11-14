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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <io.h>
#include <lib.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console.h>
#include <string.h>
#include <uart8250.h>
#include <keyboard.h>
#include <statictree.h>
#include "it8712f.h"

/* Base address 0x2e: 0x87 0x01 0x55 0x55. */
/* Base address 0x4e: 0x87 0x01 0x55 0xaa. */
static void pnp_enter_ext_func_mode(struct device * dev)
{
	outb(0x87, dev->path.pnp.port);
	outb(0x01, dev->path.pnp.port);
	outb(0x55, dev->path.pnp.port);

	if (dev->path.pnp.port == 0x4e) {
		outb(0xaa, dev->path.pnp.port);
	} else {
		outb(0x55, dev->path.pnp.port);
	}
}

static void pnp_exit_ext_func_mode(struct device * dev)
{
	pnp_write_config(dev, 0x02, 0x02);
}

static void it8712f_init(struct device * dev)
{
	struct superio_ite_it8712f_dts_config *conf;
	struct resource *res0, *res1;

	if (!dev->enabled) {
		return;
	}

	conf = dev->device_configuration;

	switch (dev->path.pnp.device) {
	case IT8712F_SP1:
		res0 = find_resource(dev, PNP_IDX_IO0);
#warning no init_uart8250 yet
//		init_uart8250(res0->base, &conf->com1);
		break;
	case IT8712F_SP2:
		res0 = find_resource(dev, PNP_IDX_IO0);
//		init_uart8250(res0->base, &conf->com2);
		break;
	case IT8712F_KBCK:
		res0 = find_resource(dev, PNP_IDX_IO0);
		res1 = find_resource(dev, PNP_IDX_IO1);
#warning no set_kbc_ps2_mode yet
//		set_kbc_ps2_mode();
//		init_pc_keyboard(res0->base, res1->base, &conf->keyboard);
		break;
	}
}

static void it8712f_pnp_set_resources(struct device * dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void it8712f_pnp_enable_resources(struct device * dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_enable_resources(dev);
	pnp_exit_ext_func_mode(dev);
}

static void it8712f_pnp_enable(struct device * dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, dev->enabled);
	pnp_exit_ext_func_mode(dev);
}

static void it8712f_setup_scan_bus(struct device *dev);

struct device_operations it8712f_ops = {
	.phase3_chip_setup_dev	 = it8712f_setup_scan_bus,
	.phase3_enable		 = it8712f_pnp_enable_resources,
	.phase4_read_resources	 = pnp_read_resources,
	.phase4_set_resources	 = it8712f_pnp_set_resources,
	.phase5_enable_resources = it8712f_pnp_enable,
	.phase6_init		 = it8712f_init,
};

static struct pnp_info pnp_dev_info[] = {
				/* Enable,  All resources need by dev,  io_info_structs */
	{&it8712f_ops, IT8712F_FDC, 0, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0xff8, 0},},
	{&it8712f_ops, IT8712F_SP1, 0, PNP_IO0 | PNP_IRQ0, {0xff8, 0},},
	{&it8712f_ops, IT8712F_SP2, 0, PNP_IO0 | PNP_IRQ0, {0xff8, 0},},
	{&it8712f_ops, IT8712F_PP, 0, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0xffc, 0},},
	{&it8712f_ops, IT8712F_EC, 0, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0xff8, 0}, {0xff8, 4},},
	{&it8712f_ops, IT8712F_KBCK, 0, PNP_IO0 | PNP_IO1 | PNP_IRQ0, {0xfff, 0}, {0xfff, 4},},
	{&it8712f_ops, IT8712F_KBCM, 0, PNP_IRQ0,},
	{&it8712f_ops, IT8712F_GPIO, 0, },
	{&it8712f_ops, IT8712F_MIDI, 0, PNP_IO0 | PNP_IRQ0, {0xff8, 0},},
	{&it8712f_ops, IT8712F_GAME, 0, PNP_IO0, {0xfff, 0},},
	{&it8712f_ops, IT8712F_IR, 0, PNP_IO0 | PNP_IRQ0, {0xff8, 0},},
};

static void it8712f_setup_scan_bus(struct device *dev)
{
	const struct superio_ite_it8712f_dts_config * const conf = dev->device_configuration;

	/* Floppy */
	pnp_dev_info[IT8712F_FDC].enable = conf->floppyenable;
	pnp_dev_info[IT8712F_FDC].io0.val = conf->floppyio;
	pnp_dev_info[IT8712F_FDC].irq0.val = conf->floppyirq;
	pnp_dev_info[IT8712F_FDC].drq0.val = conf->floppydrq;

	/* COM1 */
	pnp_dev_info[IT8712F_SP1].enable = conf->com1enable;
	pnp_dev_info[IT8712F_SP1].io0.val = conf->com1io;
	pnp_dev_info[IT8712F_SP1].irq0.val = conf->com1irq;

	/* COM2 */
	pnp_dev_info[IT8712F_SP2].enable = conf->com2enable;
	pnp_dev_info[IT8712F_SP2].io0.val = conf->com2io;
	pnp_dev_info[IT8712F_SP2].irq0.val = conf->com2irq;

	/* Parallel port */
	pnp_dev_info[IT8712F_PP].enable = conf->ppenable;
	pnp_dev_info[IT8712F_PP].io0.val = conf->ppio;
	pnp_dev_info[IT8712F_PP].irq0.val = conf->ppirq;

	/* Environment controller */
	pnp_dev_info[IT8712F_EC].enable = conf->ecenable;
	pnp_dev_info[IT8712F_EC].io0.val = conf->ecio;
	pnp_dev_info[IT8712F_EC].irq0.val = conf->ecirq;

	/* Keyboard */
	pnp_dev_info[IT8712F_KBCK].enable = conf->kbenable;
	pnp_dev_info[IT8712F_KBCK].io0.val = conf->kbio;
	pnp_dev_info[IT8712F_KBCK].io1.val = conf->kbio2;
	pnp_dev_info[IT8712F_KBCK].irq0.val = conf->kbirq;

	/* PS/2 mouse */
	pnp_dev_info[IT8712F_KBCM].enable = conf->mouseenable;
	pnp_dev_info[IT8712F_KBCM].irq0.val = conf->mouseirq;

	/* GPIO */
	pnp_dev_info[IT8712F_GPIO].enable = conf->gpioenable;

	/* MIDI port */
	pnp_dev_info[IT8712F_MIDI].enable = conf->midienable;
	pnp_dev_info[IT8712F_MIDI].io0.val = conf->midiio;
	pnp_dev_info[IT8712F_MIDI].irq0.val = conf->midiirq;

	/* Game port */
	pnp_dev_info[IT8712F_GAME].enable = conf->gameenable;
	pnp_dev_info[IT8712F_GAME].io0.val = conf->gameio;

	/* Consumer IR */
	pnp_dev_info[IT8712F_IR].enable = conf->cirenable;
	pnp_dev_info[IT8712F_IR].io0.val = conf->cirio;
	pnp_dev_info[IT8712F_IR].irq0.val = conf->cirirq;

	/* Initialize SuperIO for PNP children. */
	if (!dev->links) {
		dev->links = 1;
		dev->link[0].dev = dev;
		dev->link[0].children = NULL;
		dev->link[0].link = 0;
	}

	/* Call init with updated tables to create children. */
	pnp_enable_devices(dev, &it8712f_ops,
			   ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}
