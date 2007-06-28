/*
 * This file is part of the LinuxBIOS project.
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

/* very low level pnp manipulation intended for stage 0 or 1 */
/**
  * rawpnp_enter_ext_funct -- Enter the PNP extended functions,
  * i.e. configuration mode 
  * @param dev The device IO port.
  */
void  rawpnp_enter_ext_func_mode(u16 dev) 
{
        outb(0x87, dev);
        outb(0x87, dev);
}

/**
  * Exit the PNP extended functions, i.e. configuration mode
  * @param dev The device IO port. 
  */
void  rawpnp_exit_ext_func_mode(u16 dev) 
{
        outb(0xaa, dev);
}

/**
  * Write a pnp configuration register. This is done by writing
  * the register number to the port, and the value to the 
  * port+1
  * @param dev The device IO port. 
  * @param reg  The register number 
  * @param value The new value. 
  */
void  rawpnp_write_config(u16 dev, u8 reg, u8 value)
{
        outb(reg, dev);
        outb(value,dev + 1);
}

/**
  * Select a logical device. PNP has up to 16 logical devices. 
  * They are selected by writing the device # to register 7.
  * @param dev The device IO port. 
  * @param which Which device
  */
void rawpnp_set_logical_device(u16 dev, u8 which)
{
	rawpnp_write_config(dev, 0x07, which);
}

/**
  * Set the enable for the device. The enable is at register 30. 
  * Setting the low bit enables the device. 
  * Code must have selected the proper device using 
  * rawpnp_set_logical_device. If enable is non-zero, device
  * is enabled. If enable is zero, device is disabled. 
  * @param dev The device IO port. 
  * @param enable non-zero enables the device
  */
void rawpnp_set_enable(u16 dev, int enable)
{
	rawpnp_write_config(dev, 0x30, enable ? 0x1 : 0x0);
}

/**
  * Set the iobase for the device. The iobase is at registers 'index'
  * and 'index + 1', since these are 8 bit registers and iobase is 16 bits. 
  * Code must have selected the proper device using 
  * rawpnp_set_logical_device.
  * @param dev The device IO port. 
  * @param iobase The 16 bit iobase
  */
void rawpnp_set_iobase(u16 dev, u8 index, u16 iobase)
{
        /* Index == 0x60 or 0x62 */
	rawpnp_write_config(dev, index + 0, (iobase >> 8) & 0xff);
	rawpnp_write_config(dev, index + 1, iobase & 0xff);
}

