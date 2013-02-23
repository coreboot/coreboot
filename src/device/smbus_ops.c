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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <stdint.h>
#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>

struct bus *get_pbus_smbus(device_t dev)
{
	struct bus *pbus = dev->bus;

	while (pbus && pbus->dev && !ops_smbus_bus(pbus))
		pbus = pbus->dev->bus;

	if (!pbus || !pbus->dev || !pbus->dev->ops
	    || !pbus->dev->ops->ops_smbus_bus) {
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
int smbus_set_link(device_t dev)
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

int smbus_quick_read(device_t dev)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->quick_read(dev);
}

int smbus_quick_write(device_t dev)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->quick_write(dev);
}

int smbus_recv_byte(device_t dev)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->recv_byte(dev);
}

int smbus_send_byte(device_t dev, u8 byte)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->send_byte(dev, byte);
}

int smbus_read_byte(device_t dev, u8 addr)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->read_byte(dev, addr);
}

int smbus_write_byte(device_t dev, u8 addr, u8 val)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->write_byte(dev, addr, val);
}

int smbus_read_word(device_t dev, u8 addr)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->read_word(dev, addr);
}

int smbus_write_word(device_t dev, u8 addr, u16 val)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->write_word(dev, addr, val);
}

int smbus_process_call(device_t dev, u8 cmd, u16 data)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->process_call(dev, cmd, data);
}

int smbus_block_read(device_t dev, u8 cmd, u8 bytes, u8 *buffer)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->block_read(dev, cmd,
							      bytes, buffer);
}

int smbus_block_write(device_t dev, u8 cmd, u8 bytes, const u8 *buffer)
{
	return ops_smbus_bus(get_pbus_smbus(dev))->block_write(dev, cmd,
							       bytes, buffer);
}
