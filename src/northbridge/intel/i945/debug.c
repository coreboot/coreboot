/* SPDX-License-Identifier: GPL-2.0-only */

#include <spd.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <device/smbus_host.h>
#include <console/console.h>
#include "i945.h"

void print_pci_devices(void)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(0, 0, 0); dev <= PCI_DEV(0, 0x1f, 0x7); dev += PCI_DEV(0, 0, 1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		printk(BIOS_DEBUG, "PCI: %02x:%02x.%02x", (dev >> 20) & 0xff,
			(dev >> 15) & 0x1f, (dev >> 12) & 7);
		printk(BIOS_DEBUG, " [%04x:%04x]\n", id & 0xffff, id >> 16);
	}
}

void dump_pci_device(unsigned int dev)
{
	int i;

	printk(BIOS_DEBUG, "PCI: %02x:%02x.%02x\n", (dev >> 20) & 0xff, (dev >> 15) & 0x1f,
		(dev >> 12) & 7);

	for (i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "%02x:", i);
		val = pci_read_config8(dev, i);
		printk(BIOS_DEBUG, " %02x", val);
		if ((i & 0x0f) == 0x0f)
			printk(BIOS_DEBUG, "\n");
	}
}

void dump_pci_devices(void)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(0, 0, 0); dev <= PCI_DEV(0, 0x1f, 0x7); dev += PCI_DEV(0, 0, 1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		dump_pci_device(dev);
	}
}

void dump_spd_registers(u8 spd_map[4])
{
	for (unsigned int d = 0; d < 4; d++) {
		const unsigned int device = spd_map[d];
		if (device == 0)
			continue;

		int status = 0;
		int i;
		printk(BIOS_DEBUG, "\ndimm %02x", device);

		for (i = 0; (i < 256); i++) {
			if ((i % 16) == 0)
				printk(BIOS_DEBUG, "\n%02x: ", i);
			status = smbus_read_byte(device, i);
			if (status < 0) {
				printk(BIOS_DEBUG, "bad device: %02x\n", -status);
				break;
			}
			printk(BIOS_DEBUG, "%02x ", status);
		}
		printk(BIOS_DEBUG, "\n");
	}
}
