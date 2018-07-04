/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2012 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <device/smbus_def.h>
#include "pch.h"

#define FULL_RW		0x48
#define QUICK_RW	0x44

static void smbus_delay(void)
{
	inb(0xeb);
}

/** \brief block until the SMBus is no longer busy, or it times out
 *
 * \param smbus_base IO base address of the SMBus
 */
static int smbus_wait_until_ready(u16 smbus_base)
{
	unsigned loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(smbus_base + SMBHSTSTAT);
	} while (byte & HSTSTS_HOST_BUSY);
	return loops ? 0 : -1;
}

/** \brief block until the SMBus is no longer busy or in use, or it times out
 *
 * \param smbus_base IO base address of the SMBus
 */
static int smbus_wait_until_done(u16 smbus_base)
{
	unsigned loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(smbus_base + SMBHSTSTAT);
	} while ((byte & HSTSTS_HOST_BUSY) ||
	         (byte & ~(HSTSTS_INUSE_STS | HSTSTS_HOST_BUSY)) == 0);
	return loops ? 0 : -1;
}
/** \brief Sets the SMBus BAR, and configures it to run
 *
 */
void enable_smbus(void)
{
	pci_devfn_t dev;

	/* Set the SMBus device statically. */
	dev = PCI_DEV(0x0, 0x1f, 0x3);

	/* Check to make sure we've got the right device. */
	if (pci_read_config16(dev, 0x0) != 0x8086) {
		die("SMBus controller not found!");
	}

	/* Set SMBus I/O base. */
	pci_write_config32(dev, SMB_BASE, SMBUS_IO_BASE | PCI_BASE_ADDRESS_SPACE_IO);

	/* Set SMBus enable. */
	pci_write_config8(dev, HOSTC, HST_EN);

	/* Set SMBus I/O space enable. */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	/* Disable interrupt generation. */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);

	/* Clear any lingering errors, so transactions can run. */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
	printk(BIOS_DEBUG, "SMBus controller enabled.\n");
}

/** \brief generic smbus helper function to read & write to the smbus
 *
 * \details Configures the SMBus for the transaction, sets up the address
 *          and data bytes, starts the command, waits for the command to
 *          finish, and returns data if the command is a read.
 *
 * \param device The 8-bit device address, with the read / write bit set-up.
 *
 * \param addr_dat For full reads/writes, this contains the address within
 *                 the device of the byte being read/written. For quick writes,
 *                 this contains the data to write.
 *
 * \param data For full writes, this contains the Data to write to the
 *             device.  For all other transactions, this is ignored.
 *
 * \param command Contains the command for a full read/write (0x48) or the
 *                command for a quick read/write (0x44)
 *
 * \return Data read from the device, or -1 if there was an error
 */
static s16 smbus_rw_byte(u8 device, u8 addr_dat, u8 data, u8 command)
{
	u8 global_status_register;

	if (smbus_wait_until_ready(SMBUS_IO_BASE) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/*** Set up transaction ***/

	/* Disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & (~1), SMBUS_IO_BASE + SMBHSTCTL);

	/* Set the device being talked to using supplied device address*/
	outb(device, SMBUS_IO_BASE + SMBXMITADD);

	/* Set the address and data byte */
	outb(addr_dat, SMBUS_IO_BASE + SMBHSTCMD);
	outb(data, SMBUS_IO_BASE + SMBHSTDAT0);

	/* Clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

	/* Start the command */
	outb(command, SMBUS_IO_BASE + SMBHSTCTL);

	/* Poll for transaction completion */
	if (smbus_wait_until_done(SMBUS_IO_BASE) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	global_status_register = inb(SMBUS_IO_BASE + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(HSTSTS_SMBALERT_STS | HSTSTS_INUSE_STS);

	/* Read results - INTR gets set when a command is completed successfully */
	data = inb(SMBUS_IO_BASE + SMBHSTDAT0);
	if (global_status_register != HSTSTS_INTR)
		return SMBUS_ERROR;

	return data;
}

/** \brief Sends an address and writes one byte of data
 *
 * \param device The 7-bit address of the device being written to
 *
 * \param address The address within the device to write the data to
 *
 * \param data The data value to write to the device
 *
 * \return -1 if there was an error
 */
s16 smbus_write_single_byte(u8 device, u8 address, u8 data)
{
	return (smbus_rw_byte(device << 1, address, data, FULL_RW));
}

/** \brief Sends an address and reads one byte of data
 *
 * \param device The 7-bit address of the device being written to
 *
 * \param address The address within the device to write the data to
 *
 * \return Data read from the device, or -1 if there was an error
 */
int smbus_read_byte(unsigned device, unsigned address)
{
	return (smbus_rw_byte((device << 1) | 1, address, 0, FULL_RW));
}

/** \brief Sends one byte of data with no address byte
 *
 * \param device The 7-bit address of the device being written to
 *
 * \param data The data value to write to the device
 *
 * \return -1 if there was an error
 */
s16 smbus_quick_write(u8 device, u8 data)
{
	return (smbus_rw_byte(device << 1, data, 0, QUICK_RW));
}

/** \brief Reads one byte of data without sending an address byte
 *
 * \param device The 7-bit address of the device being written to
 *
 * \return Data read from the device, or -1 if there was an error
 */
s16 smbus_quick_read(u8 device)
{
	return (smbus_rw_byte((device << 1) | 1, 0, 0, QUICK_RW));
}
