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
#include <spd.h>
#include <arch/x86/pci_ops.h>
#include <device/pci.h>
#include <superio/via/vt1211/vt1211.h>
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

	sdram_set_registers(ctrl);
	sdram_set_spd_registers(ctrl);
	ddr2_sdram_enable(ctrl);
	
	//ram_check(0, 640*1024);
	//ram_check((8 * 1024 * 1024), (16 * 1024 * 1024));

	return 0;
}
