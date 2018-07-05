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
#include <option.h>
#include "sb700.h"

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void usb_init(struct device *dev)
{
	u8 byte;
	u16 word;
	u32 dword;

	/* 6.1 Enable OHCI0-4 and EHCI Controllers */
	struct device *sm_dev;
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	byte = pci_read_config8(sm_dev, 0x68);
	byte |= 0xFF;
	pci_write_config8(sm_dev, 0x68, byte);

	/* RPR 6.2 Enables the USB PME Event,Enable USB resume support */
	byte = pm_ioread(0x61);
	byte |= 1 << 6;
	pm_iowrite(0x61, byte);
	byte = pm_ioread(0x65);
	byte |= 1 << 2;
	pm_iowrite(0x65, byte);

	/* RPR 6.3 Support USB device wakeup from the S4/S5 state */
	byte = pm_ioread(0x65);
	byte &= ~(1 << 0);
	pm_iowrite(0x65, byte);

	/* RPR 6.5 Enable the USB controller to get reset by any software that generate a PCIRst# condition */
	byte = pm_ioread(0x65);
	byte |= (1 << 4);
	pm_iowrite(0x65, byte);

	/* USB_ADVANCED_SLEEP_CONTROL */
	byte = pm_ioread(0x95);
	byte &= ~(7 << 0);
	byte |= 6 << 0;		/* Advanced sleep up to 6 uframes */
	pm_iowrite(0x95, byte);

	/* RPR 6.10 Disable OHCI MSI Capability. */
	word = pci_read_config16(dev, 0x40);
	word |= (0x3 << 8);
	pci_write_config16(dev, 0x40, word);

	/* USB-1_OHCI0_Corner Case S3 Wake Up */
	dword = pci_read_config32(dev, 0x50);
	dword |= (1 << 16);
	pci_write_config32(dev, 0x50, dword);
}

static void usb_init2(struct device *dev)
{
	uint32_t dword;
	void *usb2_bar0;
	struct device *sm_dev;
	uint8_t rev;
	uint8_t ehci_async_data_cache;
	uint8_t nvram;

	ehci_async_data_cache = 1;
	if (get_option(&nvram, "ehci_async_data_cache") == CB_SUCCESS)
		ehci_async_data_cache = !!nvram;

	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	rev = get_sb700_revision(sm_dev);

	/* dword = pci_read_config32(dev, 0xf8); */
	/* dword |= 40; */
	/* pci_write_config32(dev, 0xf8, dword); */

	usb2_bar0 = (void *)(pci_read_config32(dev, 0x10) & ~0xFF);
	printk(BIOS_INFO, "usb2_bar0=0x%p\n", usb2_bar0);

	/* RPR6.4 Enables the USB PHY auto calibration resister to match 45ohm resistance */
	dword = 0x00020F00;
	write32(usb2_bar0 + 0xC0, dword);

	/* RPR6.9 Sets In/OUT FIFO threshold for best performance */
	dword = 0x00400040;
	write32(usb2_bar0 + 0xA4, dword);

	/* RPR6.11 Disabling EHCI Advance Asynchronous Enhancement */
	dword = pci_read_config32(dev, 0x50);
	dword |= (1 << 28);
	pci_write_config32(dev, 0x50, dword);

	/* RPR 6.12 EHCI Advance PHY Power Savings */
	/* RPR says it is just for A12. CIMM sets it when it is above A11. */
	/* But it makes the linux crash, so we skip it */
	dword = pci_read_config32(dev, 0x50);
	dword |= 1 << 31;
	pci_write_config32(dev, 0x50, dword);

	/* RPR6.13 Enabling Fix for EHCI Controller Driver Yellow Sign Issue */
	/* RPR says it is just for A12. CIMx sets it when it is above A11. */
	dword = pci_read_config32(dev, 0x50);
	dword |= (1 << 20);
	pci_write_config32(dev, 0x50, dword);

	/* RPR6.15 EHCI Async Park Mode */
	dword = pci_read_config32(dev, 0x50);
	dword |= (1 << 23);
	pci_write_config32(dev, 0x50, dword);

	/* Each step below causes the linux crashes. Leave them here
	 * for future debugging. */
	u8 byte;
	u16 word;

	/* RPR6.16 Disable EHCI MSI support */
	byte = pci_read_config8(dev, 0x50);
	byte |= (1 << 6);
	pci_write_config8(dev, 0x50, byte);

	/* RPR6.17 Disable the EHCI Dynamic Power Saving feature */
	word = read32(usb2_bar0 + 0xBC);
	word &= ~(1 << 12);
	write16(usb2_bar0 + 0xBC, word);

	/* RPR6.19 USB Controller DMA Read Delay Tolerant. */
	if (rev >= REV_SB700_A14) {
		byte = pci_read_config8(dev, 0x50);
		byte |= (1 << 7);
		pci_write_config8(dev, 0x50, byte);
	}

	/* SB700_A15, USB-2_EHCI_PID_ERROR_CHECKING */
	if (rev == REV_SB700_A15) {
		word = pci_read_config16(dev, 0x50);
		word |= (1 << 9);
		pci_write_config16(dev, 0x50, word);
	}

	/* RPR6.20 Async Park Mode. */
	/* RPR recommends not to set these bits. */
	#if 0
	dword = pci_read_config32(dev, 0x50);
	dword |= 1 << 23;
	if (rev >= REV_SB700_A14) {
		dword &= ~(1 << 2);
	}
	pci_write_config32(dev, 0x50, dword);
	#endif

	/* RPR6.22 Advance Async Enhancement */
	/* RPR6.23 USB Periodic Cache Setting */
	dword = pci_read_config32(dev, 0x50);
	if (rev == REV_SB700_A12) {
		dword |= 1 << 28; /* 6.22 */
		dword |= 1 << 27; /* 6.23 */
	} else if (rev >= REV_SB700_A14) {
		dword |= 1 << 3;
		dword &= ~(1 << 28); /* 6.22 */
		dword |= 1 << 8;
		dword &= ~(1 << 27); /* 6.23 */
	}
#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_SUBTYPE_SP5100)
	/* SP5100 Erratum 36 */
	dword &= ~(1 << 26);
	if (!ehci_async_data_cache)
		dword |= 1 << 26;
#endif
	pci_write_config32(dev, 0x50, dword);
	printk(BIOS_DEBUG, "rpr 6.23, final dword=%x\n", dword);
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
	.device = PCI_DEVICE_ID_ATI_SB700_USB_18_0,
};

static const struct pci_driver usb_1_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_USB_18_1,
};

/* the pci id of usb ctrl 0 and 1 are the same. */
/*
 * static const struct pci_driver usb_3_driver __pci_driver = {
 *	.ops = &usb_ops,
 *	.vendor = PCI_VENDOR_ID_ATI,
 *	.device = PCI_DEVICE_ID_ATI_SB700_USB_19_0,
 * };
 * static const struct pci_driver usb_4_driver __pci_driver = {
 *	.ops = &usb_ops,
 *	.vendor = PCI_VENDOR_ID_ATI,
 *	.device = PCI_DEVICE_ID_ATI_SB700_USB_19_1,
 * };
 */

static const struct pci_driver usb_4_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_USB_20_5,
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
	.device = PCI_DEVICE_ID_ATI_SB700_USB_18_2,
};
/*
 * static const struct pci_driver usb_5_driver __pci_driver = {
 *	.ops = &usb_ops2,
 *	.vendor = PCI_VENDOR_ID_ATI,
 *	.device = PCI_DEVICE_ID_ATI_SB700_USB_19_2,
 * };
 */
