/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <device/pci_ids.h>
#include "early_vx900.h"

#include <arch/io.h>
#include <arch/romcc_io.h>
#include <console/console.h>
#include <devices/dram/dram.h>

__attribute__((unused))
static void smbus_delays(int delays)
{
	while(delays--) __smbus_delay();
}


/**
 * Read a byte from the SMBus.
 *
 * @param dimm The address location of the DIMM on the SMBus.
 * @param offset The offset the data is located at.
 */
u8 __smbus_read_byte(u8 dimm, u8 offset, u16 __smbus_io_base)
{
	u8 val;

	/* Initialize SMBUS sequence */
	smbus_reset();
	/* Clear host data port. */
	outb(0x00, SMBHSTDAT0);

	smbus_wait_until_ready();
	smbus_delays(50);

	/* Actual addr to reg format. */
	dimm = (dimm << 1);
	dimm |= 1; /* read command */
	outb(dimm, SMBXMITADD);
	outb(offset, SMBHSTCMD);
	/* Start transaction, byte data read. */
	outb(0x48, SMBHSTCTL);
	smbus_wait_until_ready();

	val = inb(SMBHSTDAT0);
	return val;
}

void enable_smbus(void)
{
	device_t dev;
	u8 reg8;
	u16 __smbus_io_base = SMBUS_IO_BASE;

	/* Locate the Power Management control */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX900_LPC), 0);

	if (dev == PCI_DEV_INVALID) {
		die("Power Management Controller not found\n");
	}

	/*
	 * To use SMBus to manage devices on the system board, it is a must to
	 * enable SMBus function by setting
	 * PMU_RXD2[0] (SMBus Controller Enable) to 1.
	 * And set PMU_RXD0 and PMU_RXD1 (SMBus I/O Base) to an appropriate
	 * I/O port address, so that all registers in SMBus I/O port can be
	 * accessed.
	 */

	reg8 = pci_read_config8(dev, 0xd2);
	/* Enable SMBus controller */
	reg8 |= 1;
	/* Set SMBUS clock from 128k source */
	reg8 |= 1<<2;
	pci_write_config8(dev, 0xd2, reg8);

	reg8 = pci_read_config8(dev, 0x94);
	/* SMBUS clock from divider of 14.318 MHz */
	reg8 &= ~(1<<7);
	pci_write_config8(dev, 0x94, reg8);

	/* Set SMBus IO base */
	pci_write_config16(dev, 0xd0, SMBUS_IO_BASE);

	/*
	 * Initialize the SMBus sequence:
	 */
	/* Clear SMBus host status register */
	smbus_reset();
	/* Clear SMBus host data 0 register */
	outb(0x00, SMBHSTDAT0);

	/* Wait for SMBUS */
	smbus_wait_until_ready();

}

void spd_read(u8 addr, spd_raw_data spd)
{
	u8 reg;
	int i, regs;
	reg = smbus_read_byte(addr, 2);
	if(reg != 0x0b)
	{
		printk(BIOS_DEBUG, "SMBUS device %x not a DDR3 module\n", addr);
		spd[2] = 0;
		return;
	}

	reg = smbus_read_byte(addr, 0);
	reg &= 0xf;
	if (reg == 0x3) {
		regs = 256;
	} else if (reg == 0x2) {
		regs = 176;
	} else if (reg == 0x1) {
		regs = 128;
	} else {
		printk(BIOS_INFO, "No DIMM present at %x\n", addr);
		spd[2] = 0;
		return;
	}
	printk(BIOS_DEBUG, "SPD Data for DIMM %x \n", addr);
	for (i = 0; i < regs; i++) {
		reg = smbus_read_byte(addr, i);
		//printk(BIOS_DEBUG, "  Offset %u  = 0x%x \n", i, reg );
		spd[i] = reg;
	}
}

void dump_spd_data(void)
{
	int dimm, offset, regs;
	unsigned int reg;
	spd_raw_data spd;
	dimm_attr dimmx;

	for (dimm = 0x50; dimm < 0x52; dimm++) {
		reg = smbus_read_byte(dimm, 2);
		if(reg != 0x0b)
		{
			printk(BIOS_DEBUG,
			       "SMBUS device %x not a DDR3 module\n", dimm);
			continue;
		}

		reg = smbus_read_byte(dimm, 0);
		reg &= 0xf;
		if (reg == 0x3) {
			regs = 256;
		} else if (reg == 0x2) {
			regs = 176;
		} else if (reg == 0x1) {
			regs = 128;
		} else {
			printk(BIOS_INFO, "No DIMM present at %x\n", dimm);
			regs = 0;
			continue;
		}
		printk(BIOS_DEBUG, "SPD Data for DIMM %x \n", dimm);
		for (offset = 0; offset < regs; offset++) {
			reg = smbus_read_byte(dimm, offset);
			//printk(BIOS_DEBUG, "  Offset %u  = 0x%x \n", offset, reg );
			spd[offset] = reg;
		}

		spd_decode_ddr3(&dimmx, spd);
		dram_print_spd_ddr3(&dimmx);

	}
}

