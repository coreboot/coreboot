/*
 * This file is part of the coreboot project.
 *
 * Copyright 2008 Corey Osgood <corey.osgood@gmail.com>
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

#include <config.h>
#include <device/pnp.h>
#include "f71805f.h"

void f71805f_enable_serial(u8 dev)
{
	u8 serial_ldn;
	u16 serial_iobase;

	/* Serial port info from Kconfig (FTW!). Would be even more 
	 * awesome if we could get 'dev' from Kconfig or dts */
	/* TODO: Get/set serial port speed divisor from Kconfig as well */
#if defined (CONFIG_CONSOLE_SERIAL_COM1) && CONFIG_CONSOLE_SERIAL_COM1
	serial_ldn = F71805F_COM1;
	serial_iobase = 0x3f8;
#elif defined (CONFIG_CONSOLE_SERIAL_COM2) && CONFIG_CONSOLE_SERIAL_COM2
	serial_ldn = F71805F_COM2;
	serial_iobase = 0x2f8;
#else /* No serial console */
	return;
#endif

	rawpnp_enter_ext_func_mode(dev);
	rawpnp_set_logical_device(dev, serial_ldn);
	rawpnp_set_enable(dev, 0);
	rawpnp_set_iobase(dev, PNP_IDX_IO0, serial_iobase);
	rawpnp_set_enable(dev, 1);
	rawpnp_exit_ext_func_mode(dev);
}
