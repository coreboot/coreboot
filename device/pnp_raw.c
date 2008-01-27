/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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

#include <types.h>
#include <device/pnp.h>
#include <io.h>

/* Very low-level PNP functions, mostly intended for stage 0 or 1 */

/**
 * Enter the PNP extended function mode (a.k.a. "MB PnP" or "config" mode).
 *
 * @param port The device I/O port.
 */
void rawpnp_enter_ext_func_mode(u16 port)
{
	outb(0x87, port);
	outb(0x87, port);
}

/**
 * Exit the PNP extended function mode (a.k.a. "MB PnP" or "config" mode).
 *
 * @param dev The device IO port.
 */
void rawpnp_exit_ext_func_mode(u16 port)
{
	outb(0xaa, port);
}

/**
 * Write an 8 bit value into a PNP configuration register.
 *
 * This is done by writing the register number to the port, and the value
 * into port + 1. This code assumes that the data port is always the
 * config-port plus 1, but luckily this is true for pretty much all devices.
 *
 * @param port The device I/O port.
 * @param reg The register number.
 * @param value The value to write into the specified register.
 */
void rawpnp_write_config(u16 port, u8 reg, u8 value)
{
	outb(reg, port);
	outb(value, port + 1);
}

/**
 * Select a logical device.
 *
 * PNP has up to 16 logical devices. They are selected by writing the
 * logical device number (LDN) to register 0x07.
 *
 * @param dev The device I/O port.
 * @param ldn The logical device (number) which should be selected.
 */
void rawpnp_set_logical_device(u16 port, u8 ldn)
{
	rawpnp_write_config(port, 0x07, ldn);
}

/**
 * Set the enable for the logical device.
 *
 * The enable is at register 0x30. Setting bit zero enables the device.
 * Code must have selected the proper logical device number (LDN) using
 * rawpnp_set_logical_device() beforehand. If enable is non-zero, the device
 * is enabled. If enable is zero, the device is disabled.
 *
 * @param port The device I/O port.
 * @param enable Non-zero enables the device, zero disables it.
 */
void rawpnp_set_enable(u16 port, int enable)
{
	rawpnp_write_config(port, 0x30, enable ? 0x1 : 0x0);
}

/**
 * Set the I/O base for the device.
 *
 * The I/O base is at registers 'index' and 'index + 1', since these are
 * 8 bit registers and the I/O base is 16 bits. Typical values for 'index'
 * are 0x60 or 0x62.
 *
 * Code must have selected the proper logical device (LDN) using
 * rawpnp_set_logical_device() beforehand.
 *
 * @param port The device I/O port.
 * @param index The index port.
 * @param iobase The 16 bit I/O base.
 */
void rawpnp_set_iobase(u16 port, u8 index, u16 iobase)
{
	rawpnp_write_config(port, index + 0, (iobase >> 8) & 0xff); /* MSB */
	rawpnp_write_config(port, index + 1, iobase & 0xff);	    /* LSB */
}
