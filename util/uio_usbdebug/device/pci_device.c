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

#include <device/pci.h>
#include <device/device.h>

unsigned int pci_match_simple_dev(device_t dev, pci_devfn_t sdev)
{
	return 0;
}

void pci_dev_read_resources(struct device *dev)
{
}
