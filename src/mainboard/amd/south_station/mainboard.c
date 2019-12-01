/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>

/**
 * Southstation using SB GPIO 17/18 to control the Red/Green LED
 * These two LEDs can be used to show the OS booting status.
 */
static void southstation_led_init(void)
{
	/* multi-function pins switch to GPIO0-35 */
	pm_write8(0xea, (pm_read8(0xea) & 0xfe) | 1);

	/* select IOMux to function2, corresponds to GPIO */
	iomux_write8(0x11, (iomux_read8(0x11) & 0xfc) | 2);
	iomux_write8(0x12, (iomux_read8(0x12) & 0xfc) | 2);

	/* Lighting test */
	gpio_100_write8(0x11, 0x08); //output high
	gpio_100_write8(0x12, 0x08);
	mdelay(100);
	gpio_100_write8(0x11, 0x48); //output low
	gpio_100_write8(0x12, 0x48);
}

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
	southstation_led_init();

	/*
	 * Initialize ASF registers to an arbitrary address because someone
	 * long ago set things up this way inside the SPD read code.  The
	 * SPD read code has been made generic and moved out of the board
	 * directory, so the ASF init is being done here.
	 */
	pm_write8(0x29, 0x80);
	pm_write8(0x28, 0x61);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
