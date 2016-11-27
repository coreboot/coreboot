/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include "debug.h"
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <delay.h>

void print_debug_addr(const char *str, void *val)
{
#if CONFIG_DEBUG_CAR
		printk(BIOS_DEBUG, "------Address debug: %s%p------\n", str, val);
#endif
}

void print_debug_pci_dev(u32 dev)
{
	printk(BIOS_DEBUG, "PCI: %02x:%02x.%02x", (dev>>20) & 0xff, (dev>>15) & 0x1f, (dev>>12) & 0x7);
}

void print_pci_devices(void)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(0, 0, 0);
		dev <= PCI_DEV(0xff, 0x1f, 0x7);
		dev += PCI_DEV(0,0,1)) {
		u32 id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug_pci_dev(dev);
		printk(BIOS_DEBUG, " %04x:%04x\n", (id & 0xffff), (id>>16));
		if (((dev>>12) & 0x07) == 0) {
			u8 hdr_type;
			hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
			if ((hdr_type & 0x80) != 0x80) {
				dev += PCI_DEV(0,0,7);
			}
		}
	}
}

void print_pci_devices_on_bus(u32 busn)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(busn, 0, 0);
		dev <= PCI_DEV(busn, 0x1f, 0x7);
		dev += PCI_DEV(0,0,1)) {
		u32 id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
		   (((id >> 16) & 0xffff) == 0xffff) ||
		   (((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug_pci_dev(dev);
		printk(BIOS_DEBUG, " %04x:%04x\n", (id & 0xffff), (id>>16));
		if (((dev>>12) & 0x07) == 0) {
			u8 hdr_type;
			hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
			if ((hdr_type & 0x80) != 0x80) {
				 dev += PCI_DEV(0,0,7);
			}
		}
	}
}

void dump_pci_device_range(u32 dev, u32 start_reg, u32 size)
{
	int i;
	print_debug_pci_dev(dev);
	int j;
	int end = start_reg + size;

	for (i = start_reg; i < end; i+=4) {
		u32 val;
		if ((i & 0x0f) == 0) {
			printk(BIOS_DEBUG, "\n%04x:",i);
		}
		val = pci_read_config32(dev, i);
		for (j = 0; j < 4; j++) {
			printk(BIOS_DEBUG, " %02x", val & 0xff);
			val >>= 8;
		}
	}
	printk(BIOS_DEBUG, "\n");
}

void dump_pci_device(u32 dev)
{
	dump_pci_device_range(dev, 0, 4096);
}

void dump_pci_device_index_wait_range(u32 dev, u32 index_reg, u32 start,
					u32 size)
{
	int i;
	int end = start + size;
	print_debug_pci_dev(dev);
	printk(BIOS_DEBUG, " -- index_reg=%08x", index_reg);

	for (i = start; i < end; i++) {
		u32 val;
		int j;
		printk(BIOS_DEBUG, "\n%02x:",i);
		val = pci_read_config32_index_wait(dev, index_reg, i);
		for (j = 0; j < 4; j++) {
			printk(BIOS_DEBUG, " %02x", val & 0xff);
			val >>= 8;
		}

	}
	printk(BIOS_DEBUG, "\n");
}

void dump_pci_device_index_wait(u32 dev, u32 index_reg)
{
	dump_pci_device_index_wait_range(dev, index_reg, 0, 0x54);
	dump_pci_device_index_wait_range(dev, index_reg, 0x100, 0x08); //DIMM1 when memclk > 400Hz
}

void dump_pci_device_index(u32 dev, u32 index_reg, u32 type, u32 length)
{
	int i;
	print_debug_pci_dev(dev);

	printk(BIOS_DEBUG, " index reg: %04x type: %02x", index_reg, type);

	type<<=28;

	for (i = 0; i < length; i++) {
		u32 val;
		if ((i & 0x0f) == 0) {
			printk(BIOS_DEBUG, "\n%02x:",i);
		}
		val = pci_read_config32_index(dev, index_reg, i|type);
		printk(BIOS_DEBUG, " %08x", val);
	}
	printk(BIOS_DEBUG, "\n");
}

void dump_pci_devices(void)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(0, 0, 0);
		dev <= PCI_DEV(0xff, 0x1f, 0x7);
		dev += PCI_DEV(0,0,1)) {
		u32 id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		dump_pci_device(dev);

		if (((dev>>12) & 0x07) == 0) {
			u8 hdr_type;
			hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
			if ((hdr_type & 0x80) != 0x80) {
				dev += PCI_DEV(0,0,7);
			}
		}
	}
}

void dump_pci_devices_on_bus(u32 busn)
{
	pci_devfn_t dev;
	for (dev = PCI_DEV(busn, 0, 0);
		dev <= PCI_DEV(busn, 0x1f, 0x7);
		dev += PCI_DEV(0,0,1)) {
		u32 id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
		   (((id >> 16) & 0xffff) == 0xffff) ||
		   (((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		dump_pci_device(dev);

		if (((dev>>12) & 0x07) == 0) {
			u8 hdr_type;
			hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
			if ((hdr_type & 0x80) != 0x80) {
				dev += PCI_DEV(0,0,7);
			}
		}
	}
}

#if CONFIG_DEBUG_SMBUS
void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	printk(BIOS_DEBUG, "\n");
	for (i = 0; i < DIMM_SOCKETS; i++) {
		u32 device;
		device = ctrl->spd_addr[i];
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.0: %02x", i, device);
			for (j = 0; j < 128; j++) {
				int status;
				u8 byte;
				if ((j & 0xf) == 0) {
					printk(BIOS_DEBUG, "\n%02x: ", j);
				}
				status = smbus_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
				printk(BIOS_DEBUG, "%02x ", byte);
			}
			printk(BIOS_DEBUG, "\n");
		}
		device = ctrl->spd_addr[i+DIMM_SOCKETS];
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.1: %02x", i, device);
			for (j = 0; j < 128; j++) {
				int status;
				u8 byte;
				if ((j & 0xf) == 0) {
					printk(BIOS_DEBUG, "\n%02x: ", j);
				}
				status = smbus_read_byte(device, j);
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
	u32 device;
	printk(BIOS_DEBUG, "\n");
	for (device = 1; device < 0x80; device++) {
		int j;
		if (smbus_read_byte(device, 0) < 0) continue;
		printk(BIOS_DEBUG, "smbus: %02x", device);
		for (j = 0; j < 256; j++) {
			int status;
			u8 byte;
			status = smbus_read_byte(device, j);
			if (status < 0) {
				break;
			}
			if ((j & 0xf) == 0) {
				printk(BIOS_DEBUG, "\n%02x: ",j);
			}
			byte = status & 0xff;
			printk(BIOS_DEBUG, "%02x ", byte);
		}
		printk(BIOS_DEBUG, "\n");
	}
}
#endif

void dump_io_resources(u32 port)
{

	int i;
	udelay(2000);
	printk(BIOS_DEBUG, "%04x:\n", port);
	for (i = 0; i < 256; i++) {
		u8 val;
		if ((i & 0x0f) == 0) {
			printk(BIOS_DEBUG, "%02x:", i);
		}
		val = inb(port);
		printk(BIOS_DEBUG, " %02x",val);
		if ((i & 0x0f) == 0x0f) {
			printk(BIOS_DEBUG, "\n");
		}
		port++;
	}
}

void dump_mem(u32 start, u32 end)
{
	u32 i;
	printk(BIOS_DEBUG, "dump_mem:");
	for (i = start; i < end; i++) {
		if ((i & 0xf) == 0) {
			printk(BIOS_DEBUG, "\n%08x:", i);
		}
		printk(BIOS_DEBUG, " %02x", (u8)*((u8 *)i));
	}
	printk(BIOS_DEBUG, "\n");
}

#if IS_ENABLED(CONFIG_DIMM_DDR2)
void print_tx(const char *strval, u32 val)
{
#if CONFIG_DEBUG_RAM_SETUP
	printk(BIOS_DEBUG, "%s%08x\n", strval, val);
#endif
}

void print_t(const char *strval)
{
#if CONFIG_DEBUG_RAM_SETUP
	printk(BIOS_DEBUG, "%s", strval);
#endif
}
#endif /* CONFIG_DIMM_DDR2 */

void print_tf(const char *func, const char *strval)
{
#if CONFIG_DEBUG_RAM_SETUP
	printk(BIOS_DEBUG, "%s: %s", func, strval);
#endif
}
