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
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <stdlib.h>
#include <pc80/mc146818rtc.h>

#include "pmh7.h"
#include "chip.h"

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

void pmh7_trackpoint_enable(int onoff)
{
	if (onoff)
		pmh7_register_clear_bit(0x51, 0);
	else
		pmh7_register_set_bit(0x51, 0);
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

	val = pmh7_register_read(reg);
	pmh7_register_write(reg, val | (1 << bit));
}

void pmh7_register_clear_bit(int reg, int bit)
{
	char val;

	val = pmh7_register_read(reg);
	pmh7_register_write(reg, val & ~(1 << bit));
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
static void enable_dev(struct device *dev)
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

	if (get_option(&val, "touchpad") != CB_SUCCESS)
		val = 1;
	pmh7_touchpad_enable(val);

	if (get_option(&val, "trackpoint") != CB_SUCCESS)
		val = 1;
	pmh7_trackpoint_enable(val);

	printk(BIOS_INFO, "PMH7: ID %02x Revision %02x\n",
	       pmh7_register_read(EC_LENOVO_PMH7_REG_ID),
	       pmh7_register_read(EC_LENOVO_PMH7_REG_REV));
}

struct chip_operations ec_lenovo_pmh7_ops = {
	CHIP_NAME("Lenovo Power Management Hardware Hub 7")
	.enable_dev = enable_dev,
};
#endif
#endif
