/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Ronald G. Minnich
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/pci_def.h>
#include <console/console.h>

#include "i82801dx.h"

void enable_smbus(void)
{
	device_t dev = PCI_DEV(0x0, 0x1f, 0x3);

	print_debug("SMBus controller enabled\n");
	/* set smbus iobase */
	pci_write_config32(dev, 0x20, SMBUS_IO_BASE | 1);
	/* Set smbus enable */
	pci_write_config8(dev, 0x40, 0x01);
	/* Set smbus iospace enable */
	pci_write_config16(dev, 0x4, 0x01);
	/* Disable interrupt generation */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);
	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
}

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

static int smbus_wait_until_active(void)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(SMBUS_IO_BASE + SMBHSTSTAT);
		if ((val & 1)) {
			break;
		}
	} while (--loops);
	return loops ? 0 : -4;
}

static int smbus_wait_until_ready(void)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(SMBUS_IO_BASE + SMBHSTSTAT);
		if ((val & 1) == 0) {
			break;
		}
		if (loops == (SMBUS_TIMEOUT / 2)) {
			outb(inb(SMBUS_IO_BASE + SMBHSTSTAT),
			     SMBUS_IO_BASE + SMBHSTSTAT);
		}
	} while (--loops);
	return loops ? 0 : -2;
}

static int smbus_wait_until_done(void)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();

		val = inb(SMBUS_IO_BASE + SMBHSTSTAT);
		if ((val & 1) == 0) {
			break;
		}
		if ((val & ~((1 << 6) | (1 << 0))) != 0) {
			break;
		}
	} while (--loops);
	return loops ? 0 : -3;
}

int smbus_read_byte(unsigned device, unsigned address)
{
	unsigned char global_status_register;
	unsigned char byte;

	/* print_err("smbus_read_byte\n"); */
	if (smbus_wait_until_ready() < 0) {
		print_err("SMBUS not ready (-2)\n");
		return -2;
	}

	/* setup transaction */
	/* disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xfe, SMBUS_IO_BASE + SMBHSTCTL);
	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, SMBUS_IO_BASE + SMBXMITADD);
	/* set the command/address... */
	outb(address & 0xFF, SMBUS_IO_BASE + SMBHSTCMD);
	/* set up for a byte data read */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xe3) | (0x2 << 2),
	     SMBUS_IO_BASE + SMBHSTCTL);

	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

	/* clear the data byte... */
	outb(0, SMBUS_IO_BASE + SMBHSTDAT0);

	/* start a byte read, with interrupts disabled */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) | 0x40),
	     SMBUS_IO_BASE + SMBHSTCTL);
	/* poll for it to start */
	if (smbus_wait_until_active() < 0) {
		print_err("SMBUS not active (-4)\n");
		return -4;
	}

	/* poll for transaction completion */
	if (smbus_wait_until_done() < 0) {
		print_err("SMBUS not completed (-3)\n");
		return -3;
	}

	global_status_register = inb(SMBUS_IO_BASE + SMBHSTSTAT) & ~(1 << 6);	/* Ignore the In Use Status... */

	/* read results of transaction */
	byte = inb(SMBUS_IO_BASE + SMBHSTDAT0);

	if (global_status_register != 2) {
		//print_spew("%s: no device (%02x, %02x)\n", __func__, device, address);
		return -1;
	}
	//print_debug("%s: %02x@%02x = %02x\n", __func__, device, address, byte);
	return byte;
}

#if 0
static void smbus_write_byte(unsigned device, unsigned address,
			     unsigned char val)
{
	if (smbus_wait_until_ready() < 0) {
		return;
	}

	/* by LYH */
	outb(0x37, SMBUS_IO_BASE + SMBHSTSTAT);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 0, SMBUS_IO_BASE + SMBHSTADDR);

	/* data to send */
	outb(val, SMBUS_IO_BASE + SMBHSTDAT);

	outb(address & 0xFF, SMBUS_IO_BASE + SMBHSTCMD);

	/* start the command */
	outb(0xa, SMBUS_IO_BASE + SMBHSTCTL);

	/* poll for transaction completion */
	smbus_wait_until_done();
	return;
}
#endif
