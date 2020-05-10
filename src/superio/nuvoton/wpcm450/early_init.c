/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pnp_def.h>
#include "wpcm450.h"

static void wpcm450_pnp_set_logical_device(u8 dev, u16 port)
{
	outb(0x7, port);
	outb(dev, port+1);
}

static void wpcm450_pnp_set_enable(u8 dev, u16 port, u8 enable)
{
	outb(0x30, port);
	outb(enable, port+1);
}

static void wpcm450_pnp_set_iobase(u8 dev, u16 port, u8 index, u16 iobase)
{
	outb(index, port);
	outb((iobase>>8)&0xFF, port+1);
	outb(index+1, port);
	outb(iobase&0xFF, port+1);
}

void wpcm450_enable_dev(u8 dev, u16 port, u16 iobase)
{
	wpcm450_pnp_set_logical_device(dev, port);
	wpcm450_pnp_set_enable(dev, port, 0);
	if (iobase)
		wpcm450_pnp_set_iobase(dev, port, PNP_IDX_IO0, iobase);
	wpcm450_pnp_set_enable(dev, port, 1);
}
