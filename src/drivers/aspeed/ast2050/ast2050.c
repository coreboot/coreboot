/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>
#include <edid.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <pc80/vga.h>

#include "../common/aspeed_coreboot.h"
#include "../common/ast_drv.h"

static void aspeed_ast2050_set_resources(struct device *dev)
{
	/* Reserve VGA regions */
	mmio_resource(dev, 3, 0xa0000 >> 10, 0x1ffff >> 10);

	/* Run standard resource set routine */
	pci_dev_set_resources(dev);
}

static void aspeed_ast2050_init(struct device *dev)
{
	u8 ret;
	struct drm_device drm_dev;

	drm_dev.pdev = dev;

	printk(BIOS_INFO, "ASpeed AST2050: initializing video device\n");
	ret = ast_driver_load(&drm_dev, 0);

	/* Unlock extended configuration registers */
	outb(0x80, 0x3d4); outb(0xa8, 0x3d5);

	/* Set CRT Request Threshold */
	outb(0xa6, 0x3d4); outb(0x2f, 0x3d5);
	outb(0xa7, 0x3d4); outb(0x3f, 0x3d5);

	/* Initialize standard VGA text mode */
	vga_io_init();
	vga_textmode_init();
	printk(BIOS_INFO, "ASpeed VGA text mode initialized\n");

	/* if we don't have console, at least print something... */
	vga_line_write(0, "ASpeed VGA text mode initialized");
}

static struct device_operations aspeed_ast2050_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = aspeed_ast2050_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = aspeed_ast2050_init,
	.scan_bus         = 0,
};

static const struct pci_driver aspeed_ast2050_driver __pci_driver = {
	.ops    = &aspeed_ast2050_ops,
	.vendor = PCI_VENDOR_ID_ASPEED,
	.device = PCI_DEVICE_ID_ASPEED_AST2050_VGA,
};
