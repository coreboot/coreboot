/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/gpio.h>
#include <stddef.h>

const struct gpio_operations *dev_get_gpio_ops(struct device *dev)
{
	if (!dev) {
		printk(BIOS_ERR, "Could not get gpio operations, device is NULL.");
		return NULL;
	} else if (!dev->ops) {
		printk(BIOS_ERR, "Could not get gpio operations, dev->ops is NULL.");
		return NULL;
	} else if (!dev->ops->ops_gpio) {
		printk(BIOS_ERR, "Could not get gpio operations, ops_gpio is NULL.");
		return NULL;
	}

	return dev->ops->ops_gpio;
}
