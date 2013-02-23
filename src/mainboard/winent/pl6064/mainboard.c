/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Win Enterprises, Inc. (anishp@win-ent.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the license.
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

static void init(struct device *dev)
{
	printk(BIOS_DEBUG, "Win Enterprises PL-6064/65 ENTER %s\n", __func__);
	printk(BIOS_DEBUG, "Win Enterprises PL-6064/65 EXIT %s\n", __func__);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
