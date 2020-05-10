/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <device/device.h>
#include <device/smbus.h>
#include <console/console.h>

static void at24rf08c_init(struct device *dev)
{
	int i, j;

	if (!dev->enabled)
		return;

	/* Ensure that EEPROM/RFID chip is not accessible through RFID.
	   Need to do it only on 5c. */
	if (dev->path.type != DEVICE_PATH_I2C || dev->path.i2c.device != 0x5c)
		return;

	printk(BIOS_DEBUG, "Locking EEPROM RFID\n");

	for (i = 0; i < 8; i++) {
		/* After a register write AT24RF08C sometimes stops responding.
		   Retry several times in case of failure. */
		for (j = 0; j < 100; j++)
			if (smbus_write_byte(dev, i, 0x0f) >= 0)
				break;
	}

	printk(BIOS_DEBUG, "init EEPROM done\n");
}

static struct device_operations at24rf08c_operations = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = at24rf08c_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &at24rf08c_operations;
}

struct chip_operations drivers_i2c_at24rf08c_ops = {
	CHIP_NAME("AT24RF08C")
	.enable_dev = enable_dev,
};
