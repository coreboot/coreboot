/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

/* RAM driver for the SMSC KBC1100 Super I/O chip */

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
#include "sch4037.h"

/* Forward declarations */
static void enable_dev(device_t dev);
static void sch4037_init(device_t dev);

struct chip_operations superio_smsc_sch4037_ops = {
	CHIP_NAME("SMSC SCH4037 Super I/O")
		.enable_dev = enable_dev,
};

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = sch4037_init,
	.ops_pnp_mode     = &pnp_conf_mode_55_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, SCH4037_KBC,  PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1, { 0x7ff, 0 }, { 0x7ff, 0x4}, },
};

static void enable_dev(device_t dev)
{
	printk(BIOS_SPEW, "file '%s',line %d, %s()\n", __FILE__, __LINE__, __func__);
	pnp_enable_devices(dev, &pnp_ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

static void sch4037_init(device_t dev)
{
	struct superio_smsc_sch4037_config *conf = dev->chip_info;
	struct resource *res0, *res1;

	if (!dev->enabled) {
		return;
	}

	switch(dev->path.pnp.device) {

		case SCH4037_KBC:
			res0 = find_resource(dev, PNP_IDX_IO0);
			res1 = find_resource(dev, PNP_IDX_IO1);
			pc_keyboard_init(&conf->keyboard);
			break;
	}
}
