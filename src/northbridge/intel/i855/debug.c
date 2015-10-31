/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Ronald G. Minnich
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

#include <spd.h>

static void print_debug_pci_dev(unsigned dev)
{
	printk(BIOS_DEBUG, "PCI: %02x:%02x.%x",
		(dev >> 20) & 0xff, (dev >> 15) & 0x1f, (dev >> 12) & 0x07);
}

static inline void print_pci_devices(void)
{
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0);
		dev <= PCI_DEV(0, 0x1f, 0x7);
		dev += PCI_DEV(0,0,1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug_pci_dev(dev);
		printk(BIOS_DEBUG, "\n");
	}
}

static void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);
	printk(BIOS_DEBUG, "\n");

	for(i = 0; i <= 255; i++) {
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
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0);
		dev <= PCI_DEV(0, 0x1f, 0x7);
		dev += PCI_DEV(0,0,1)) {
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

static inline void dump_spd_registers(void)
{
	int i;
	printk(BIOS_DEBUG, "\n");
	for(i = 0; i < 2; i++) {
		unsigned device;
		device = DIMM0 + i;
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.0: %02x", i, device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0)
					printk(BIOS_DEBUG, "\n%02x: ", j);
				status = smbus_read_byte(device, j);
				if (status < 0) {
					printk(BIOS_DEBUG, "bad device\n");
					break;
				}
				byte = status & 0xff;
				printk(BIOS_DEBUG, "%02x ", byte);
			}
			printk(BIOS_DEBUG, "\n");
		}
	}
}

static inline void dump_smbus_registers(void)
{
        int i;
        printk(BIOS_DEBUG, "\n");
        for(i = 1; i < 0x80; i++) {
                unsigned device;
                device = i;
                int j;
                printk(BIOS_DEBUG, "smbus: %02x", device);
                for(j = 0; j < 256; j++) {
                	int status;
                        unsigned char byte;
                        if ((j & 0xf) == 0)
                	        printk(BIOS_DEBUG, "\n%02x: ", j);
                        status = smbus_read_byte(device, j);
                        if (status < 0) {
                                printk(BIOS_DEBUG, "bad device\n");
                                break;
                        }
                        byte = status & 0xff;
			printk(BIOS_DEBUG, "%02x ", byte);
                }
                printk(BIOS_DEBUG, "\n");
	}
}
