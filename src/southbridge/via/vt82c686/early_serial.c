/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006-2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 Corey Osgood <corey_osgood@verizon.net>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* This has been ported to the VIA VT82C686(A/B) from the SMSC FDC37M60x
 * by Corey Osgood. See vt82c686.h for more information. */

#include <arch/io.h>
#include <device/pci_ids.h>
#include "vt82c686.h"

#define SIO_INDEX	0x3f0
#define SIO_DATA	0x3f1

/**
 * Configure the chip by writing the byte 'value' into the register
 * specified by 'index'.
 *
 * @param index The index of the register to modify.
 * @param value The value to write into the register.
 */
static void vt82c686_sio_write(uint8_t index, uint8_t value)
{
	outb(index, SIO_INDEX);
	outb(value, SIO_DATA);
}

/**
 * Enable the serial port(s) of the VT82C686(A/B) Super I/O chip.
 *
 * @param dev TODO
 * @param iobase TODO
 */
static void vt82c686_enable_serial(device_t dev, unsigned iobase)
{
	uint8_t reg;
	device_t sbdev;

	/* TODO: Use info from 'dev' and 'iobase'. */
	/* TODO: Only enable one serial port (depending on config) or both? */

	/* (1) Enter configuration mode (set Function 0 Rx85[1] = 1). */

	/* Find the southbridge. Die upon error. */
	sbdev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
					 PCI_DEVICE_ID_VIA_82C686), 0);
	// sbdev = PCI_DEV(0, 7, 0);
	if (sbdev == PCI_DEV_INVALID) {
		/* Serial output is not yet working at this point, but
		 * die() emits the POST code 0xff and halts the CPU, too. */
		die("Southbridge not found.\n");
	}

	/* Enable Super-I/O (bit 0) and Super-I/O Configuration (bit 1). */
	reg = pci_read_config8(sbdev, 0x85);
	pci_write_config8(sbdev, 0x85, reg | 0x3);	/* Set bits 0 and 1. */

	/* (2) Configure the chip. */

	/* Enable serial port 1 (set bit 2) and 2 (set bit 3). */
	vt82c686_sio_write(VT82C686_FS, 0xf);

	// vt82c686_sio_write(VT82C686_POWER, 0x00);	/* No powerdown */
	// vt82c686_sio_write(VT82C686_SP_CTRL, 0x00);	/* Normal operation */
	vt82c686_sio_write(VT82C686_SP1, 0xfe);		/* SP1: 0x3f8 */
	vt82c686_sio_write(VT82C686_SP2, 0xbe);		/* SP2: 0x2f8 */

	/* Enable high speed on serial port 1 (set bit 6) and 2 (set bit 7). */
	vt82c686_sio_write(VT82C686_SP_CFG, 0xc0);

	/* (3) Exit configuration mode (set Function 0 Rx85[1] = 0). */
	reg = pci_read_config8(sbdev, 0x85);
	pci_write_config8(sbdev, 0x85, reg & 0xfd);	/* Clear bit 1. */
}

