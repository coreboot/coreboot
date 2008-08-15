/*
 * AMD 8111 "southbridge"
 * This file is part of the coreboot project.
 * Copyright (C) 2004-2005 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> and Jason Schildt for Linux Networx)
 * Copyright (C) 2005-7 YingHai Lu
 * Copyright (C) 2005 Ollie Lo
 * Copyright (C) 2005-2007 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "amd8111.h"
#include "amd8111_smbus.h"

#define SMBUS_IO_BASE 0x0f00

void enable_smbus(void)
{
	u32 bdf;
	u8 enable;

	/* this 746b is the ACPI device. This is from original code. It's weird however. */
	pci_locate_device_on_bus(0, PCI_VENDOR_ID_AMD, 0x746b, &dev);
	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\r\n");
	}

	pci_conf1_write_config32(dev, 0x58, SMBUS_IO_BASE | 1);
	enable = pci_conf1_read_config8(dev, 0x41);
	pci_conf1_write_config8(dev, 0x41, enable | (1 << 7));

	/* check that we can see the smbus controller I/O. */
	if (inw(SMBUS_IO_BASE)==0xFF){
		die("SMBUS controller I/O not found\n");
	}

	/* clear any lingering errors, so the transaction will run */
	outw(inw(SMBUS_IO_BASE + SMBGSTATUS), SMBUS_IO_BASE + SMBGSTATUS);
	printk(BIOS_SPEW, "SMBus controller enabled\r\n");
}

int smbus_wait_until_ready(u16 smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		u16 val;
		smbus_delay();
		val = inw(smbus_io_base + SMBGSTATUS);
		if ((val & 0x800) == 0) {
			break;
		}
		if(loops == (SMBUS_TIMEOUT / 2)) {
			outw(inw(smbus_io_base + SMBGSTATUS), 
				smbus_io_base + SMBGSTATUS);
		}
	} while(--loops);
	return loops?0:SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

int smbus_wait_until_done(u16 smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		u16 val;
		smbus_delay();
		
		val = inw(smbus_io_base + SMBGSTATUS);
		if (((val & 0x8) == 0) | ((val & 0x0037) != 0)) {
			break;
		}
	} while(--loops);
	return loops?0:SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
}

int do_smbus_recv_byte(u16 smbus_io_base, u16 device)
{
	u16 global_status_register;
	u8 byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(0, smbus_io_base + SMBHSTCMD);
	/* set up for a send byte */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x1), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* set the data word...*/
	outw(0, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	/* read results of transaction */
	byte = inw(smbus_io_base + SMBHSTDAT) & 0xff;

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return byte;
}

int do_smbus_send_byte(u16 smbus_io_base, u16 device, u16 value)
{
	u16 global_status_register;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(0, smbus_io_base + SMBHSTCMD);
	/* set up for a send byte */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x1), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* set the data word...*/
	outw(value, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}
	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return 0;
}


int do_smbus_read_byte(u16 smbus_io_base, u16 device, u8 address)
{
	u16 global_status_register;
	u8 byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(address & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a byte data read */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x2), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* clear the data word...*/
	outw(0, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	/* read results of transaction */
	byte = inw(smbus_io_base + SMBHSTDAT) & 0xff;

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return byte;
}

int do_smbus_write_byte(u16 smbus_io_base, u16 device, u8 address, u8 val)
{
	u16 global_status_register;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);
	outb(address & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a byte data write */ /* FIXME */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x2), smbus_io_base + SMBGCTL);
	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* write the data word...*/
	outw(val, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}
	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return 0;
}



int smbus_recv_byte(u16 device)
{
	return do_smbus_recv_byte(SMBUS_IO_BASE, device);
}

int smbus_send_byte(u16 device, u8 val)
{
	return do_smbus_send_byte(SMBUS_IO_BASE, device, val);
}

int smbus_read_byte(u16 device, u16 address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}

int smbus_write_byte(u16 device, u16 address, u8 val)
{
	return do_smbus_write_byte(SMBUS_IO_BASE, device, address, val);
}

