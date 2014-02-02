/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Vladimir Serbinenko
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

#include <types.h>
#include <string.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/smbus.h>
#include <smbios.h>
#include <console/console.h>

static void at24rf08c_init(device_t dev)
{
	int i, j;

	if (!dev->enabled)
		return;

	/* Ensure that EEPROM/RFID chip is not accessible through RFID.
	   Need to do it only on 5c.  */
	if (dev->path.type != DEVICE_PATH_I2C || dev->path.i2c.device != 0x5c)
		return;

	printk(BIOS_DEBUG, "Locking EEPROM RFID\n");

	for (i = 0; i < 8; i++) {
		/* After a register write AT24RF08C sometimes stops responding.
		   Retry several times in case of failure.
		 */
		for (j = 0; j < 100; j++)
			if (smbus_write_byte(dev, i, 0x0f) >= 0)
				break;
	}

	printk(BIOS_DEBUG, "init EEPROM done\n");
}

static void at24rf08c_noop(device_t dummy)
{
}

static struct device_operations at24rf08c_operations = {
	.read_resources = at24rf08c_noop,
	.set_resources = at24rf08c_noop,
	.enable_resources = at24rf08c_noop,
	.init = at24rf08c_init,
};

static void enable_dev(device_t dev)
{
	dev->ops = &at24rf08c_operations;
}

struct chip_operations drivers_i2c_at24rf08c_ops = {
	CHIP_NAME("AT24RF08C")
	    .enable_dev = enable_dev,
};
