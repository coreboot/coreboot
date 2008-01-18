/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey_osgood@verizon.net>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <device/pci_ids.h>
#include <spd.h>
#include <stdlib.h>
#include "vt8237r.h"

/**
 * Print an error, should it occur. If no error, just exit.
 *
 * @param host_status The data returned on the host status register after
 *		      a transaction is processed.
 * @param loops The number of times a transaction was attempted.
 */
static void smbus_print_error(u8 host_status, int loops)
{
	/* Check if there actually was an error. */
	if ((host_status == 0x00 || host_status == 0x40 ||
	     host_status == 0x42) && (loops < SMBUS_TIMEOUT))
		return;

	if (loops >= SMBUS_TIMEOUT)
		print_err("SMBus timeout\r\n");
	if (host_status & (1 << 4))
		print_err("Interrupt/SMI# was Failed Bus Transaction\r\n");
	if (host_status & (1 << 3))
		print_err("Bus error\r\n");
	if (host_status & (1 << 2))
		print_err("Device error\r\n");
	if (host_status & (1 << 1))
		print_debug("Interrupt/SMI# completed successfully\r\n");
	if (host_status & (1 << 0))
		print_err("Host busy\r\n");
}

/**
 * Wait for the SMBus to become ready to process the next transaction.
 */
static void smbus_wait_until_ready(void)
{
	int loops;

	PRINT_DEBUG("Waiting until SMBus ready\r\n");

	loops = 0;
	/* Yes, this is a mess, but it's the easiest way to do it. */
	while ((inb(SMBHSTSTAT) & 1) == 1 && loops < SMBUS_TIMEOUT)
		++loops;

	smbus_print_error(inb(SMBHSTSTAT), loops);
}

/**
 * Reset and take ownership of the SMBus.
 */
static void smbus_reset(void)
{
	outb(HOST_RESET, SMBHSTSTAT);

	/* Datasheet says we have to read it to take ownership of SMBus. */
	inb(SMBHSTSTAT);

	PRINT_DEBUG("After reset status: ");
	PRINT_DEBUG_HEX16(inb(SMBHSTSTAT));
	PRINT_DEBUG("\r\n");
}

/**
 * Read a byte from the SMBus.
 *
 * @param dimm The address location of the DIMM on the SMBus.
 * @param offset The offset the data is located at.
 */
u8 smbus_read_byte(u8 dimm, u8 offset)
{
	u8 val;

	PRINT_DEBUG("DIMM ");
	PRINT_DEBUG_HEX16(dimm);
	PRINT_DEBUG(" OFFSET ");
	PRINT_DEBUG_HEX16(offset);
	PRINT_DEBUG("\r\n");

	smbus_reset();

	/* Clear host data port. */
	outb(0x00, SMBHSTDAT0);
	SMBUS_DELAY();
	smbus_wait_until_ready();

	/* Actual addr to reg format. */
	dimm = (dimm << 1);
	dimm |= 1;
	outb(dimm, SMBXMITADD);
	outb(offset, SMBHSTCMD);

	/* Start transaction, byte data read. */
	outb(0x48, SMBHSTCTL);
	SMBUS_DELAY();
	smbus_wait_until_ready();

	val = inb(SMBHSTDAT0);
	PRINT_DEBUG("Read: ");
	PRINT_DEBUG_HEX16(val);
	PRINT_DEBUG("\r\n");

	/* Probably don't have to do this, but it can't hurt. */
	smbus_reset();

	return val;
}

/**
 * Enable the smbus on vt8237r-based systems
 */
void enable_smbus(void)
{
	device_t dev;

	/* Power management controller */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VT8237R_LPC), 0);

	if (dev == PCI_DEV_INVALID)
		die("Power management controller not found\r\n");

	/* 7 = SMBus Clock from RTC 32.768KHz
	 * 5 = Internal PLL reset from susp
	 */
	pci_write_config8(dev, VT8237R_POWER_WELL, 0xa0);

	/* Enable SMBus. */
	pci_write_config16(dev, VT8237R_SMBUS_IO_BASE_REG,
			   VT8237R_SMBUS_IO_BASE | 0x1);

	/* SMBus Host Configuration, enable. */
	pci_write_config8(dev, VT8237R_SMBUS_HOST_CONF, 0x01);

	/* Make it work for I/O. */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	smbus_reset();

	/* Reset the internal pointer. */
	inb(SMBHSTCTL);
}

/**
 * A fixup for some systems that need time for the SMBus to "warm up". This is 
 * needed on some VT823x based systems, where the SMBus spurts out bad data for 
 * a short time after power on. This has been seen on the VIA Epia series and 
 * Jetway J7F2-series. It reads the ID byte from SMBus, looking for 
 * known-good data from a slot/address. Exits on either good data or a timeout.
 *
 * TODO: This should probably go into some global file, but one would need to
 *       be created just for it. If some other chip needs/wants it, we can
 *       worry about it then.
 *
 * @param ctrl The memory controller and SMBus addresses.
 */
void smbus_fixup(const struct mem_controller *ctrl)
{
	int i, ram_slots, current_slot = 0;
	u8 result = 0;

	ram_slots = ARRAY_SIZE(ctrl->channel0);
	if (!ram_slots) {
		print_err("smbus_fixup() thinks there are no RAM slots!\r\n");
		return;
	}

	PRINT_DEBUG("Waiting for SMBus to warm up");

	/*
	 * Bad SPD data should be either 0 or 0xff, but YMMV. So we look for
	 * the ID bytes of SDRAM, DDR, DDR2, and DDR3 (and anything in between).
	 * VT8237R has only been seen on DDR and DDR2 based systems, so far.
	 */
	for (i = 0; (i < SMBUS_TIMEOUT && ((result < SPD_MEMORY_TYPE_SDRAM) ||
				(result > SPD_MEMORY_TYPE_SDRAM_DDR3))); i++) {

		if (current_slot > ram_slots)
			current_slot = 0;

		result = smbus_read_byte(ctrl->channel0[current_slot],
					 SPD_MEMORY_TYPE);
		current_slot++;
		PRINT_DEBUG(".");
	}

	if (i >= SMBUS_TIMEOUT)
		print_err("SMBus timed out while warming up\r\n");
	else
		PRINT_DEBUG("Done\r\n");
}
