/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <stdlib.h>
#include "pmh7.h"
#include "chip.h"

void pmh7_backlight_enable(int onoff)
{
	if (onoff)
		pmh7_register_set_bit(0x50, 5);
	else
		pmh7_register_clear_bit(0x50, 5);
}

void pmh7_register_set_bit(int reg, int bit)
{
	char val;

	outb(reg, EC_LENOVO_PMH7_ADDR);
	val = inb(EC_LENOVO_PMH7_DATA);
	outb(reg, EC_LENOVO_PMH7_ADDR);
	outb(val | (1 << bit), EC_LENOVO_PMH7_DATA);
}

void pmh7_register_clear_bit(int reg, int bit)
{
	char val;

	outb(reg, EC_LENOVO_PMH7_ADDR);
	val = inb(EC_LENOVO_PMH7_DATA);
	outb(reg, EC_LENOVO_PMH7_ADDR);
	outb(val &= ~(1 << bit), EC_LENOVO_PMH7_DATA);
}

char pmh7_register_read(int reg)
{
	outb(reg, EC_LENOVO_PMH7_ADDR);
	return inb(EC_LENOVO_PMH7_DATA);
}

void pmh7_register_write(int reg, int val)
{
	outb(reg, EC_LENOVO_PMH7_ADDR);
	outb(val, EC_LENOVO_PMH7_DATA);
}

static void enable_dev(device_t dev)
{
	struct ec_lenovo_pmh7_config *conf = dev->chip_info;
	struct resource *resource;

	resource = new_resource(dev, EC_LENOVO_PMH7_INDEX);
	resource->flags = IORESOURCE_IO | IORESOURCE_FIXED;
	resource->base = EC_LENOVO_PMH7_BASE;
	resource->size = 16;
	resource->align = 5;
	resource->gran = 5;

	pmh7_backlight_enable(conf->backlight_enable);
}

struct chip_operations ec_lenovo_pmh7_ops = {
	CHIP_NAME("Lenovo Power Management Hardware Hub 7")
	.enable_dev = enable_dev,
};
