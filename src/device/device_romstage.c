/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003-2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2003 Greg Watson <jarrah@users.sourceforge.net>
 * Copyright (C) 2004 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005-2006 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
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
#include <device/path.h>
#include <device/pci.h>
#include <device/resource.h>

/** Linked list of ALL devices */
ROMSTAGE_CONST struct device * ROMSTAGE_CONST all_devices = &dev_root;

/**
 * Given a PCI bus and a devfn number, find the device structure.
 *
 * @param bus The bus number.
 * @param devfn A device/function number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
ROMSTAGE_CONST struct device *dev_find_slot(unsigned int bus,
						unsigned int devfn)
{
	ROMSTAGE_CONST struct device *dev, *result;

	result = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_PCI) &&
		    (dev->bus->secondary == bus) &&
		    (dev->path.pci.devfn == devfn)) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Given an SMBus bus and a device number, find the device structure.
 *
 * @param bus The bus number.
 * @param addr A device number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
ROMSTAGE_CONST struct device *dev_find_slot_on_smbus(unsigned int bus,
							unsigned int addr)
{
	ROMSTAGE_CONST struct device *dev, *result;

	result = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_I2C) &&
		    (dev->bus->secondary == bus) &&
		    (dev->path.i2c.device == addr)) {
			result = dev;
			break;
		}
	}
	return result;
}

