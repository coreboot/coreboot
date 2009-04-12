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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/romcc_io.h>
#include "it8712f.h"

/* The base address is 0x2e or 0x4e, depending on config bytes. */
#define SIO_BASE                     0x2e
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     SIO_BASE+1

/* Global configuration registers. */
#define IT8712F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8712F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8712F_CONFIG_REG_CONFIGSEL 0x22 /* Configuration Select. */
#define IT8712F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8712F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. */
#define IT8712F_CONFIG_REG_MFC       0x2a /* Multi-function control */
#define IT8712F_CONFIG_REG_WATCHDOG  0x72 /* Watchdog control. */

#define IT8712F_CONFIGURATION_PORT   0x2e /* Write-only. */

/* The content of IT8712F_CONFIG_REG_LDN (index 0x07) must be set to the
   LDN the register belongs to, before you can access the register. */
static void it8712f_sio_write(uint8_t ldn, uint8_t index, uint8_t value)
{
	outb(IT8712F_CONFIG_REG_LDN, SIO_BASE);
	outb(ldn, SIO_DATA);
	outb(index, SIO_BASE);
	outb(value, SIO_DATA);
}


static void it8712f_enter_conf(void)
{
	/*  Enter the configuration state (MB PnP mode). */

	/* Perform MB PnP setup to put the SIO chip at 0x2e. */
	/* Base address 0x2e: 0x87 0x01 0x55 0x55. */
	/* Base address 0x4e: 0x87 0x01 0x55 0xaa. */
	outb(0x87, IT8712F_CONFIGURATION_PORT);
	outb(0x01, IT8712F_CONFIGURATION_PORT);
	outb(0x55, IT8712F_CONFIGURATION_PORT);
	outb(0x55, IT8712F_CONFIGURATION_PORT);
}

static void it8712f_exit_conf(void)
{
	/* Exit the configuration state (MB PnP mode). */
	it8712f_sio_write(0x00, IT8712F_CONFIG_REG_CC, 0x02);
}


static void it8712f_24mhz_clkin(void)
{
	it8712f_enter_conf();

	/* Select 24MHz CLKIN (48MHZ default)*/
	it8712f_sio_write(0x00, IT8712F_CONFIG_REG_CLOCKSEL, 0x1);

	it8712f_exit_conf();

}

static void it8712f_enable_3vsbsw(void) {

	/* We need to set enable 3VSBSW#, this was documented only in IT8712F_V0.9.2!
	 LDN 7, reg 0x2a - needed for S3, or memory power will be cut off.
	 Enable 3VSBSW#. (For System Suspend-to-RAM)
	 0: 3VSBSW# will be always inactive.
	 1: 3VSBSW# enabled. It will be (NOT SUSB#) NAND SUSC#.
	*/

	it8712f_enter_conf();
	it8712f_sio_write(0x07, IT8712F_CONFIG_REG_MFC, 0x80);
	it8712f_exit_conf();
}


static void it8712f_kill_watchdog(void)
{
	it8712f_enter_conf();

	/* Kill the Watchdog */
	it8712f_sio_write(0x07, IT8712F_CONFIG_REG_WATCHDOG, 0x00);

	it8712f_exit_conf();
}

/* Enable the peripheral devices on the IT8712F Super I/O chip. */
static void it8712f_enable_serial(device_t dev, unsigned iobase)
{

	/* (1) Enter the configuration state (MB PnP mode). */
	it8712f_enter_conf();

	/* (2) Modify the data of configuration registers. */

	/* Select the chip to configure (if there's more than one).
           Set bit 7 to select JP3=1, clear bit 7 to select JP3=0.
           If this register is not written, both chips are configured. */
	/* it8712f_sio_write(0x00, IT8712F_CONFIG_REG_CONFIGSEL, 0x00); */

	/* Enable serial port(s). */
	it8712f_sio_write(IT8712F_SP1,  0x30, 0x1); /* Serial port 1 */
	it8712f_sio_write(IT8712F_SP2,  0x30, 0x1); /* Serial port 2 */

	/* Clear software suspend mode (clear bit 0). TODO: Needed? */
	/* it8712f_sio_write(0x00, IT8712F_CONFIG_REG_SWSUSP, 0x00); */

	/* (3) Exit the configuration state (MB PnP mode). */
	it8712f_exit_conf();
}
