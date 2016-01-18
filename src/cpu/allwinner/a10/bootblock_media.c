/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * CBFS accessors for bootblock stage.
 *
 */
#include <boot_device.h>
#include <console/console.h>

const struct region_device *boot_device_ro(void)
{
	printk(BIOS_ERR, "Oh my! I don't know how to access CBFS yet.");
	return NULL;
}
