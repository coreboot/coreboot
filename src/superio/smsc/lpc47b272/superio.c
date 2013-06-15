/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
 * Copyright (C) 2005 Digital Design Corporation
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

/* RAM driver for SMSC LPC47B272 Super I/O chip. */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <console/console.h>
#include <device/smbus.h>
#include <string.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"
#include "lpc47b272.h"

/* Forward declarations */
static void enable_dev(device_t dev);
static void lpc47b272_init(device_t dev);
// static void dump_pnp_device(device_t dev);

struct chip_operations superio_smsc_lpc47b272_ops = {
	CHIP_NAME("SMSC LPC47B272 Super I/O")
	.enable_dev = enable_dev
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = lpc47b272_init,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, LPC47B272_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, LPC47B272_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, LPC47B272_SP1, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, LPC47B272_SP2, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, LPC47B272_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, {0x07ff, 0}, {0x07ff, 4}, },
	{ &ops, LPC47B272_RT,  PNP_IO0, {0x0780, 0}, },
};

/**
 * Create device structures and allocate resources to devices specified in the
 * pnp_dev_info array (above).
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void enable_dev(device_t dev)
{
	pnp_enable_devices(dev, &pnp_ops, ARRAY_SIZE(pnp_dev_info),
			   pnp_dev_info);
}

/**
 * Initialize the specified Super I/O device.
 *
 * Devices other than COM ports and the keyboard controller are ignored.
 * For COM ports, we configure the baud rate.
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void lpc47b272_init(device_t dev)
{
	struct superio_smsc_lpc47b272_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case LPC47B272_KBC:
		pc_keyboard_init(&conf->keyboard);
		break;
	}
}

#if 0
/**
 * Print the values of all of the LPC47B272's configuration registers.
 *
 * NOTE: The LPC47B272 must be in config mode when this function is called.
 *
 * @param dev Pointer to structure describing a Super I/O device.
 */
static void dump_pnp_device(device_t dev)
{
	int i;
	print_debug("\n");

	for (i = 0; i <= LPC47B272_MAX_CONFIG_REGISTER; i++) {
		u8 register_value;

		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}

		/*
		 * Skip over 'register' that would cause exit from
		 * configuration mode.
		 */
		if (i == 0xaa)
			register_value = 0xaa;
		else
			register_value = pnp_read_config(dev, i);

		print_debug_char(' ');
		print_debug_hex8(register_value);
		if ((i & 0x0f) == 0x0f)
			print_debug("\n");
	}

   	print_debug("\n");
}
#endif
