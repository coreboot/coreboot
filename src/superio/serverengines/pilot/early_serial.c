/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

/* PILOT Super I/O is only based on LPC observation done on factory system. */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include "pilot.h"

/* Pilot uses 0x5A/0xA5 pattern to activate deactivate config access. */
void pnp_enter_ext_func_mode(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0x5A, port);
}

void pnp_exit_ext_func_mode(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0xA5, port);
}

/* Serial config is a fairly standard procedure. */
void pilot_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);
}

void pilot_disable_serial(pnp_devfn_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x0000);
	pnp_set_enable(dev, 0);
	pnp_exit_ext_func_mode(dev);
}
