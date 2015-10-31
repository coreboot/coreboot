/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/intel/gma/i915.h>
#include "chip.h"

static void gma_func0_init(struct device *dev)
{
	u32 reg32;

	/* IGD needs to be bus master. */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);

	pci_dev_init(dev);
}

static void gma_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
			((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

const struct i915_gpu_controller_info *
intel_gma_get_controller_info(void)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x2,0));
	if (!dev) {
		return NULL;
	}
	struct northbridge_intel_sch_config *chip = dev->chip_info;
	return &chip->gfx;
}

static void gma_ssdt(device_t device)
{
	const struct i915_gpu_controller_info *gfx = intel_gma_get_controller_info();
	if (!gfx) {
		return;
	}

	drivers_intel_gma_displays_ssdt_generate(gfx);
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt_generator = gma_ssdt,
	.init			= gma_func0_init,
	.scan_bus		= 0,
	.enable			= 0,
	.ops_pci		= &gma_pci_ops,
};

static const struct pci_driver sch_gma_func0_driver __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x8108,
};
