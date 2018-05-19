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
#include <device/pci_ehci.h>

static const u8 SiS_SiS7002_init[22][3]={
	{0x04, 0x00, 0x06},
	{0x0D, 0x00, 0x00},

	{0x2C, 0xFF, 0x39},
	{0x2D, 0xFF, 0x10},
	{0x2E, 0xFF, 0x02},
	{0x2F, 0xFF, 0x70},

	{0x74, 0x00, 0x00},
	{0x75, 0x00, 0x00},
	{0x76, 0x00, 0x00},
	{0x77, 0x00, 0x00},

	{0x7A, 0x00, 0x00},
	{0x7B, 0x00, 0x00},

	{0x40, 0x00, 0x20},
	{0x41, 0x00, 0x00},
	{0x42, 0x00, 0x00},
	{0x43, 0x00, 0x08},

	{0x44, 0x00, 0x04},

	{0x48, 0x00, 0x10},
	{0x49, 0x00, 0x80},
	{0x4A, 0x00, 0x07},
	{0x4B, 0x00, 0x00},

	{0x00, 0x00, 0x00} //End of table
};

static void usb2_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	int i;
	u8  temp8;

	printk(BIOS_DEBUG, "USB 2.0 INIT:---------->\n");

	//-------------- enable USB2.0 (SiS7002) ----------------------

	i = 0;
	while (SiS_SiS7002_init[i][0] != 0) {
		temp8 = pci_read_config8(dev, SiS_SiS7002_init[i][0]);
		temp8 &= SiS_SiS7002_init[i][1];
		temp8 |= SiS_SiS7002_init[i][2];
		pci_write_config8(dev, SiS_SiS7002_init[i][0], temp8);
		i++;
	};

	res = find_resource(dev, 0x10);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "base = 0x%p\n", base);
	write32(base + 0x20, 0x2);
	//-------------------------------------------------------------

#if DEBUG_USB2
	printk(BIOS_DEBUG, "****** USB 2.0 PCI config ******");
	printk(BIOS_DEBUG, "\n    03020100  07060504  0B0A0908  0F0E0D0C");

	for (i=0;i<0xff;i+=4) {
		if ((i%16)==0)
			printk(BIOS_DEBUG, "\n%02x: ", i);
		printk(BIOS_DEBUG, "%08x  ", pci_read_config32(dev,i));
	}
	printk(BIOS_DEBUG, "\n");
#endif
	printk(BIOS_DEBUG, "USB 2.0 INIT:<----------\n");
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

static struct device_operations usb2_ops  = {
	.read_resources	= pci_ehci_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init		= usb2_init,
//	.enable		= sis966_enable,
	.scan_bus	= 0,
	.ops_pci	= &lops_pci,
};

static const struct pci_driver usb2_driver __pci_driver = {
	.ops	= &usb2_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_USB2,
};
