/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/smbus.h>
#include <device/pci.h>
#include "chip.h"
#include "tas5825m.h"

int tas5825m_write_at(struct device *dev, uint8_t addr, uint8_t value)
{
	return smbus_write_byte(dev, addr, value);
}

//TODO: use I2C block write for better performance
int tas5825m_write_block_at(struct device *dev, uint8_t addr,
	const uint8_t *values, uint8_t length)
{
	int res = 0;
	for (uint8_t i = 0; i < length; i++) {
		res = smbus_write_byte(dev, addr + i, values[i]);
		if (res < 0)
			return res;
	}
	return (int)length;
}

int tas5825m_set_page(struct device *dev, uint8_t page)
{
	return tas5825m_write_at(dev, 0x00, page);
}

int tas5825m_set_book(struct device *dev, uint8_t book)
{
	int res = tas5825m_set_page(dev, 0x00);
	if (res < 0)
		return res;
	return tas5825m_write_at(dev, 0x7F, book);
}

__weak int tas5825m_setup(struct device *dev, int id)
{
	printk(BIOS_ERR, "tas5825m: setup not implemented\n");
	return -1;
}

static void tas5825m_init(struct device *dev)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C &&
		ops_smbus_bus(get_pbus_smbus(dev))) {
		printk(BIOS_DEBUG, "tas5825m at %s\n", dev_path(dev));

		struct drivers_i2c_tas5825m_config *config = dev->chip_info;
		if (config) {
			printk(BIOS_DEBUG, "tas5825m id %d\n", config->id);
			int res = tas5825m_setup(dev, config->id);
			if (res)
				printk(BIOS_ERR, "tas5825m init failed: %d\n", res);
			else
				printk(BIOS_DEBUG, "tas5825m init successful\n");
		} else {
			printk(BIOS_ERR, "tas5825m: failed to find config\n");
		}
	}
}

static struct device_operations tas5825m_operations = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.init			= tas5825m_init,
};

static void tas5825m_enable_dev(struct device *dev)
{
	dev->ops = &tas5825m_operations;
}

struct chip_operations drivers_i2c_tas5825m_ops = {
	CHIP_NAME("TI TAS5825M Amplifier")
	.enable_dev = tas5825m_enable_dev,
};
