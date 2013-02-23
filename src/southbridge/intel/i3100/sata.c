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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/* This code is based on src/southbridge/intel/esb6300/esb6300_sata.c */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i3100.h"

typedef struct southbridge_intel_i3100_config config_t;

static void sata_init(struct device *dev)
{
	u8 ahci;
	u32 *ahci_bar;
	config_t *config = dev->chip_info;

	if (config == NULL) {
	   printk(BIOS_ERR, "i3100_sata: error: device not in devicetree.cb!\n");
	   return;
	}

	/* Get the chip configuration */
	ahci = (pci_read_config8(dev, SATA_MAP) >> 6) & 0x03;

	/* Enable SATA devices */
	printk(BIOS_INFO, "SATA init (%s mode)\n", ahci ? "AHCI" : "Legacy");

	if(ahci) {
	  /* AHCI mode */
	  pci_write_config8(dev, SATA_MAP, (1 << 6) | (0 << 0));

	  /* Enable ports */
	  pci_write_config8(dev, SATA_PCS, 0x03);
	  pci_write_config8(dev, SATA_PCS + 1, 0x0F);

	  /* Setup timings */
	  pci_write_config16(dev, SATA_PTIM, 0x8000);
	  pci_write_config16(dev, SATA_STIM, 0x8000);

	  /* Synchronous DMA */
	  pci_write_config8(dev, SATA_SYNCC, 0);
	  pci_write_config16(dev, SATA_SYNCTIM, 0);

	  /* IDE I/O configuration */
	  pci_write_config32(dev, SATA_IIOC, 0);

	  ahci_bar = (u32 *)(pci_read_config32(dev, 0x27) & ~0x3ff);
	  ahci_bar[3] = config->sata_ports_implemented;
	} else {
	  /* SATA configuration */
	  pci_write_config8(dev, SATA_CMD, 0x07);
	  pci_write_config8(dev, SATA_PI, 0x8f);

	  /* Set timings */
	  pci_write_config16(dev, SATA_PTIM, 0x0a307);
	  pci_write_config16(dev, SATA_STIM, 0x0a307);

	  /* Sync DMA */
	  pci_write_config8(dev, SATA_SYNCC, 0x0f);
	  pci_write_config16(dev, SATA_SYNCTIM, 0x1111);

	  /* Fast ATA */
	  pci_write_config16(dev, SATA_IIOC, 0x1000);

	  /* Select IDE mode */
	  pci_write_config8(dev, SATA_MAP, 0x00);

	  /* Enable ports 0-3 */
	  pci_write_config8(dev, SATA_PCS + 1, 0x0f);

	}

	/* secret init sequence, required */
	pci_write_config32(dev, 0x94, 0x00400180);
	pci_write_config32(dev, 0xa0, 0x18);
	pci_write_config32(dev, 0xa4, 0x224);
	pci_write_config32(dev, 0xa0, 0x42);
	pci_write_config32(dev, 0xa4, 0x22006d);
	pci_write_config32(dev, 0xa0, 0x84);
	pci_write_config32(dev, 0xa4, 0x24);
	pci_write_config32(dev, 0xa0, 0x7a);
	pci_write_config32(dev, 0xa4, 0x220000);
	pci_write_config32(dev, 0xa0, 0x9c);
	pci_write_config32(dev, 0xa4, 0x24);
	pci_write_config32(dev, 0xa0, 0x90);
	pci_write_config32(dev, 0xa4, 0x220000);
	pci_write_config32(dev, 0xa0, 0xa0);
	pci_write_config32(dev, 0xa4, 0x12492aa);

	printk(BIOS_DEBUG, "SATA Enabled\n");
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
	.enable           = i3100_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_IDE,
};

static const struct pci_driver sata_driver __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_AHCI,
};

static const struct pci_driver ide_driver_ep80579 __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_EP80579_IDE,
};

static const struct pci_driver sata_driver_ep80579 __pci_driver = {
	.ops    = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_EP80579_AHCI,
};
