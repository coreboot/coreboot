/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2004 Li-Ta Lo <ollie@lanl.gov>
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

/*
 * Multi-level I2C MUX? May need to find the first I2C device and then set link
 * down to current dev.
 *
 * 1 store get_pbus_smbus list link
 * 2 reverse the link and call set link.
 *
 * @param dev TODO.
 */
int smbus_set_link(struct device *dev)
{
	struct bus *pbus_a[4]; // 4 level mux only. Enough?
	struct bus *pbus = dev->bus;
	int pbus_num = 0;
	int i;

	while (pbus && pbus->dev && (pbus->dev->path.type == DEVICE_PATH_I2C)) {
		pbus_a[pbus_num++] = pbus;
		pbus = pbus->dev->bus;
	}

	// printk(BIOS_INFO, "smbus_set_link: ");
	for (i = pbus_num - 1; i >= 0; i--) {
		// printk(BIOS_INFO, " %s[%d] -> ", dev_path(pbus_a[i]->dev),
		// pbus_a[i]->link);
		if (ops_smbus_bus(get_pbus_smbus(pbus_a[i]->dev))) {
			if (pbus_a[i]->dev->ops
			    && pbus_a[i]->dev->ops->set_link)
				pbus_a[i]->dev->ops->set_link(pbus_a[i]->dev,
							pbus_a[i]->link_num);
		}
	}
	// printk(BIOS_INFO, " %s\n", dev_path(dev));

	return pbus_num;
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
