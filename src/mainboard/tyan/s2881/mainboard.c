/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2005 Tyan
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan.
 * Copyright (C) 2007 Ward Vandewege <ward@gnu.org>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <device/device.h>
#include "chip.h"
#include <console/console.h>
#include <device/smbus.h>

/**
 * Do some S2881-specific HWM initialization for the ADT7463 chip.
 *
 * See Analog Devices ADT7463 datasheet, Rev C (2004):
 * http://www.analog.com/en/prod/0,,766_825_ADT7463,00.html
 */
static void dummy_init(device_t dev)
{
	device_t smbus_dev;
	device_t adt7463;
	struct device_path path;

	int result;

	/* Find the SMBus controller (AMD-8111). */
	smbus_dev = dev_find_device(0x1022, 0x746b, 0);
	if (!smbus_dev) {
		die("SMBus controller not found\n");
	}
	printk_debug("SMBus controller found\n");

	/* Find the ADT7463 device. */
	path.type = DEVICE_PATH_I2C;
	path.u.i2c.device = 0x2d;
	adt7463 = find_dev_path(smbus_dev->link, &path);
	if (!adt7463) {
		die("ADT7463 not found\n");
	}
	printk_debug("ADT7463 found\n");

	/* Set all fans to 'Fastest Speed Calculated by All 3 Temperature
	 * Channels Controls PWMx'.
	 */
	result = smbus_write_byte(adt7463, 0x5c, 0xc2);
	result = smbus_write_byte(adt7463, 0x5d, 0xc2);
	result = smbus_write_byte(adt7463, 0x5e, 0xc2);

	/* Make sure that our fans never stop when temp. falls below Tmin, 
	   but rather keep going at minimum duty cycle (applies to automatic 
	   fan control mode only). */
	result = smbus_write_byte(adt7463, 0x62, 0xc0);

	/* Set minimum PWM duty cycle to 25%, rather than the default 50%. */
	result = smbus_write_byte(adt7463, 0x64, 0x40);
	result = smbus_write_byte(adt7463, 0x65, 0x40);
	result = smbus_write_byte(adt7463, 0x66, 0x40);

	/* Set Tmin to 55C, rather than the default 90C. Above this temperature
	   the fans will start blowing harder as temperature increases
	   (automatic mode only). */
	result = smbus_write_byte(adt7463, 0x67, 0x37);
	result = smbus_write_byte(adt7463, 0x68, 0x37);
	result = smbus_write_byte(adt7463, 0x69, 0x37);

	/* Set THERM limit to 70C, rather than the default 100C.
	   The fans will kick in at 100% if the sensors reach this temperature,
	   (only in automatic mode, but supposedly even when hardware is
	   locked up). This is a failsafe measure. */
	result = smbus_write_byte(adt7463, 0x6a, 0x46);
	result = smbus_write_byte(adt7463, 0x6b, 0x46);
	result = smbus_write_byte(adt7463, 0x6c, 0x46);

	/* Remote temperature 1 offset (LSB == 0.25C). */
	result = smbus_write_byte(adt7463, 0x70, 0x02);

	/* Remote temperature 2 offset (LSB == 0.25C). */
	result = smbus_write_byte(adt7463, 0x72, 0x01);

	/* Set TACH measurements to normal (1/second). */
	result = smbus_write_byte(adt7463, 0x78, 0xf0);

	printk_debug("ADT7463 properly initialized");
}

static void dummy_noop(device_t dummy)
{
}

static struct device_operations dummy_operations = {
	.read_resources		= dummy_noop,
	.set_resources		= dummy_noop,
	.enable_resources	= dummy_noop,
	.init			= dummy_init,
};

static unsigned int scan_root_bus(device_t root, unsigned int max)
{
	struct device_path path;
	device_t dummy;
	unsigned link_i;

	max = root_dev_scan_bus(root, max);

	printk_debug("scan_root_bus ok\n");

	/* The following is a little silly. We need a hook into the boot
	 * process *after* the ADT7463 device has been initialized. So we
	 * create this dummy device, and we put the ADT7463 S2881 specific
	 * settings in its init function, which gets called
	 * as the last device to be initialized.
	 */

	link_i = root->links;
	if (link_i >= MAX_LINKS) {
		printk_debug("Reached MAX_LINKS, not configuring ADT7463");
		return max;
	}
	root->link[link_i].link = link_i;
	root->link[link_i].dev = root;
	root->link[link_i].children = 0;
	root->links++;

	path.type = DEVICE_PATH_PNP;
	path.u.pnp.port = 0;
	path.u.pnp.device = 0;
	dummy = alloc_dev(&root->link[link_i], &path);
	dummy->ops = &dummy_operations;

	return max;
}

static struct device_operations mainboard_operations = {
	.read_resources		= root_dev_read_resources,
	.set_resources		= root_dev_set_resources,
	.enable_resources	= root_dev_enable_resources,
	.init			= root_dev_init,
	.scan_bus		= scan_root_bus,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &mainboard_operations;
}

#if CONFIG_CHIP_NAME == 1
struct chip_operations mainboard_tyan_s2881_ops = {
	CHIP_NAME("Tyan S2881 Mainboard")
	.enable_dev = enable_dev,
};
#endif
