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
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "sis966.h"
#include "chip.h"

uint8_t	SiS_SiS5513_init[49][3]={
{0x04, 0xFF, 0x05},
{0x0D, 0xFF, 0x80},
{0x2C, 0xFF, 0x39},
{0x2D, 0xFF, 0x10},
{0x2E, 0xFF, 0x13},
{0x2F, 0xFF, 0x55},
{0x50, 0xFF, 0xA2},
{0x51, 0xFF, 0x21},
{0x53, 0xFF, 0x21},
{0x54, 0xFF, 0x2A},
{0x55, 0xFF, 0x96},
{0x52, 0xFF, 0xA2},
{0x56, 0xFF, 0x81},
{0x57, 0xFF, 0xC0},
{0x60, 0xFF, 0xFB},
{0x61, 0xFF, 0xAA},
{0x62, 0xFF, 0xFB},
{0x63, 0xFF, 0xAA},
{0x81, 0xFF, 0xB3},
{0x82, 0xFF, 0x72},
{0x83, 0xFF, 0x40},
{0x85, 0xFF, 0xB3},
{0x86, 0xFF, 0x72},
{0x87, 0xFF, 0x40},
{0x94, 0xFF, 0xC0},
{0x95, 0xFF, 0x08},
{0x96, 0xFF, 0xC0},
{0x97, 0xFF, 0x08},
{0x98, 0xFF, 0xCC},
{0x99, 0xFF, 0x04},
{0x9A, 0xFF, 0x0C},
{0x9B, 0xFF, 0x14},
{0xA0, 0xFF, 0x11},
{0x57, 0xFF, 0xD0},

{0xD8, 0xFE, 0x01},    // Com reset
{0xC8, 0xFE, 0x01},
{0xC4, 0xFF, 0xFF},    // Clear status
{0xC5, 0xFF, 0xFF},
{0xC6, 0xFF, 0xFF},
{0xC7, 0xFF, 0xFF},
{0xD4, 0xFF, 0xFF},
{0xD5, 0xFF, 0xFF},
{0xD6, 0xFF, 0xFF},
{0xD7, 0xFF, 0xFF},


{0x2C, 0xFF, 0x39},	// set subsystem ID
{0x2D, 0xFF, 0x10},
{0x2E, 0xFF, 0x13},
{0x2F, 0xFF, 0x55},


{0x00, 0x00, 0x00}					//End of table
};

static void ide_init(struct device *dev)
{
	struct southbridge_sis_sis966_config *conf;
	/* Enable ide devices so the linux ide driver will work */
	uint32_t dword;
	uint16_t word;
	uint8_t byte;
	conf = dev->chip_info;



printk(BIOS_DEBUG, "IDE_INIT:---------->\n");


//-------------- enable IDE (SiS5513) -------------------------
{
	uint8_t  temp8;
	int i=0;
	while (SiS_SiS5513_init[i][0] != 0) {
		temp8 = pci_read_config8(dev, SiS_SiS5513_init[i][0]);
		temp8 &= SiS_SiS5513_init[i][1];
		temp8 |= SiS_SiS5513_init[i][2];
		pci_write_config8(dev, SiS_SiS5513_init[i][0], temp8);
		i++;
	};
}
//-----------------------------------------------------------

	word = pci_read_config16(dev, 0x50);
	/* Ensure prefetch is disabled */
	word &= ~((1 << 15) | (1 << 13));
	if (conf->ide1_enable) {
		/* Enable secondary ide interface */
		word |= (1<<0);
		printk(BIOS_DEBUG, "IDE1\t");
	}
	if (conf->ide0_enable) {
		/* Enable primary ide interface */
		word |= (1<<1);
		printk(BIOS_DEBUG, "IDE0\n");
	}

	word |= (1<<12);
	word |= (1<<14);

	pci_write_config16(dev, 0x50, word);


	byte = 0x20; // Latency: 64-->32
	pci_write_config8(dev, 0xd, byte);

	dword = pci_read_config32(dev, 0xf8);
	dword |= 12;
	pci_write_config32(dev, 0xf8, dword);

#if DEBUG_IDE
{
	int i;

	printk(BIOS_DEBUG, "****** IDE PCI config ******");
	printk(BIOS_DEBUG, "\n    03020100  07060504  0B0A0908  0F0E0D0C");

	for (i=0;i<0xff;i+=4) {
		if ((i%16)==0)
			printk(BIOS_DEBUG, "\n%02x: ", i);
		printk(BIOS_DEBUG, "%08x  ", pci_read_config32(dev,i));
	}
	printk(BIOS_DEBUG, "\n");
}
#endif
printk(BIOS_DEBUG, "IDE_INIT:<----------\n");
}

static void lpci_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}
static struct pci_operations lops_pci = {
	.set_subsystem	= lpci_set_subsystem,
};

static struct device_operations ide_ops  = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init		= ide_init,
	.scan_bus	= 0,
//	.enable		= sis966_enable,
	.ops_pci	= &lops_pci,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_IDE,
};
