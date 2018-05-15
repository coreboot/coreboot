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
 */

#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/resource.h>

/** Linked list of ALL devices */
DEVTREE_CONST struct device * DEVTREE_CONST all_devices = &dev_root;

/**
 * Given a PCI bus and a devfn number, find the device structure.
 *
 * @param bus The bus number.
 * @param devfn A device/function number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_slot(unsigned int bus,
						unsigned int devfn)
{
	DEVTREE_CONST struct device *dev, *result;

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
 * Given a Device Path Type, find the device structure.
 *
 * @param prev_match The previously matched device instance.
 * @param path_type The Device Path Type.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_path(
		DEVTREE_CONST struct device *prev_match,
		enum device_path_type path_type)
{
	DEVTREE_CONST struct device *dev, *result = NULL;

	if (prev_match == NULL)
		prev_match = all_devices;
	else
		prev_match = prev_match->next;

	for (dev = prev_match; dev; dev = dev->next) {
		if (dev->path.type == path_type) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Given a device pointer, find the next PCI device.
 *
 * @param previous_dev A pointer to a PCI device structure.
 * @return Pointer to the next device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_next_pci_device(
		DEVTREE_CONST struct device *previous_dev)
{
	return dev_find_path(previous_dev, DEVICE_PATH_PCI);
}

/**
 * Given an SMBus bus and a device number, find the device structure.
 *
 * @param bus The bus number.
 * @param addr A device number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_slot_on_smbus(unsigned int bus,
							unsigned int addr)
{
	DEVTREE_CONST struct device *dev, *result;

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

/**
 * Given a PnP port and a device number, find the device structure.
 *
 * @param port The I/O port.
 * @param device Logical device number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_slot_pnp(u16 port, u16 device)
{
	DEVTREE_CONST struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_PNP) &&
		    (dev->path.pnp.port == port) &&
		    (dev->path.pnp.device == device)) {
			return dev;
		}
	}
	return 0;
}
