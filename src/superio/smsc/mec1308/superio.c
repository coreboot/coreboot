/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* RAM driver for the SMSC MEC1308 Super I/O chip */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <device/smbus.h>
#include <string.h>
#include <pc80/keyboard.h>
#include <stdlib.h>
#include "chip.h"
#include "mec1308.h"
#if CONFIG_HAVE_ACPI_RESUME
#include <arch/acpi.h>
#endif

static void pnp_enter_conf_state(device_t dev)
{
	outb(0x55, dev->path.pnp.port);
}

static void pnp_exit_conf_state(device_t dev)
{
	outb(0xaa, dev->path.pnp.port);
}

static void mec1308_pnp_set_resources(device_t dev)
{
	pnp_set_resources(dev);
}

static void mec1308_pnp_enable_resources(device_t dev)
{
	pnp_enable_resources(dev);
}

static void mec1308_pnp_enable(device_t dev)
{
	pnp_alt_enable(dev);
}

static void mec1308_init(device_t dev)
{
	struct superio_smsc_mec1308_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;

	switch(dev->path.pnp.device) {
	case MEC1308_KBC:
#if CONFIG_HAVE_ACPI_RESUME
		if (acpi_slp_type == 3)
			return;
#endif
		pc_keyboard_init(&conf->keyboard);
		break;
	}
}

static const struct pnp_mode_ops pnp_conf_mode_ops = {
	.enter_conf_mode  = pnp_enter_conf_state,
	.exit_conf_mode   = pnp_exit_conf_state,
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = mec1308_pnp_set_resources,
	.enable_resources = mec1308_pnp_enable_resources,
	.enable           = mec1308_pnp_enable,
	.init             = mec1308_init,
	.ops_pnp_mode     = &pnp_conf_mode_ops,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, MEC1308_PM1,  PNP_IO0, { 0x7ff, 0 } },
	{ &ops, MEC1308_EC1,  PNP_IO0, { 0x7ff, 0 } },
	{ &ops, MEC1308_EC2,  PNP_IO0, { 0x7ff, 0 } },
	{ &ops, MEC1308_UART, PNP_IO0 | PNP_IRQ0, {0x0ff8, 0}, },
	{ &ops, MEC1308_KBC,  PNP_IRQ0, { 0, 0 } /* IO Fixed at 0x60/0x64 */ },
	{ &ops, MEC1308_EC0,  PNP_IO0, { 0x7ff, 0 } },
	{ &ops, MEC1308_MBX,  PNP_IO0, { 0x7ff, 0 } },
};

static void enable_dev(device_t dev)
{
	pnp_enable_devices(dev, &pnp_ops,
			   ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_smsc_mec1308_ops = {
	CHIP_NAME("SMSC MEC1308 EC SuperIO Interface")
	.enable_dev = enable_dev
};

