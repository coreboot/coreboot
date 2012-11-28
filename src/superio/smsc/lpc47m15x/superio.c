/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

/* RAM driver for the SMSC LPC47M15X Super I/O chip */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <device/smbus.h>
#include <string.h>
#include <uart8250.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"
#include "lpc47m15x.h"

/* Forward declarations */
static void enable_dev(device_t dev);
static void lpc47m15x_pnp_set_resources(device_t dev);
static void lpc47m15x_pnp_enable_resources(device_t dev);
static void lpc47m15x_pnp_enable(device_t dev);
static void lpc47m15x_init(device_t dev);

static void pnp_enter_conf_state(device_t dev);
static void pnp_exit_conf_state(device_t dev);

struct chip_operations superio_smsc_lpc47m15x_ops = {
	CHIP_NAME("SMSC LPC47M15x/192/997 Super I/O")
	.enable_dev = enable_dev
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = lpc47m15x_pnp_set_resources,
	.enable_resources = lpc47m15x_pnp_enable_resources,
	.enable           = lpc47m15x_pnp_enable,
	.init             = lpc47m15x_init,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, LPC47M15X_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, LPC47M15X_PP,  PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, {0x07f8, 0}, },
	{ &ops, LPC47M15X_SP1, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, LPC47M15X_SP2, PNP_IO0 | PNP_IRQ0, {0x07f8, 0}, },
	{ &ops, LPC47M15X_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, {0x07ff, 0}, {0x07ff, 4}, },
};

static void enable_dev(device_t dev)
{
	pnp_enable_devices(dev, &pnp_ops,
			   ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

static void lpc47m15x_pnp_set_resources(device_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_set_resources(dev);
	pnp_exit_conf_state(dev);
}

static void lpc47m15x_pnp_enable_resources(device_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_enable_resources(dev);
	pnp_exit_conf_state(dev);
}

static void lpc47m15x_pnp_enable(device_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, !!dev->enabled);
	pnp_exit_conf_state(dev);
}

static void lpc47m15x_init(device_t dev)
{
	struct superio_smsc_lpc47m15x_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case LPC47M15X_KBC:
		pc_keyboard_init(&conf->keyboard);
		break;
	}
}

static void pnp_enter_conf_state(device_t dev)
{
	outb(0x55, dev->path.pnp.port);
}

static void pnp_exit_conf_state(device_t dev)
{
	outb(0xaa, dev->path.pnp.port);
}
