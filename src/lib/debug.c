/*
 * This file is part of the coreboot project.
 *
 * (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

static void print_debug_pci_dev(unsigned int dev)
{
	printk(BIOS_DEBUG, "PCI: %02x:%02x.%x",
		(dev >> 16) & 0xff, (dev >> 11) & 0x1f, (dev >> 8) & 7);
}

static inline void print_pci_devices(void)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	for (dev = PCI_DEV(0, 0, 0);
	     dev <= PCI_DEV(0x00, 0x1f, 0x7); dev += PCI_DEV(0, 0, 1)) {
		u32 id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug_pci_dev(dev);
		printk(BIOS_DEBUG, "\n");
	}
}

static void dump_pci_device(unsigned int dev)
{
	int i;
	print_debug_pci_dev(dev);
	printk(BIOS_DEBUG, "\n");

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

static inline void dump_pci_devices(void)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	for (dev = PCI_DEV(0, 0, 0);
	     dev <= PCI_DEV(0, 0x1f, 0x7); dev += PCI_DEV(0, 0, 1)) {
		u32 id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		dump_pci_device(dev);
	}
}


static inline void dump_io_resources(unsigned int port)
{
	int i;
	printk(BIOS_DEBUG, "%04x:\n", port);
	for (i = 0; i < 256; i++) {
		u8 val;
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "%02x:", i);
		val = inb(port);
		printk(BIOS_DEBUG, " %02x", val);
		if ((i & 0x0f) == 0x0f)
			printk(BIOS_DEBUG, "\n");
		port++;
	}
}
