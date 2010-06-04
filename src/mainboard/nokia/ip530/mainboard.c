/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Marc Bertens <mbertens@xs4all.nl>
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

#include <device/device.h>
#include "chip.h"
#include <device/pci_def.h>
#include <device/pci.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#define OUTB	outb
#define INB		inb

/*
*	Taken from flashrom project
*	Generic Super I/O helper functions
*/
static uint8_t sio_read(uint16_t port, uint8_t reg)
{
	OUTB( reg, port );
	return ( INB( port + 1 ) );
}

static void sio_write(uint16_t port, uint8_t reg, uint8_t data)
{
	OUTB( reg, port );
	OUTB( data, port + 1 );
	return;
}

static void nokia_ip530_board_enable( device_t dev )
{
	print_debug( "Setting up IP530-Super I/O devices\n");
	sio_write( 0x20, 0x03, 0x80 );
	printk( BIOS_DEBUG, "--Register 0x03 = %X := 0x80\n", sio_read( 0x20, 0x03 ) );
	sio_write( 0x20, 0x22, 0x30 );
	printk( BIOS_DEBUG, "--Register 0x22 = %X := 0x30\n", sio_read( 0x20, 0x22 ) );
	sio_write( 0x20, 0x24, 0x84 );
	printk( BIOS_DEBUG, "--Register 0x24 = %X := 0x84\n", sio_read( 0x20, 0x24 ) );
	return;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("Nokia IP530 Mainboard")
	.enable_dev = nokia_ip530_board_enable,
};
