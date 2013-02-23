/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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

#include <arch/romcc_io.h>
#include "it8705f.h"

/* The base address is 0x2e or 0x4e, depending on config bytes. */
#define SIO_BASE                     0x2e
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     (SIO_BASE + 1)

/* Global configuration registers. */
#define IT8705F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8705F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8705F_CONFIG_REG_CONFIGSEL 0x22 /* Configuration Select. */

/* WTF? 0x23 and 0x24 are swapped here (when compared to other IT87xx). */
#define IT8705F_CONFIG_REG_CLOCKSEL  0x24 /* Clock Selection, Flash I/F. */
#define IT8705F_CONFIG_REG_SWSUSP    0x23 /* Software Suspend. */

#define IT8705F_CONFIGURATION_PORT   0x2e /* Write-only. */

static void it8705f_sio_write(u8 ldn, u8 index, u8 value)
{
	outb(IT8705F_CONFIG_REG_LDN, SIO_BASE);
	outb(ldn, SIO_DATA);
	outb(index, SIO_BASE);
	outb(value, SIO_DATA);
}

/* Enable the serial port(s). */
static void it8705f_enable_serial(device_t dev, u16 iobase)
{
	/* (1) Enter the configuration state (MB PnP mode). */

	/* Perform MB PnP setup to put the SIO chip at 0x2e. */
	/* Base address 0x2e: 0x87 0x01 0x55 0x55. */
	/* Base address 0x4e: 0x87 0x01 0x55 0xaa. */
	outb(0x87, IT8705F_CONFIGURATION_PORT);
	outb(0x01, IT8705F_CONFIGURATION_PORT);
	outb(0x55, IT8705F_CONFIGURATION_PORT);
	outb(0x55, IT8705F_CONFIGURATION_PORT);

	/* (2) Modify the data of configuration registers. */

	/*
	 * Select the chip to configure (if there's more than one).
	 * Set bit 7 to select JP3=1, clear bit 7 to select JP3=0.
	 * If this register is not written, both chips are configured.
	 */
	/* it8705f_sio_write(0x00, IT8705F_CONFIG_REG_CONFIGSEL, 0x00); */

	/* Enable serial port(s). */
	it8705f_sio_write(IT8705F_SP1, 0x30, 0x1); /* Serial port 1 */
	it8705f_sio_write(IT8705F_SP2, 0x30, 0x1); /* Serial port 2 */

	/* Select 24MHz CLKIN (set bit 0). */
	it8705f_sio_write(0x00, IT8705F_CONFIG_REG_CLOCKSEL, 0x01);

	/* Clear software suspend mode (clear bit 0). TODO: Needed? */
	/* it8705f_sio_write(0x00, IT8705F_CONFIG_REG_SWSUSP, 0x00); */

	/* (3) Exit the configuration state (MB PnP mode). */
	it8705f_sio_write(0x00, IT8705F_CONFIG_REG_CC, 0x02);
}
