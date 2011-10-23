/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include "chip.h"
#include <string.h>

static void ics954309_init(device_t dev)
{
	struct drivers_ics_954309_config *config;
	u8 initdata[12];

	if (!dev->enabled || dev->path.type != DEVICE_PATH_I2C)
		return;

	config = dev->chip_info;

	initdata[0] = config->reg0;
	initdata[1] = config->reg1;
	initdata[2] = config->reg2;
	initdata[3] = config->reg3;
	initdata[4] = config->reg4;
	initdata[5] = config->reg5;
	initdata[6] = config->reg6;
	initdata[7] = config->reg7;
	initdata[8] = config->reg8;
	initdata[9] = config->reg9;
	initdata[10] = config->reg10;
	initdata[11] = config->reg11;

	smbus_block_write(dev, 0, 12, initdata);
}

static void ics954309_noop(device_t dummy)
{
}

static struct device_operations ics954309_operations = {
        .read_resources   = ics954309_noop,
        .set_resources    = ics954309_noop,
        .enable_resources = ics954309_noop,
        .init             = ics954309_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &ics954309_operations;
}

struct chip_operations drivers_ics_954309_ops = {
	CHIP_NAME("ICS 954309 Clock generator")
	.enable_dev = enable_dev,
};
