#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <unistd.h>
#include <stdio.h>

 /* * $Id$*/
 /*
 * 82801Support code by Eric Beiderman of lnxi.com
 */

#if 0
#include <smbus.h>
#include <arch/io.h>
#include <pci.h>
#include <southbridge/intel/82801.h>
#include "82801.h"
#endif

int smbus_io_base;
int smbus_devfn;
#define SMBUS_BUS 0

#define SMBHSTSTAT 0x0
#define SMBHSTCTL  0x2
#define SMBHSTCMD  0x3
#define SMBXMITADD 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBBLKDAT  0x7
#define SMBTRNSADD 0x9
#define SMBSLVDATA 0xa
#define SMLINK_PIN_CTL 0xe
#define SMBUS_PIN_CTL  0xf 

/* Define register settings */
#define HOST_RESET 0xff
#define DIMM_BASE 0xa0        // 1010000 is base for DIMM in SMBus
#define READ_CMD  0x01        // 1 in the 0 bit of SMBHSTADD states to READ

void smbus_setup(void)
{
  pcibios_read_config_dword(SMBUS_BUS, smbus_devfn, 0x20, 
					    &smbus_io_base);
  smbus_io_base &= ~3;
  pcibios_write_config_byte(SMBUS_BUS, smbus_devfn, 0x40, 1);
  pcibios_write_config_word(SMBUS_BUS, smbus_devfn, 0x4, 1);
}

static void smbus_wait_until_ready(void)
{
	volatile unsigned char c;

	c = inb(smbus_io_base + SMBHSTSTAT);
	while((c & 1) == 1) {
		printf("c is 0x%x\n", c);
		c = inb(smbus_io_base + SMBHSTSTAT);
		/* nop */
	}
}

void reset(void)
{
  outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);
  outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);
  outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);
  outb(HOST_RESET, smbus_io_base + SMBHSTSTAT);

  smbus_wait_until_ready();
  printf("After reset status %#x\n", inb(smbus_io_base + SMBHSTSTAT));
}

static void smbus_wait_until_done(void)
{
	unsigned char byte;
	unsigned long giveup;
	do {
		byte = inb(smbus_io_base + SMBHSTSTAT);
		giveup++;
		if (giveup > 1000000)
		  return;
	} while((byte &1) == 1);
	while( (byte & ~((1<<6)|(1<<0))) == 0) {
		byte = inb(smbus_io_base + SMBHSTSTAT);
	}
}


static void smbus_print_error(unsigned char host_status_register)
{

	printf("smbus_error: 0x%02x\n", host_status_register);
	if (host_status_register & (1 << 7)) {
		printf("Byte Done Status\n");
	}
	if (host_status_register & (1 << 6)) {
		printf("In Use Status\n");
	}
	if (host_status_register & (1 << 5)) {
		printf("SMBus Alert Status\n");
	}
	if (host_status_register & (1 << 4)) {
		printf("Interrup/SMI# was Failed Bus Transaction\n");
	}
	if (host_status_register & (1 << 3)) {
		printf("Bus Error\n");
	}
	if (host_status_register & (1 << 2)) {
		printf("Device Error\n");
	}
	if (host_status_register & (1 << 1)) {
		printf("Interrupt/SMI# was Successful Completion\n");
	}
	if (host_status_register & (1 << 0)) {
		printf("Host Busy\n");
	}
}


int smbus_read_byte(unsigned device, unsigned address, unsigned char *result)
{
	unsigned char host_status_register;
	unsigned char byte;

	reset();

	smbus_wait_until_ready();

	/* setup transaction */
	/* disable interrupts */
	outb(inb(smbus_io_base + SMBHSTCTL) & (~1), smbus_io_base + SMBHSTCTL);
	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, smbus_io_base + SMBXMITADD);
	/* set the command/address... */
	outb(address & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a byte data read */
	outb((inb(smbus_io_base + SMBHSTCTL) & 0xE3) | (0x2 << 2), 
	     smbus_io_base + SMBHSTCTL);

	/* clear any lingering errors, so the transaction will run */
	outb(inb(smbus_io_base + SMBHSTSTAT), smbus_io_base + SMBHSTSTAT);

	/* clear the data byte...*/
	outb(0, smbus_io_base + SMBHSTDAT0);

	/* start the command */
	outb((inb(smbus_io_base + SMBHSTCTL) | 0x40), 
	     smbus_io_base + SMBHSTCTL);

	/* poll for transaction completion */
	smbus_wait_until_done();

	host_status_register = inb(smbus_io_base + SMBHSTSTAT);

	/* Ignore the In Use Status... */
	host_status_register &= ~(1 << 6);

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTDAT0);
	smbus_print_error(byte);

	*result = byte;
	return host_status_register != 0x02;
}
/*
 * Scan SPD bus
 *
 *
 * Derived from ...
 * Copyright 2000 Silicon Integrated System Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. SiS 630 Specification
 *
 */


unsigned short acpi_base;

unsigned char
read_spd(unsigned char slot, unsigned char index)
{
	unsigned char value;

	smbus_read_byte(0xA1 + (slot << 1), index, &value);

	return value;
}

main()
{
	unsigned char b;
	unsigned short w;
	int slot;


	/* get io privilege access PCI configuration space */
	if (iopl(3) != 0) {
	  perror("Can not set io priviliage");
	  exit(1);
	}
	// Find the 801
	for (smbus_devfn = 0; smbus_devfn < 256; smbus_devfn++)
	  {
	    unsigned long val;
	    pcibios_read_config_dword(0, smbus_devfn, 0, &val);
	    if (val == 0x24438086)
	      break;
	  }
	
	if (smbus_devfn == 256) {
	  printf("smbus_devfn not found!\n");
	  return(1);
	}

	smbus_setup();
	reset();

	for(slot = 1; slot < 0xff; slot += 2) {
		unsigned char val;
		if (slot == 0x67) {
			slot = 0x71;
			continue;
		}
		smbus_read_byte(slot, 0, &val);
		printf("slot 0x%x val 0x%x\n", slot, val);
	}
	for(slot = 0; slot < 4; slot++) {
	printf("SLOT %d\n", slot);
	printf("Number of bytes used by module manufacturer 0x%02x\n",
	       read_spd(slot, 0x00));

	printf("Memory Type 0x%02x\n",
	       read_spd(slot, 0x02));

	printf("Number of Row Address bits 0x%02x\n",
	       read_spd(slot, 0x03));

	printf("Number of Column Address bits 0x%02x\n",
	       read_spd(slot, 0x04));

	printf("Number of Sides 0x%02x\n",
	       read_spd(slot, 0x05));

	printf("Number of Banks 0x%02x\n",
	       read_spd(slot, 0x11));
	}
}
