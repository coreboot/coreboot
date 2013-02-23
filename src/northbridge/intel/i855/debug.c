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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <spd.h>

static void print_debug_pci_dev(unsigned dev)
{
	print_debug("PCI: ");
	print_debug_hex8((dev >> 20) & 0xff);
	print_debug_char(':');
	print_debug_hex8((dev >> 15) & 0x1f);
	print_debug_char('.');
	print_debug_hex8((dev >> 12) & 0x07);
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
		print_debug("\n");
	}
}

static void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);
	print_debug("\n");

	for(i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
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
	print_debug("\n");
	for(i = 0; i < 2; i++) {
		unsigned device;
		device = DIMM0 + i;
		if (device) {
			int j;
			print_debug("dimm: ");
			print_debug_hex8(i);
			print_debug(".0: ");
			print_debug_hex8(device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
					print_debug("\n");
					print_debug_hex8(j);
					print_debug(": ");
				}
				status = smbus_read_byte(device, j);
				if (status < 0) {
					print_debug("bad device\n");
					break;
				}
				byte = status & 0xff;
				print_debug_hex8(byte);
				print_debug_char(' ');
			}
			print_debug("\n");
		}
	}
}

static inline void dump_smbus_registers(void)
{
        int i;
        print_debug("\n");
        for(i = 1; i < 0x80; i++) {
                unsigned device;
                device = i;
                int j;
                print_debug("smbus: ");
                print_debug_hex8(device);
                for(j = 0; j < 256; j++) {
                	int status;
                        unsigned char byte;
                        if ((j & 0xf) == 0) {
                	        print_debug("\n");
                                print_debug_hex8(j);
                                print_debug(": ");
                        }
                        status = smbus_read_byte(device, j);
                        if (status < 0) {
                                print_debug("bad device\n");
                                break;
                        }
                        byte = status & 0xff;
                        print_debug_hex8(byte);
                        print_debug_char(' ');
                }
                print_debug("\n");
	}
}
