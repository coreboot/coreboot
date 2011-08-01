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
/**
 * @file post_codes.h
 *
 * This file defines the implementations for the functions defined in smbus.h
 * These are a generic SMBUS implementation, which should work with a majority
 * of chipsets.
 * They are marked weak so that they can be overridden by the chipset code if
 * necessary.
 */

#include "smbus.h"

/**
 * \brief Brief delay for SMBUS transactions
 */
__attribute__((weak))
void __smbus_delay(void)
{
	inb(0x80);
}

/**
 * \brief Clear the SMBUS host status register
 */
__attribute__((weak))
void __smbus_reset(u16 __smbus_io_base)
{
	outb(0xdf, SMBHSTSTAT);
}

/**
 * \brief Print an error, should it occur. If no error, just exit.
 *
 * @param host_status The data returned on the host status register after
 *		      a transaction is processed.
 * @param loops The number of times a transaction was attempted.
 * @return	0 if no error occurred
 * 		1 if an error was detected
 */
__attribute__((weak))
int __smbus_print_error(u8 host_status, int loops, u16 __smbus_io_base)
{
	/* Check if there actually was an error. */
	if ((host_status == 0x00 || host_status == 0x40 ||
		host_status == 0x42) && (loops < SMBUS_TIMEOUT))
		return 0;
	
	if (loops >= SMBUS_TIMEOUT)
		printsmbus("SMBus timeout\n");
	if (host_status & (1 << 4))
		printsmbus("Interrupt/SMI# was Failed Bus Transaction\n");
	if (host_status & (1 << 3))
		printsmbus("Bus error\n");
	if (host_status & (1 << 2))
		printsmbus("Device error\n");
	if (host_status & (1 << 1))
		printsmbus("Interrupt/SMI# completed successfully\n");
	if (host_status & (1 << 0))
		printsmbus("Host busy\n");
	return 1;
}

/**
 * \brief Checks if the SMBUS is currently busy with a transaction
 */
__attribute__((weak))
int __smbus_is_busy(u16 __smbus_io_base)
{
	/* Check if bit 0 of the status register is 1 (busy) or 0 (ready) */
	return ( (inb(SMBHSTSTAT) & (1 << 0)) == 1);
}

/**
 * \brief Wait for the SMBUS to become ready to process a new transaction.
 */
__attribute__((weak))
int __smbus_wait_until_ready(u16 __smbus_io_base)
{
	int loops;
	
	printsmbus("Waiting until SMBus ready\n");
	
	/* Loop up to SMBUS_TIMEOUT times, waiting for bit 0 of the
	 * SMBus Host Status register to go to 0, indicating the operation
	 * was completed successfully. I don't remember why I did it this way,
	 * but I think it was because ROMCC was running low on registers */
	loops = 0;
	while (__smbus_is_busy(__smbus_io_base) && loops < SMBUS_TIMEOUT)
		++loops;
	
	return __smbus_print_error(inb(SMBHSTSTAT), loops, __smbus_io_base);
}

/**
 * \brief Read a byte from the SMBUS.
 *
 * @param dimm The address location of the DIMM on the SMBus.
 * @param offset The offset the data is located at.
 */
__attribute__((weak))
u8 __smbus_read_byte(u8 dimm, u8 offset, u16 __smbus_io_base)
{
	u8 val;
	
	/* Initialize SMBUS sequence */
	__smbus_reset(__smbus_io_base);
	/* Clear host data port. */
	outb(0x00, SMBHSTDAT0);
	
	__smbus_wait_until_ready(__smbus_io_base);
	
	/* Actual addr to reg format. */
	dimm = (dimm << 1);
	dimm |= 1; /* read command */
	outb(dimm, SMBXMITADD);
	outb(offset, SMBHSTCMD);
	/* Start transaction, byte data read. */
	outb(0x48, SMBHSTCTL);
	__smbus_wait_until_ready(__smbus_io_base);
	
	val = inb(SMBHSTDAT0);
	return val;
}