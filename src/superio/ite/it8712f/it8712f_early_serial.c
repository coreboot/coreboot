/*
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/romcc_io.h>
#include "it8712f.h"

/* The base address is 0x2e or 0x4e, depending on config bytes. */
#define SIO_BASE                     0x2e
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     SIO_BASE+1

/* Global Configuration Registers. */
#define IT8712F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8712F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8712F_CONFIG_REG_CONFIGSEL 0x22 /* Configuration Select. */
#define IT8712F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8712F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. */

#define IT8712F_CONFIGURATION_PORT   0x2E /* Write-only. */

/* The content of IT8712F_CONFIG_REG_LDN (index 0x07) must be set to the
 * LDN the register belongs to, before you can access the register. */
static void it8712f_sio_write(uint8_t ldn, uint8_t index, uint8_t value)
{
	outb(IT8712F_CONFIG_REG_LDN, SIO_BASE);
	outb(ldn, SIO_DATA);
	outb(index, SIO_BASE);
	outb(value, SIO_DATA);
}

/* Enable the peripheral devices on the IT8712F Super IO chip. */
static void it8712f_enable_serial(device_t dev, unsigned iobase)
{
	/* (1) Enter the configuration state (MB PnP mode). */

	/* Perform MB PnP setup to put the SIO chip at 0x2e. */
	/* Base address 0x2e: 0x87 0x01 0x55 0x55. */
	/* Base address 0x4e: 0x87 0x01 0x55 0xaa. */
	outb(0x87, IT8712F_CONFIGURATION_PORT);
	outb(0x01, IT8712F_CONFIGURATION_PORT);
	outb(0x55, IT8712F_CONFIGURATION_PORT);
	outb(0x55, IT8712F_CONFIGURATION_PORT);

	/* (2) Modify the data of configuration registers. */

	/* Select the chip to configure (if there's more than one).
         * Set bit 7 to select JP3=1, clear bit 7 to select JP3=0.
         * If this register is not written, both chips are configured. */
	/* it8712f_sio_write(0x00, IT8712F_CONFIG_REG_CONFIGSEL, 0x00); */

	/* Enable all devices. */
	it8712f_sio_write(IT8712F_FDC,  0x30, 0x1); /* Floppy */
	it8712f_sio_write(IT8712F_SP1,  0x30, 0x1); /* Serial port 1 */
	it8712f_sio_write(IT8712F_SP2,  0x30, 0x1); /* Serial port 2 */
	it8712f_sio_write(IT8712F_PP,   0x30, 0x1); /* Parallel port */
	it8712f_sio_write(IT8712F_EC,   0x30, 0x1); /* Environment controller */
	it8712f_sio_write(IT8712F_KBCK, 0x30, 0x1); /* Keyboard */
	it8712f_sio_write(IT8712F_KBCM, 0x30, 0x1); /* Mouse */
	it8712f_sio_write(IT8712F_MIDI, 0x30, 0x1); /* MIDI port */
	it8712f_sio_write(IT8712F_GAME, 0x30, 0x1); /* GAME port */
	it8712f_sio_write(IT8712F_IR,   0x30, 0x1); /* Consumer IR */

	/* Select 24MHz/48MHz CLKIN (set/clear bit 0). TODO: Needed? */
	/* it8712f_sio_write(0x00, IT8712F_CONFIG_REG_CLOCKSEL, 0x00); */

	/* Clear software suspend mode (clear bit 0). TODO: Needed? */
	/* it8712f_sio_write(0x00, IT8712F_CONFIG_REG_SWSUSP, 0x00); */

	/* (3) Exit the configuration state (MB PnP mode). */
	it8712f_sio_write(0x00, IT8712F_CONFIG_REG_CC, 0x02); 
}

