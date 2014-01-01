/*
 * This file is part of uio_usbdebug
 *
 * Copyright (C) 2013 Nico Huber <nico.h@gmx.de>
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

#include <device/device.h>

static char g_path[] = { '\0' };

const char *dev_path(device_t dev)
{
	return g_path;
}

struct resource *find_resource(device_t dev, unsigned index)
{
	return NULL;
}

void report_resource_stored(device_t dev, struct resource *resource,
			    const char *comment)
{
}

struct device *dev_find_slot(unsigned int bus, unsigned int devfn)
{
	return NULL;
}
