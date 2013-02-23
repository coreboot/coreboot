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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#ifndef __PRE_RAM__
#ifndef __SMM__
#include <device/device.h>
#include <device/pnp.h>
#endif
#endif
#include <stdlib.h>
#include "pmh7.h"
#include "chip.h"
#include <pc80/mc146818rtc.h>

void pmh7_backlight_enable(int onoff)
{
	if (onoff)
		pmh7_register_set_bit(0x50, 5);
	else
		pmh7_register_clear_bit(0x50, 5);
}

void pmh7_dock_event_enable(int onoff)
{
	if (onoff)
		pmh7_register_set_bit(0x60, 3);
	else
		pmh7_register_clear_bit(0x60, 3);

}

void pmh7_touchpad_enable(int onoff)
{
	if (onoff)
		pmh7_register_clear_bit(0x51, 2);
	else
		pmh7_register_set_bit(0x51, 2);
}

void pmh7_ultrabay_power_enable(int onoff)
{
	if (onoff)
		pmh7_register_clear_bit(0x62, 0);
	else
		pmh7_register_set_bit(0x62, 0);
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

#ifndef __PRE_RAM__
#ifndef __SMM__
static void enable_dev(device_t dev)
{
	struct ec_lenovo_pmh7_config *conf = dev->chip_info;
	struct resource *resource;
	u8 val;

	resource = new_resource(dev, EC_LENOVO_PMH7_INDEX);
	resource->flags = IORESOURCE_IO | IORESOURCE_FIXED;
	resource->base = EC_LENOVO_PMH7_BASE;
	resource->size = 16;
	resource->align = 5;
	resource->gran = 5;

	pmh7_backlight_enable(conf->backlight_enable);
	pmh7_dock_event_enable(conf->dock_event_enable);

	if (!get_option(&val, "touchpad"))
		pmh7_touchpad_enable(val);
}

struct chip_operations ec_lenovo_pmh7_ops = {
	CHIP_NAME("Lenovo Power Management Hardware Hub 7")
	.enable_dev = enable_dev,
};
#endif
#endif
