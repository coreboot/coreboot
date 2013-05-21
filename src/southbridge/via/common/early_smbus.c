/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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
 * @file via_early_sambus.c
 *
 * This file defines the implementations for the functions defined in
 * device/early/smbus.h
 *
 * These implementations work with most via chipsets. Any VIA port should try
 * to use these. Makefile.inc needs to be adapted to link against this file
 * during romstage:
 * @code
 *	romstage-y += ./../../../southbridge/via/common/early_smbus.c
 * @endcode
 *
 * These functions are marked weak in the event that one or more might need to
 * be overridden. This may be the case when, for example, a chipset needs a
 * longer delay for a specific operation.
 */

#include <device/early_smbus.h>

#include <arch/io.h>

/**
 * \brief SMBUS IO ports in relation to the base IO port
 */
#define SMBHSTSTAT(base)		(u16)(u32)base + 0x0
#define SMBSLVSTAT(base)		(u16)(u32)base + 0x1
#define SMBHSTCTL(base)			(u16)(u32)base + 0x2
#define SMBHSTCMD(base)			(u16)(u32)base + 0x3
#define SMBXMITADD(base)		(u16)(u32)base + 0x4
#define SMBHSTDAT0(base)		(u16)(u32)base + 0x5
#define SMBHSTDAT1(base)		(u16)(u32)base + 0x6
#define SMBBLKDAT(base)			(u16)(u32)base + 0x7
#define SMBSLVCTL(base)			(u16)(u32)base + 0x8
#define SMBTRNSADD(base)		(u16)(u32)base + 0x9
#define SMBSLVDATA (base)		(u16)(u32)base + 0xa

#define SMBUS_TIMEOUT		(100*1000*10)

/**
 * \brief Brief delay for SMBUS transactions
 */
void smbus_delay(void)
{
	inb(0x80);
}

/**
 * \brief Clear the SMBUS host status register
 *
 * @param smbus_dev The base SMBUS IO port
 */
__attribute__ ((weak))
void smbus_reset(u32 smbus_dev)
{
	outb(0xdf, SMBHSTSTAT(smbus_dev));
}

/**
 * \brief Print an error, should it occur. If no error, just exit.
 *
 * @param smbus_dev The base SMBUS IO port
 * @param host_status The data returned on the host status register after
 *		      a transaction is processed.
 * @param loops The number of times a transaction was attempted.
 * @return	0 if no error occurred
 * 		1 if an error was detected
 */
__attribute__ ((weak))
int smbus_print_error(u32 smbus_dev, u8 host_status, int loops)
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
 *
 * @param smbus_dev The base SMBUS IO port
 */
__attribute__ ((weak))
int smbus_is_busy(u32 smbus_dev)
{
	/* Check if bit 0 of the status register is 1 (busy) or 0 (ready) */
	return ((inb(SMBHSTSTAT(smbus_dev)) & (1 << 0)) == 1);
}

/**
 * \brief Wait for the SMBUS to become ready to process a new transaction.
 *
 * @param smbus_dev The base SMBUS IO port
 */
__attribute__ ((weak))
int smbus_wait_until_ready(u32 smbus_dev)
{
	int loops;

	printsmbus("Waiting until SMBus ready\n");

	/* Loop up to SMBUS_TIMEOUT times, waiting for bit 0 of the
	 * SMBus Host Status register to go to 0, indicating the operation
	 * was completed successfully. I don't remember why I did it this way,
	 * but I think it was because ROMCC was running low on registers */
	loops = 0;
	while (smbus_is_busy(smbus_dev) && loops < SMBUS_TIMEOUT)
		++loops;

	return smbus_print_error(smbus_dev, inb(SMBHSTSTAT(smbus_dev)), loops);
}

/**
 * \brief Read a byte from the SMBUS.
 *
 * @param smbus_dev The base SMBUS IO port
 * @param addr The address location of the DIMM on the SMBus.
 * @param offset The offset the data is located at.
 */
__attribute__ ((weak))
u8 smbus_read_byte(u32 smbus_dev, u8 addr, u8 offset)
{
	u8 val;

	/* Initialize SMBUS sequence */
	smbus_reset(smbus_dev);
	/* Clear host data port. */
	outb(0x00, SMBHSTDAT0(smbus_dev));

	smbus_wait_until_ready(smbus_dev);

	/* Actual addr to reg format. */
	addr = (addr << 1);
	addr |= 1;		/* read command */
	outb(addr, SMBXMITADD(smbus_dev));
	outb(offset, SMBHSTCMD(smbus_dev));
	/* Start transaction, byte data read. */
	outb(0x48, SMBHSTCTL(smbus_dev));
	smbus_wait_until_ready(smbus_dev);

	val = inb(SMBHSTDAT0(smbus_dev));
	return val;
}
