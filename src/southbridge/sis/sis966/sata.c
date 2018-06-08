/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
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
#include <device/device.h>
#include <delay.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "sis966.h"
#include <arch/io.h>

uint8_t SiS_SiS1183_init[68][3] = {
	{0x04, 0x00, 0x05},
	{0x09, 0x00, 0x05},
	{0x2C, 0x00, 0x39},
	{0x2D, 0x00, 0x10},
	{0x2E, 0x00, 0x83},
	{0x2F, 0x00, 0x11},
	{0x90, 0x00, 0x40},
	{0x91, 0x00, 0x00},	// set mode
	{0x50, 0x00, 0xA2},
	{0x52, 0x00, 0xA2},
	{0x55, 0x00, 0x96},
	{0x52, 0x00, 0xA2},
	{0x55, 0xF7, 0x00},
	{0x56, 0x00, 0xC0},
	{0x57, 0x00, 0x14},
	{0x67, 0x00, 0x28},
	{0x81, 0x00, 0xB3},
	{0x82, 0x00, 0x72},
	{0x83, 0x00, 0x40},
	{0x85, 0x00, 0xB3},
	{0x86, 0x00, 0x72},
	{0x87, 0x00, 0x40},
	{0x88, 0x00, 0xDE},	// after set mode
	{0x89, 0x00, 0xB3},
	{0x8A, 0x00, 0x72},
	{0x8B, 0x00, 0x40},
	{0x8C, 0x00, 0xDE},
	{0x8D, 0x00, 0xB3},
	{0x8E, 0x00, 0x92},
	{0x8F, 0x00, 0x40},
	{0x93, 0x00, 0x00},
	{0x94, 0x00, 0x80},
	{0x95, 0x00, 0x08},
	{0x96, 0x00, 0x80},
	{0x97, 0x00, 0x08},
	{0x9C, 0x00, 0x80},
	{0x9D, 0x00, 0x08},
	{0x9E, 0x00, 0x80},
	{0x9F, 0x00, 0x08},
	{0xA0, 0x00, 0x15},
	{0xA1, 0x00, 0x15},
	{0xA2, 0x00, 0x15},
	{0xA3, 0x00, 0x15},

	{0xD8, 0xFE, 0x01},	// Com reset
	{0xC8, 0xFE, 0x01},
	{0xE8, 0xFE, 0x01},
	{0xF8, 0xFE, 0x01},

	{0xD8, 0xFE, 0x00},	// Com reset
	{0xC8, 0xFE, 0x00},
	{0xE8, 0xFE, 0x00},
	{0xF8, 0xFE, 0x00},

	{0xC4, 0xFF, 0xFF},	// Clear status
	{0xC5, 0xFF, 0xFF},
	{0xC6, 0xFF, 0xFF},
	{0xC7, 0xFF, 0xFF},
	{0xD4, 0xFF, 0xFF},
	{0xD5, 0xFF, 0xFF},
	{0xD6, 0xFF, 0xFF},
	{0xD7, 0xFF, 0xFF},
	{0xE4, 0xFF, 0xFF},	// Clear status
	{0xE5, 0xFF, 0xFF},
	{0xE6, 0xFF, 0xFF},
	{0xE7, 0xFF, 0xFF},
	{0xF4, 0xFF, 0xFF},
	{0xF5, 0xFF, 0xFF},
	{0xF6, 0xFF, 0xFF},
	{0xF7, 0xFF, 0xFF},

	{0x00, 0x00, 0x00}	// End of table
};

static void sata_init(struct device *dev)
{
	struct southbridge_sis_sis966_config *conf;
	int i;
	uint32_t temp32;
	uint8_t temp8;

	conf = dev->chip_info;
	printk(BIOS_DEBUG, "SATA_INIT:---------->\n");

	/* Enable IDE (SiS1183) */
	i = 0;
	while (SiS_SiS1183_init[i][0] != 0) {
		temp8 = pci_read_config8(dev, SiS_SiS1183_init[i][0]);
		temp8 &= SiS_SiS1183_init[i][1];
		temp8 |= SiS_SiS1183_init[i][2];
		pci_write_config8(dev, SiS_SiS1183_init[i][0], temp8);
		i++;
	};

	for (i = 0; i < 10; i++) {
		temp32 = pci_read_config32(dev, 0xC0);
		printk(BIOS_DEBUG, "status= %x\n", temp32);
		if (((temp32 & 0xF) == 0x3) || ((temp32 & 0xF) == 0x0))
			break;
	}

#if DEBUG_SATA
	printk(BIOS_DEBUG, "****** SATA PCI config ******");
	printk(BIOS_DEBUG, "\n    03020100  07060504  0B0A0908  0F0E0D0C");

	for (i = 0; i < 0xff; i += 4) {
		if ((i % 16) == 0)
			printk(BIOS_DEBUG, "\n%02x: ", i);
		printk(BIOS_DEBUG, "%08x  ", pci_read_config32(dev, i));
	}
	printk(BIOS_DEBUG, "\n");
#endif

	printk(BIOS_DEBUG, "SATA_INIT:<----------\n");
}

static void lpci_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	pci_write_config32(dev, 0x40,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
//	.enable         = sis966_enable,
	.init = sata_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver sata0_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_SIS,
	.device = PCI_DEVICE_ID_SIS_SIS966_SATA,
};
