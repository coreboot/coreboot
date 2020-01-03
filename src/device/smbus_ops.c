/*
 * This file is part of the coreboot project.
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

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>

struct bus *get_pbus_smbus(struct device *dev)
{
	struct bus *const pbus = i2c_link(dev);
	if (!pbus->dev->ops->ops_smbus_bus) {
		printk(BIOS_ALERT, "%s Cannot find SMBus bus operations",
		       dev_path(dev));
		die("");
	}
	return pbus;
}

#define CHECK_PRESENCE(x)				       \
	if (!ops_smbus_bus(get_pbus_smbus(dev))->x) {	       \
		printk(BIOS_ERR, "%s missing " #x "\n",	       \
		       dev_path(dev));			       \
		return -1;				       \
	}

int smbus_block_read(struct device *dev, u8 cmd, u8 bytes, u8 *buffer)
{
	CHECK_PRESENCE(block_read);

	return ops_smbus_bus(get_pbus_smbus(dev))->block_read(dev, cmd,
							      bytes, buffer);
}

int smbus_block_write(struct device *dev, u8 cmd, u8 bytes, const u8 *buffer)
{
	CHECK_PRESENCE(block_write);

	return ops_smbus_bus(get_pbus_smbus(dev))->block_write(dev, cmd,
							       bytes, buffer);
}
