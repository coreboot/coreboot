/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DEVICE_MDIO_H__
#define __DEVICE_MDIO_H__

#include <device/device.h>
#include <types.h>

struct mdio_bus_operations {
	uint16_t (*read)(struct device *dev, uint8_t phy_adr, uint8_t reg_adr);
	void (*write)(struct device *dev, uint8_t phy_adr, uint8_t reg_adr, uint16_t data);
};

/* Helper for getting mdio operations from a device */
const struct mdio_bus_operations *dev_get_mdio_ops(struct device *dev);

#endif	/* __DEVICE_MDIO_H__ */
