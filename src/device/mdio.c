/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mdio.h>
#include <stddef.h>

const struct mdio_bus_operations *dev_get_mdio_ops(struct device *dev)
{
	if (!dev || !dev->ops || !dev->ops->ops_mdio) {
		printk(BIOS_ERR, "Could not get MDIO operations.\n");
		return NULL;
	}

	return dev->ops->ops_mdio;
}

uint16_t mdio_read(struct device *dev, uint8_t offset)
{
	const struct mdio_bus_operations *mdio_ops;
	struct device *parent = dev->bus->dev;

	assert(dev->path.type == DEVICE_PATH_MDIO);
	mdio_ops = dev_get_mdio_ops(parent);
	if (!mdio_ops)
		return 0;
	return mdio_ops->read(parent, dev->path.mdio.addr, offset);
}
void mdio_write(struct device *dev, uint8_t offset, uint16_t val)
{
	const struct mdio_bus_operations *mdio_ops;
	struct device *parent = dev->bus->dev;

	assert(dev->path.type == DEVICE_PATH_MDIO);
	mdio_ops = dev_get_mdio_ops(parent);
	if (!mdio_ops)
		return;
	mdio_ops->write(parent, dev->path.mdio.addr, offset, val);
}
