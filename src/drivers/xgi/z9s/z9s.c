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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include "../common/xgi_coreboot.h"
#include "../common/XGIfb.h"

static void xgi_z9s_set_resources(struct device *dev)
{
	/* Reserve VGA regions */
	mmio_resource(dev, 3, 0xa0000 >> 10, 0x1ffff >> 10);

	/* Run standard resource set routine */
	pci_dev_set_resources(dev);
}

static void xgi_z9s_init(struct device *dev)
{
	u8 ret;
	struct xgifb_video_info *xgifb_info;

	if (IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT)) {
		printk(BIOS_INFO, "XGI Z9s: initializing video device\n");
		xgifb_info = malloc(sizeof(*xgifb_info));
		ret = xgifb_probe(dev, xgifb_info);
		if (!ret)
			xgifb_modeset(dev, xgifb_info);
		free(xgifb_info);
	}
}

static struct device_operations xgi_z9s_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = xgi_z9s_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = xgi_z9s_init,
	.scan_bus         = 0,
};

static const struct pci_driver xgi_z9s_driver __pci_driver = {
	.ops    = &xgi_z9s_ops,
	.vendor = PCI_VENDOR_ID_XGI,
	.device = PCI_DEVICE_ID_XGI_20,
};
