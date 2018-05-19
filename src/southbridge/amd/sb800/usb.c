/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
#include <device/pci_ops.h>
#include <device/pci_ehci.h>
#include <arch/io.h>
#include "sb800.h"

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void usb_init(struct device *dev)
{
	u8 byte;
	u16 word;

	/* 7.1 Enable OHCI0-4 and EHCI Controllers */
	/* pmio 0xEF; */

	/* RPR 7.2 USB S4/S5 Wake-up or PHY Power-down Support */
	byte = pm_ioread(0xF0);
	byte |= 1 << 0;		/* A12, USB Wake from S5 not supported on the platform */
	pm_iowrite(0xF0, byte);

	/* RPR 7.4 Enable the USB controller to get reset by any software that generate a PCIRst# condition */
	byte = pm_ioread(0xF0);
	byte |= (1 << 2);
	byte |= 3 << 8;		/* rpr 7.5 */
	pm_iowrite(0xF0, byte);

	/* RPR 7.9 Disable OHCI MSI Capability. */
	word = pci_read_config16(dev, 0x40);
	word |= (0x1 << 8);
	pci_write_config16(dev, 0x40, word);
}

static void usb_init2(struct device *dev)
{
	u32 dword;
	void *usb2_bar0;
	struct device *sm_dev;

	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	//rev = get_sb800_revision(sm_dev);

	/* dword = pci_read_config32(dev, 0xf8); */
	/* dword |= 40; */
	/* pci_write_config32(dev, 0xf8, dword); */

	usb2_bar0 = (void *)(pci_read_config32(dev, 0x10) & ~0xFF);
	printk(BIOS_INFO, "usb2_bar0=0x%p\n", usb2_bar0);

	/* RPR7.3 Enables the USB PHY auto calibration resister to match 45ohm resistance */
	dword = 0x00020F00;
	write32(usb2_bar0 + 0xC0, dword);

	/* RPR7.8 Sets In/OUT FIFO threshold for best performance */
	dword = 0x00400040;
	write32(usb2_bar0 + 0xA4, dword);

	/* RPR7.10 Disable EHCI MSI support */
	dword = pci_read_config32(dev, 0x50);
	dword |= (1 << 6);
	pci_write_config32(dev, 0x50, dword);

	/* RPR7.12 EHCI Async Park Mode */
	dword = pci_read_config32(dev, 0x50);
	dword &= ~(0xF << 8);
	dword &= ~(0xF << 12);
	dword |= 1 << 8;
	dword |= 2 << 12;
	pci_write_config32(dev, 0x50, dword);

	/* RPR 6.12 EHCI Advance PHY Power Savings */
	/* RPR says it is just for A12. CIMM sets it when it is above A11. */
	/* But it makes the linux crash, so we skip it */
	#if 0
	dword = pci_read_config32(dev, 0x50);
	dword |= 1 << 31;
	pci_write_config32(dev, 0x50, dword);
	#endif

	/* Each step below causes the linux crashes. Leave them here
	 * for future debugging. */
#if 0
	u8 byte;
	u16 word;


	/* RPR6.17 Disable the EHCI Dynamic Power Saving feature */
	word = read32(usb2_bar0 + 0xBC);
	word &= ~(1 << 12);
	write16(usb2_bar0 + 0xBC, word);

	/* RPR6.19 USB Controller DMA Read Delay Tolerant. */
	if (rev >= REV_SB800_A14) {
		byte = pci_read_config8(dev, 0x50);
		byte |= (1 << 7);
		pci_write_config8(dev, 0x50, byte);
	}

	/* RPR6.20 Async Park Mode. */
	/* RPR recommends not to set these bits. */
	#if 0
	dword = pci_read_config32(dev, 0x50);
	dword |= 1 << 23;
	if (rev >= REV_SB800_A14) {
		dword &= ~(1 << 2);
	}
	pci_write_config32(dev, 0x50, dword);
	#endif

	/* RPR6.22 Advance Async Enhancement */
	/* RPR6.23 USB Periodic Cache Setting */
	dword = pci_read_config32(dev, 0x50);
	if (rev == REV_SB800_A12) {
		dword |= 1 << 28; /* 6.22 */
		dword |= 1 << 27; /* 6.23 */
	} else if (rev >= REV_SB800_A14) {
		dword |= 1 << 3;
		dword &= ~(1 << 28); /* 6.22 */
		dword |= 1 << 8;
		dword &= ~(1 << 27); /* 6.23 */
	}
	printk(BIOS_DEBUG, "rpr 6.23, final dword=%x\n", dword);
#endif
}

static struct device_operations usb_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver usb_0_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_18_0,
};
static const struct pci_driver usb_1_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_18_2,
};

/* the pci id of usb ctrl 0 and 1 are the same. */
/*
 * static const struct pci_driver usb_3_driver __pci_driver = {
 * 	.ops = &usb_ops,
 * 	.vendor = PCI_VENDOR_ID_ATI,
 * 	.device = PCI_DEVICE_ID_ATI_SB800_USB_19_0,
 * };
 * static const struct pci_driver usb_4_driver __pci_driver = {
 * 	.ops = &usb_ops,
 * 	.vendor = PCI_VENDOR_ID_ATI,
 * 	.device = PCI_DEVICE_ID_ATI_SB800_USB_19_1,
 * };
 */

static const struct pci_driver usb_4_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_20_5,
};

static struct device_operations usb_ops2 = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init2,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver usb_5_driver __pci_driver = {
	.ops = &usb_ops2,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_18_2,
};
/*
 * static const struct pci_driver usb_5_driver __pci_driver = {
 * 	.ops = &usb_ops2,
 * 	.vendor = PCI_VENDOR_ID_ATI,
 * 	.device = PCI_DEVICE_ID_ATI_SB800_USB_19_2,
 * };
 */
