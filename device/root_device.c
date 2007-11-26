/*
 * This file is part of the LinuxBIOS project.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console.h>
#include <device/device.h>
#include <device/pci.h>
// #include <part/hard_reset.h>

/** 
 * Read the resources for the root device,
 * that encompass the resources for the entire system.
 *
 * @param root Pointer to the device structure for the system root device.
 */
void root_dev_read_resources(struct device *root)
{
	struct resource *resource;

	/* Initialize the system wide I/O space constraints. */
	resource = new_resource(root, 0);
	resource->base = 0x400;
	resource->size = 0;
	resource->align = 0;
	resource->gran = 0;
	resource->limit = 0xffffUL;
	resource->flags = IORESOURCE_IO;
	compute_allocate_resource(&root->link[0], resource,
				  IORESOURCE_IO, IORESOURCE_IO);

	/* Initialize the system wide memory resources constraints. */
	resource = new_resource(root, 1);
	resource->base = 0;
	resource->size = 0;
	resource->align = 0;
	resource->gran = 0;
	resource->limit = 0xffffffffUL;
	resource->flags = IORESOURCE_MEM;
	compute_allocate_resource(&root->link[0], resource,
				  IORESOURCE_MEM, IORESOURCE_MEM);
}

/**
 * Write the resources for the root device,
 * and every device under it which are all of the devices.
 *
 * @param root Pointer to the device structure for the system root device.
 */
void root_dev_set_resources(struct device *root)
{
	struct bus *bus;

	bus = &root->link[0];
	compute_allocate_resource(bus,
				  &root->resource[0], IORESOURCE_IO,
				  IORESOURCE_IO);
	compute_allocate_resource(bus, &root->resource[1], IORESOURCE_MEM,
				  IORESOURCE_MEM);
	phase4_assign_resources(bus);
}

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
 * This function is the default scan_bus() method for the root device and
 * LPC bridges.
 *
 * @param busdevice Pointer to the device structure which the static
 * 		    buses are attached.
 * @param max Maximum bus number currently used before scanning.
 * @return Largest bus number used.
 */
static int smbus_max = 0;
unsigned int scan_static_bus(struct device *busdevice, unsigned int max)
{
	struct device *child;
	unsigned int link;

	printk(BIOS_INFO, "%s for %s (%s)\n", __func__, busdevice->dtsname,
	       dev_path(busdevice));

	for (link = 0; link < busdevice->links; link++) {
		/* For smbus bus enumerate. */
		child = busdevice->link[link].children;
		if (child && child->path.type == DEVICE_PATH_I2C) {
			busdevice->link[link].secondary = ++smbus_max;
		}
		for (child = busdevice->link[link].children; child;
		     child = child->sibling) {
			if (child->ops && child->ops->phase3_enable_scan) {
				child->ops->phase3_enable_scan(child);
			}
			/* Sigh. Have to enable to scan... */
			if (child->ops && child->ops->phase5_enable_resources) {
				child->ops->phase5_enable_resources(child);
			}
			if (child->path.type == DEVICE_PATH_I2C) {
				printk(BIOS_DEBUG, "smbus: %s(%s)[%d]->",
				       child->dtsname,
				       dev_path(child->bus->dev),
				       child->bus->link);
			}
			printk(BIOS_DEBUG, "%s(%s) %s\n",
			       child->dtsname, dev_path(child),
			       child->enabled ? "enabled" : "disabled");
		}
	}
	for (link = 0; link < busdevice->links; link++) {
		for (child = busdevice->link[link].children; child;
		     child = child->sibling) {
			if (!child->ops || !child->ops->phase3_scan)
				continue;
			printk(BIOS_INFO, "%s(%s) scanning...\n",
			       child->dtsname, dev_path(child));
			max = dev_phase3_scan(child, max);
		}
	}

	printk(BIOS_INFO, "%s for %s(%s) done\n", __func__, busdevice->dtsname,
	       dev_path(busdevice));

	return max;
}

/**
 * Enable resources for children devices.
 *
 * This function is called by the global enable_resource() indirectly via the
 * device_operation::enable_resources() method of devices.
 *
 * Indirect mutual recursion:
 *	enable_childrens_resources() -> enable_resources()
 *	enable_resources() -> device_operation::enable_resources()
 *	device_operation::enable_resources() -> enable_children_resources()
 *
 * @param dev The device whose children's resources are to be enabled.
 */
void enable_childrens_resources(struct device *dev)
{
	unsigned int link;
	for (link = 0; link < dev->links; link++) {
		struct device *child;
		for (child = dev->link[link].children; child;
		     child = child->sibling) {
			dev_phase5(child);
		}
	}
}

void root_dev_enable_resources(struct device *dev)
{
	enable_childrens_resources(dev);
}

/**
 * Scan root bus for generic systems.
 *
 * This function is the default scan_bus() method of the root device.
 *
 * @param root The root device structure.
 * @param max The current bus number scanned so far, usually 0x00.
 * @return TODO
 */
unsigned int root_dev_scan_bus(struct device *root, unsigned int max)
{
	return scan_static_bus(root, max);
}

void root_dev_init(struct device *root)
{
}

void root_dev_reset(struct bus *bus)
{
	printk(BIOS_INFO, "Resetting board... NOT! Define hard_reset please\n");
	// hard_reset();
}

/**
 * Default device operation for root device.
 *
 * This is the default device operation for root devices. These operations
 * should be fully usable as is. If you need something else, set up your
 * own ops in (e.g.) the mainboard, and initialize it in the dts in the
 * mainboard directory. 
 */
const struct device_operations default_dev_ops_root = {
	.phase4_read_resources   = root_dev_read_resources,
	.phase4_set_resources    = root_dev_set_resources,
	.phase5_enable_resources = root_dev_enable_resources,
	.phase6_init             = root_dev_init,
	.phase3_scan             = root_dev_scan_bus,
	.reset_bus               = root_dev_reset,
};

/**
 * The root of device tree.
 *
 * This is the root of the device tree. The device tree is defined in the
 * static.c file and is generated by config tool during compile time.
 */
extern struct device dev_root;
