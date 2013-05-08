/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <console/console.h>
#include <device/device.h>
#include <pc80/mc146818rtc.h>

static void init(struct device *dev)
{
	u8 defaults_loaded = 0;

	printk(BIOS_DEBUG, "ALIX1.C ENTER %s\n", __func__);

	if (get_option(&defaults_loaded, "cmos_defaults_loaded") < 0) {
		printk(BIOS_INFO, "failed to get cmos_defaults_loaded");
		defaults_loaded = 0;
	}
	if (!defaults_loaded) {
		printk(BIOS_INFO, "Restoring CMOS defaults\n");
		/* set baudrate to 115200 baud */
		set_option("baud_rate", &(u8[]){ 0x00 });
		/* set default debug_level (DEFAULT_CONSOLE_LOGLEVEL starts at 1) */
		set_option("debug_level",
				&(u8[]) { CONFIG_DEFAULT_CONSOLE_LOGLEVEL+1 });
		set_option("cmos_defaults_loaded", &(u8[]){ 0x01 });
	}

	printk(BIOS_DEBUG, "ALIX1.C EXIT %s\n", __func__);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

