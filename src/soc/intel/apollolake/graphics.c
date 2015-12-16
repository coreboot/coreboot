/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <fsp/util.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_ids.h>

static uintptr_t framebuffer_bar = (uintptr_t)NULL;

void lb_framebuffer(struct lb_header *header)
{
	enum cb_err ret;
	struct lb_framebuffer *framebuffer;

	framebuffer = (void *)lb_new_record(header);
	ret = fsp_fill_lb_framebuffer(framebuffer);
	if (ret != CB_SUCCESS) {
		printk(BIOS_ALERT, "FSP did not return a valid framebuffer\n");
		return;
        }

	if (!framebuffer_bar) {
		printk(BIOS_ALERT, "Framebuffer BAR invalid (00:02.0 BAR2)\n");
		return;
	}

	/* Resource allocator can move the BAR around after FSP configures it */
	framebuffer->physical_address = framebuffer_bar;
	printk(BIOS_DEBUG, "Graphics framebuffer located at 0x%llx\n",
	       framebuffer->physical_address);
}

static void igd_set_resources(struct device *dev)
{
	framebuffer_bar = find_resource(dev, PCI_BASE_ADDRESS_2)->base;
	pci_dev_set_resources(dev);
}

static const struct device_operations igd_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = igd_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci_dev_init,
	.enable           = DEVICE_NOOP
};

static const struct pci_driver integrated_graphics_driver __pci_driver = {
	.ops	= &igd_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_APOLLOLAKE_IGD,
};
