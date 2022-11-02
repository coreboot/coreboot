/* SPDX-License-Identifier: GPL-2.0-only */

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
