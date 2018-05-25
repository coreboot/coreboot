/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#include <compiler.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/i2c.h>
#include <soc/ramstage.h>
#include <soc/reg_access.h>

__weak void mainboard_gpio_i2c_init(struct device *dev)
{
	/* Initialize any of the GPIOs or I2C devices */
	printk(BIOS_SPEW, "WEAK; mainboard_gpio_i2c_init\n");
}

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= mainboard_gpio_i2c_init,
};

static const struct pci_driver gfx_driver __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= I2CGPIO_DEVID,
};
