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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <reset.h>

/**
 * Read the resources for the root device,
 * that encompass the resources for the entire system.
 * @param root Pointer to the device structure for the system root device
 */
void root_dev_read_resources(device_t root)
{
	printk(BIOS_ERR, "%s should never be called.\n", __func__);
}

/**
 * @brief Write the resources for every device
 *
 * Write the resources for the root device,
 * and every device under it which are all of the devices.
 * @param root Pointer to the device structure for the system root device
 */
void root_dev_set_resources(device_t root)
{
	printk(BIOS_ERR, "%s should never be called.\n", __func__);
}

/**
 * @brief Scan devices on static buses.
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
 * @param bus Pointer to the device structure which the static buses are attached
 * @param max  Maximum bus number currently used before scanning.
 * @return Largest bus number used.
 */
static int smbus_max = 0;
unsigned int scan_static_bus(device_t bus, unsigned int max)
{
	device_t child;
	unsigned link;

	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(bus));

	for(link = 0; link < bus->links; link++) {
		/* for smbus bus enumerate */
		child = bus->link[link].children;
		if(child && child->path.type == DEVICE_PATH_I2C) {
			bus->link[link].secondary = ++smbus_max;
		}
		for(child = bus->link[link].children; child; child = child->sibling) {
			if (child->chip_ops && child->chip_ops->enable_dev) {
				child->chip_ops->enable_dev(child);
			}
			if (child->ops && child->ops->enable) {
				child->ops->enable(child);
			}
 			if (child->path.type == DEVICE_PATH_I2C) {
 				printk(BIOS_DEBUG, "smbus: %s[%d]->",
					dev_path(child->bus->dev), child->bus->link );
			}
			printk(BIOS_DEBUG, "%s %s\n",
				dev_path(child),
				child->enabled?"enabled": "disabled");
		}
	}
	for(link = 0; link < bus->links; link++) {
		for(child = bus->link[link].children; child; child = child->sibling) {
			if (!child->ops || !child->ops->scan_bus)
				continue;
			printk(BIOS_SPEW, "%s scanning...\n", dev_path(child));
			max = scan_bus(child, max);
		}
	}

	printk(BIOS_SPEW, "%s for %s done\n", __func__, dev_path(bus));

	return max;
}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whos children's resources are to be enabled
 *
 * This function is called by the global enable_resource() indirectly via the
 * device_operation::enable_resources() method of devices.
 *
 * Indirect mutual recursion:
 *	enable_childrens_resources() -> enable_resources()
 *	enable_resources() -> device_operation::enable_resources()
 *	device_operation::enable_resources() -> enable_children_resources()
 */
void enable_childrens_resources(device_t dev)
{
	unsigned link;
	for(link = 0; link < dev->links; link++) {
		device_t child;
		for(child = dev->link[link].children; child; child = child->sibling) {
			enable_resources(child);
		}
	}
}

void root_dev_enable_resources(device_t dev)
{
	enable_childrens_resources(dev);
}

/**
 * @brief Scan root bus for generic systems
 *
 * @param root The root device structure
 * @param max The current bus number scanned so far, usually 0x00
 *
 * This function is the default scan_bus() method of the root device.
 */
unsigned int root_dev_scan_bus(device_t root, unsigned int max)
{
	return scan_static_bus(root, max);
}

void root_dev_init(device_t root)
{
}

void root_dev_reset(struct bus *bus)
{
	printk(BIOS_INFO, "Reseting board...\n");
	hard_reset();
}

/**
 * @brief Default device operation for root device
 *
 * This is the default device operation for root devices. These operations
 * should be fully usable as is.  However the chip_operations::enable_dev()
 * of a motherboard can override this if you want non-default behavior.
 */
struct device_operations default_dev_ops_root = {
	.read_resources   = root_dev_read_resources,
	.set_resources    = root_dev_set_resources,
	.enable_resources = root_dev_enable_resources,
	.init             = root_dev_init,
	.scan_bus         = root_dev_scan_bus,
	.reset_bus        = root_dev_reset,
};
