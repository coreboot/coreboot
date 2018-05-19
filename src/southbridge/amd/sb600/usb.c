/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
#include "sb600.h"

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void usb_init(struct device *dev)
{
	u8 byte;
	u16 word;
	u32 dword;

	/* Enable OHCI0-4 and EHCI Controllers */
	struct device *sm_dev;
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	byte = pci_read_config8(sm_dev, 0x68);
	byte |= 0x3F;
	pci_write_config8(sm_dev, 0x68, byte);

	/* RPR 5.2 Enables the USB PME Event,Enable USB resume support */
	byte = pm_ioread(0x61);
	byte |= 1 << 6;
	pm_iowrite(0x61, byte);
	byte = pm_ioread(0x65);
	byte |= 1 << 2;
	pm_iowrite(0x65, byte);

	/* RPR 5.3 Support USB device wakeup from the S4/S5 state */
	byte = pm_ioread(0x65);
	byte &= ~(1 << 0);
	pm_iowrite(0x65, byte);

	/* RPR 5.6 Enable the USB controller to get reset by any software that generate a PCIRst# condition */
	byte = pm_ioread(0x65);
	byte |= (1 << 4);
	pm_iowrite(0x65, byte);

	/* RPR 5.11 Disable OHCI MSI Capability */
	word = pci_read_config16(dev, 0x40);
	word |= (0x1F << 8);
	pci_write_config16(dev, 0x40, word);

	/* RPR 5.8 Disable the OHCI Dynamic Power Saving feature  */
	dword = pci_read_config32(dev, 0x50);
	dword &= ~(1 << 16);
	pci_write_config32(dev, 0x50, dword);

	/* RPR 5.12 Enable prevention of OHCI accessing the invalid system memory address range */
	word = pci_read_config16(dev, 0x50);
	word |= 1 << 15;
	pci_write_config16(dev, 0x50, word);

	/* RPR 5.15 Disable SMI handshake in between USB and ACPI for USB legacy support. */
	/* The BIOS should always set this bit to prevent the malfunction on USB legacy keyboard/mouse support */
	word = pci_read_config16(dev, 0x50);
	word |= 1 << 12;
	pci_write_config16(dev, 0x50, word);
}

static void usb_init2(struct device *dev)
{
	u8 byte;
	u16 word;
	u32 dword;
	void *usb2_bar0;
	/* dword = pci_read_config32(dev, 0xf8); */
	/* dword |= 40; */
	/* pci_write_config32(dev, 0xf8, dword); */

	usb2_bar0 = (void *)(pci_read_config32(dev, 0x10) & ~0xFF);
	printk(BIOS_INFO, "usb2_bar0=0x%p\n", usb2_bar0);

	/* RPR5.4 Enables the USB PHY auto calibration resister to match 45ohm resistance */
	dword = 0x00020F00;
	write32(usb2_bar0 + 0xC0, dword);

	/* RPR5.5 Sets In/OUT FIFO threshold for best performance */
	dword = 0x00200040;
	write32(usb2_bar0 + 0xA4, dword);

	/* RPR5.9 Disable the EHCI Dynamic Power Saving feature */
	word = read16(usb2_bar0 + 0xBC);
	word &= ~(1 << 12);
	write16(usb2_bar0 + 0xBC, word);

	/* RPR5.10 Disable EHCI MSI support */
	byte = pci_read_config8(dev, 0x50);
	byte |= (1 << 6);
	pci_write_config8(dev, 0x50, byte);

	/* RPR5.13 Disable C3 time enhancement feature */
	dword = pci_read_config32(dev, 0x50);
	dword &= ~(1 << 28);
	pci_write_config32(dev, 0x50, dword);

	/* EHCI Erratum (adapted from Linux) */
	dword = pci_read_config32(dev, 0x53);
	dword |= (1 << 3);
	pci_write_config32(dev, 0x53, dword);

	/* RPR5.14 Disable USB PHY PLL Reset signal to come from ACPI */
	byte = pci_read_config8(dev, 0x54);
	byte &= ~(1 << 0);
	pci_write_config8(dev, 0x54, byte);
}

static struct device_operations usb_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init,
	/*.enable           = sb600_enable, */
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const unsigned short usb_ids[] = {
	PCI_DEVICE_ID_ATI_SB600_USB_0,
	PCI_DEVICE_ID_ATI_SB600_USB_1,
	PCI_DEVICE_ID_ATI_SB600_USB_2,
	PCI_DEVICE_ID_ATI_SB600_USB_3,
	PCI_DEVICE_ID_ATI_SB600_USB_4,
	0,
};

static const struct pci_driver usb_0_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.devices= usb_ids,
};

static struct device_operations usb_ops2 = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init2,
	/*.enable           = sb600_enable, */
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver usb_5_driver __pci_driver = {
	.ops = &usb_ops2,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB600_USB2,
};
