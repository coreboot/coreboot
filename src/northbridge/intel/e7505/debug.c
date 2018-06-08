/*
 * This file is part of the coreboot project.
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

#include <device/pci_def.h>
#include <console/console.h>
#include <stdlib.h>
#include <arch/io.h>
#include <spd.h>

#include "raminit.h"
#include "debug.h"

/*
 * generic debug code, used by mainboard specific romstage.c
 *
 */

void print_debug_pci_dev(unsigned dev)
{
	printk(BIOS_DEBUG, "PCI: %02x:%02x.%x",
		(dev >> 16) & 0xff, (dev >> 11) & 0x1f, (dev >> 8) & 7);
}

void print_pci_devices(void)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(0, 0, 0);
		dev <= PCI_DEV(0xff, 0x1f, 0x7);
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

void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);

	for (i = 0; i < 256; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "\n%02x:",i);
		val = pci_read_config8(dev, i);
		printk(BIOS_DEBUG, " %02x", val);
	}
	printk(BIOS_DEBUG, "\n");
}

void dump_pci_devices(void)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(0, 0, 0);
		dev <= PCI_DEV(0xff, 0x1f, 0x7);
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

void dump_pci_devices_on_bus(unsigned busn)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(busn, 0, 0);
		dev <= PCI_DEV(busn, 0x1f, 0x7);
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

void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	printk(BIOS_DEBUG, "\n");
	for (i = 0; i < 4; i++) {
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.0: %02x", i, device);
			for (j = 0; j < 128; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0)
					printk(BIOS_DEBUG, "\n%02x: ", j);
				status = spd_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
				printk(BIOS_DEBUG, "%02x ", byte);
			}
			printk(BIOS_DEBUG, "\n");
		}
		device = ctrl->channel1[i];
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.1: %02x", i, device);
			for (j = 0; j < 128; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0)
					printk(BIOS_DEBUG, "\n%02x: ", j);
				status = spd_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
				printk(BIOS_DEBUG, "%02x ", byte);
			}
			printk(BIOS_DEBUG, "\n");
		}
	}
}
void dump_smbus_registers(void)
{
	unsigned device;
	printk(BIOS_DEBUG, "\n");
	for (device = 1; device < 0x80; device++) {
		int j;
		if ( spd_read_byte(device, 0) < 0 ) continue;
		printk(BIOS_DEBUG, "smbus: %02x", device);
		for (j = 0; j < 256; j++) {
			int status;
			unsigned char byte;
			status = spd_read_byte(device, j);
			if (status < 0) {
				break;
			}
			if ((j & 0xf) == 0)
				printk(BIOS_DEBUG, "\n%02x: ",j);
			byte = status & 0xff;
			printk(BIOS_DEBUG, "%02x ", byte);
		}
		printk(BIOS_DEBUG, "\n");
	}
}

void dump_io_resources(unsigned port)
{
	int i;
	printk(BIOS_DEBUG, "%04x:\n", port);
	for (i = 0; i < 256; i++) {
		uint8_t val;
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "%02x:", i);
		val = inb(port);
		printk(BIOS_DEBUG, " %02x",val);
		if ((i & 0x0f) == 0x0f) {
			printk(BIOS_DEBUG, "\n");
		}
		port++;
	}
}

void dump_mem(unsigned start, unsigned end)
{
	unsigned i;
	printk(BIOS_DEBUG, "dump_mem:");
	for (i = start; i < end; i++) {
		if ((i & 0xf)==0)
			printk(BIOS_DEBUG, "\n%08x:", i);
		printk(BIOS_DEBUG, " %02x", (unsigned char)*((unsigned char *)i));
	}
	printk(BIOS_DEBUG, "\n");
}
