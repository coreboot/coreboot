/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003-2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2003 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2004-2005 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005 Tyan
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <reset.h>

const char mainboard_name[] = CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER;

/**
 * Scan devices on static buses.
 *
 * The enumeration of certain buses is purely static. The existence of
 * devices on those buses can be completely determined at compile time
 * and is specified in the config file. Typical examples are the 'PNP'
 * devices on a legacy ISA/LPC bus. There is no need of probing of any kind,
 * the only thing we have to do is to walk through the bus and
 * enable or disable devices as indicated in the config file.
 *
 * On the other hand, some devices are virtual and their existence is
 * artificial. They can not be probed at run time. One example is the
 * debug device. Those virtual devices have to be listed in the config
 * file under some static bus in order to be enumerated at run time.
 *
 * @param bus Pointer to the device to which the static buses are attached to.
 */

void scan_static_bus(struct device *bus)
{
	struct device *child;
	struct bus *link;

	for (link = bus->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling) {

			if (child->chip_ops && child->chip_ops->enable_dev)
				child->chip_ops->enable_dev(child);

			if (child->ops && child->ops->enable)
				child->ops->enable(child);

			printk(BIOS_DEBUG, "%s %s\n", dev_path(child),
			       child->enabled ? "enabled" : "disabled");
		}
	}
}

void scan_lpc_bus(struct device *bus)
{
	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(bus));

	scan_static_bus(bus);

	printk(BIOS_SPEW, "%s for %s done\n", __func__, dev_path(bus));
}

void scan_usb_bus(struct device *bus)
{
	struct bus *link;

	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(bus));

	scan_static_bus(bus);

	/* Scan bridges in case this device is a hub */
	for (link = bus->link_list; link; link = link->next)
		scan_bridges(link);

	printk(BIOS_SPEW, "%s for %s done\n", __func__, dev_path(bus));
}

void scan_generic_bus(struct device *bus)
{
	struct device *child;
	struct bus *link;
	static int bus_max = 0;

	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(bus));

	for (link = bus->link_list; link; link = link->next) {

		link->secondary = ++bus_max;

		for (child = link->children; child; child = child->sibling) {

			if (child->chip_ops && child->chip_ops->enable_dev)
				child->chip_ops->enable_dev(child);

			if (child->ops && child->ops->enable)
				child->ops->enable(child);

			printk(BIOS_DEBUG, "bus: %s[%d]->", dev_path(child->bus->dev),
			       child->bus->link_num);

			printk(BIOS_DEBUG, "%s %s\n", dev_path(child),
			       child->enabled ? "enabled" : "disabled");
		}
	}

	printk(BIOS_SPEW, "%s for %s done\n", __func__, dev_path(bus));
}

void scan_smbus(struct device *bus)
{
	scan_generic_bus(bus);
}

/**
 * Scan root bus for generic systems.
 *
 * This function is the default scan_bus() method of the root device.
 *
 * @param root The root device structure.
 */
static void root_dev_scan_bus(struct device *bus)
{
	struct bus *link;

	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(bus));

	scan_static_bus(bus);

	for (link = bus->link_list; link; link = link->next)
		scan_bridges(link);

	printk(BIOS_SPEW, "%s for %s done\n", __func__, dev_path(bus));
}

static void root_dev_reset(struct bus *bus)
{
	printk(BIOS_INFO, "Resetting board...\n");
	hard_reset();
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static const char *root_dev_acpi_name(const struct device *dev)
{
	return "\\_SB";
}
#endif

/**
 * Default device operation for root device.
 *
 * This is the default device operation for root devices. These operations
 * should be fully usable as is. However the chip_operations::enable_dev()
 * of a motherboard can override this if you want non-default behavior.
 */
struct device_operations default_dev_ops_root = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = DEVICE_NOOP,
	.scan_bus         = root_dev_scan_bus,
	.reset_bus        = root_dev_reset,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name        = root_dev_acpi_name,
#endif
};
