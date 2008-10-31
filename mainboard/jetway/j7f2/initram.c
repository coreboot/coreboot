/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define _MAINOBJECT

#include <types.h>
#include <lib.h>
#include <console.h>
#include <io.h>
#include <spd.h>
#include <via_c7.h>
#include <arch/x86/pci_ops.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <superio/fintek/f71805f/f71805f.h>
#include <southbridge/via/vt8237/vt8237.h>
#include <northbridge/via/cn700/cn700.h>

#define SMBUS_IO_BASE	0x0400

u8 spd_read_byte(u16 dev, u8 addr)
{
	return smbus_read_byte(dev, addr, SMBUS_IO_BASE);
}

void find_smbus_devices(u8 min, u8 max)
{
	u8 dev;
	u8 result;
	for(dev = min; dev < max; dev++)
	{
		result = spd_read_byte(dev, SPD_MEMORY_TYPE);
		switch(result)
		{
			case SPD_MEMORY_TYPE_SDRAM: printk(BIOS_DEBUG,
				"Possible SDRAM spd at address 0x%2x\n", dev);
				break;
			case SPD_MEMORY_TYPE_SDRAM_DDR: printk(BIOS_DEBUG,
				"Possible DDR SDRAM spd at address 0x%2x\n", dev);
				break;
			case SPD_MEMORY_TYPE_SDRAM_DDR2: printk(BIOS_DEBUG,
				"Possible DDR2 SDRAM spd at address 0x%2x\n", dev);
				break;
		};
	}
}


void dump_smbus_registers(void)
{
	int device;
	for(device = 1; device < (int)0x80; device++) {
		int j;
		//if(spd_read_byte(device, 0) < 0 )
		//	continue;
		printk(BIOS_DEBUG, "smbus: %02x", device);
		for(j = 0; j < 256; j++) {
			int status;
			u8 byte;
			status = spd_read_byte(device, j);
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

static void enable_mainboard_devices(void) 
{
	u32 dev;

	pci_conf1_find_device(0x1106, 0x3227, &dev);
	/* Disable GP3 */
	pci_conf1_write_config8(dev, 0x98, 0x00);

	pci_conf1_write_config8(dev, 0x50, 0x88);//disable mc97, sata
	pci_conf1_write_config8(dev, 0x51, 0x1f);
	pci_conf1_write_config8(dev, 0x58, 0x60);
	pci_conf1_write_config8(dev, 0x59, 0x80);
	pci_conf1_write_config8(dev, 0x5b, 0x08);

	pci_conf1_find_device(0x1106, 0x0571, &dev);

	/* Make it respond to IO space */
	pci_conf1_write_config8(dev, 0x04, 0x07);

	/* Compatibility mode addresses */
	//pci_conf1_write_config32(dev, 0x10, 0);
	//pci_conf1_write_config32(dev, 0x14, 0);
	//pci_conf1_write_config32(dev, 0x18, 0);
	//pci_conf1_write_config32(dev, 0x1b, 0);

	/* Native mode base address */
	//pci_conf1_write_config32(dev, 0x20, BUS_MASTER_ADDR | 1);

	pci_conf1_write_config8(dev, 0x40, 0x4b);//was 0x3
	pci_conf1_write_config8(dev, 0x41, 0xf2);
	pci_conf1_write_config8(dev, 0x42, 0x09);
	/* I'll be damned if I know what these do */
	pci_conf1_write_config8(dev, 0x3c, 0xff);//was 0x0e
	pci_conf1_write_config8(dev, 0x3d, 0x00);//was 0x00
}

static void enable_shadow_ram(void) 
{
	u8 shadowreg;

	printk(BIOS_DEBUG, "Enabling shadow ram\n");
	/* Enable shadow ram as normal dram */
	/* 0xc0000-0xcffff */
	pci_conf1_write_config8(PCI_BDF(0, 0, 3), 0x80, 0xff);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x61, 0xff);
	/* 0xd0000-0xdffff */
	pci_conf1_write_config8(PCI_BDF(0, 0, 3), 0x81, 0xff);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x62, 0xff);
	/* 0xe0000-0xeffff */
	pci_conf1_write_config8(PCI_BDF(0, 0, 3), 0x82, 0xff);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x64, 0xff);

	/* 0xf0000-0xfffff */
	shadowreg = pci_conf1_read_config8(PCI_BDF(0, 0, 3), 0x83);
	shadowreg |= 0x30;
	pci_conf1_write_config8(PCI_BDF(0, 0, 3), 0x83, shadowreg);

	/* Do it again for the vlink controller */
	shadowreg = pci_conf1_read_config8(PCI_BDF(0, 0, 7), 0x63);
	shadowreg |= 0x30;
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x63, shadowreg);
}

static void enable_vlink(void)
{
	printk(BIOS_DEBUG, "Enabling Via V-Link\n");

	/* Enable V-Link statically in 8x mode, using Jetway default values */
//40: 14 19 88 80 82 44 00 04 13 b9 88 80 82 44 00 01
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x42, 0x88);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x45, 0x44);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x46, 0x00);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x47, 0x04);
	//pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x48, 0x13);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x4b, 0x80);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x4c, 0x82);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x4d, 0x44);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x4e, 0x00);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x4f, 0x01);
//b0: 05 01 00 83 35 66 66 64 45 98 77 11 00 00 00 00
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0xb4, 0x35);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0xb5, 0x66);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0xb6, 0x66);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0xb7, 0x64);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0xb8, 0x45);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0xb9, 0x98);
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0xba, 0x77);

	/* This has to be done last, I think */
	pci_conf1_write_config8(PCI_BDF(0, 0, 7), 0x48, 0x13);
}

int main(void)
{
	struct board_info ctrl[] = {
		{
		.d0f2 = PCI_BDF(0, 0, 2),
		.d0f3 = PCI_BDF(0, 0, 3),
		.d0f4 = PCI_BDF(0, 0, 4),
		.d0f7 = PCI_BDF(0, 0, 7),
		.d1f0 = PCI_BDF(0, 1, 0),
		.spd_channel0 = {0x50},
		},
	};

	printk(BIOS_DEBUG, "In initram.c main()\n");

	enable_vlink();
	enable_mainboard_devices();
	enable_shadow_ram();

	c7_cpu_setup(PCI_BDF(0, 0, 2));

	enable_smbus(SMBUS_IO_BASE);
	//find_smbus_devices(0x00, 0xff);
	sdram_set_registers(ctrl);
	sdram_set_spd_registers(ctrl);
	ddr2_sdram_enable(ctrl);
	
	//ram_check(0, 640*1024);
	//ram_check((8 * 1024 * 1024), (16 * 1024 * 1024));

	return 0;
}
