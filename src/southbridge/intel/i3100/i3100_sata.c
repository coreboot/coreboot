/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

/* This code is based on src/southbridge/intel/esb6300/esb6300_sata.c */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i3100.h"

static void sata_init(struct device *dev)
{
	/* Enable SATA devices */

	printk_debug("SATA init\n");
	/* SATA configuration */
	pci_write_config8(dev, 0x04, 0x07);
	pci_write_config8(dev, 0x09, 0x8f);

	/* Set timings */
	pci_write_config16(dev, 0x40, 0x0a307);
	pci_write_config16(dev, 0x42, 0x0a307);

	/* Sync DMA */
	pci_write_config16(dev, 0x48, 0x000f);
	pci_write_config16(dev, 0x4a, 0x1111);

	/* Fast ATA */
	pci_write_config16(dev, 0x54, 0x1000);

	/* Select IDE mode */
	pci_write_config8(dev, 0x90, 0x00);

	/* Enable ports 0-3 */
	pci_write_config8(dev, 0x92, 0x0f);

	printk_debug("SATA Enabled\n");
}

static void sata_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = sata_set_subsystem,
};

static struct device_operations sata_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = sata_init,
	.scan_bus         = 0,
	.ops_pci          = &lops_pci,
};

static struct pci_driver sata_driver __pci_driver = {
        .ops    = &sata_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_3100_SATA,
};

static struct pci_driver sata_driver_nr __pci_driver = {
        .ops    = &sata_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_3100_SATA_R,
};

