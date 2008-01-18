/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <string.h>
#include <console/console.h>
#include <stdlib.h>
#include "../flash.h"

static flash_device *first_flash = 0;

int register_flash_device (const flash_fn * fn, char *tag, void *data)
{
    flash_device *device = malloc (sizeof (flash_device));

    if (device)
    {
	const char *result;
	device->fn = fn;
	device->tag = tag;
	device->data = data;
	if ((result = fn->identify(device)) != 0)
	{
	    printk_info("Registered flash %s\n", result);
	    device->next = first_flash;
	    first_flash = device;
	}
	return result ? 0 : -1;
    }
    return -1;
}

flash_device *find_flash_device(const char *name)
{
    int len = strlen(name);

    if (first_flash)
    {
	flash_device *flash;

	for (flash = first_flash; flash; flash = flash->next)
	    if (strlen(flash->tag) == len && memcmp(name, flash->tag, len) == 0)
		return flash;
    }
    printk_info ("No flash %s registered\n", name);
    return 0;
}
