/*
 * $Header$
 *
 * lpc47m10x_early_serial.c: Pre-RAM driver for SMSC LPC47M10X2 Super I/O chip
 * derived from lpc47n217
 *
 * Copyright (C) 2005 Digital Design Corporation
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * $Log$
 *
 */

#include <arch/romcc_io.h>
#include "lpc47m10x.h"

//----------------------------------------------------------------------------------
// Function:    	pnp_enter_conf_state
// Parameters:  	dev - high 8 bits = Super I/O port
// Return Value:	None
// Description: 	Enable access to the LPC47M10X2's configuration registers.
//
static inline void pnp_enter_conf_state(device_t dev) {
	unsigned port = dev>>8;
    outb(0x55, port);
}

//----------------------------------------------------------------------------------
// Function:    	pnp_exit_conf_state
// Parameters:  	dev - high 8 bits = Super I/O port
// Return Value:	None
// Description: 	Disable access to the LPC47M10X2's configuration registers.
//
static void pnp_exit_conf_state(device_t dev) {
	unsigned port = dev>>8;
    outb(0xaa, port);
}

//----------------------------------------------------------------------------------
// Function:    	lpc47b272_enable_serial
// Parameters:  	dev - high 8 bits = Super I/O port, 
//						  low 8 bits = logical device number (per lpc47b272.h)
//					iobase - processor I/O port address to assign to this serial device
// Return Value:	bool
// Description: 	Configure the base I/O port of the specified serial device
//					and enable the serial device.
//
static void lpc47b272_enable_serial(device_t dev, unsigned iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}
