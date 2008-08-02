/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2008 Peter Stuge <peter@stuge.se>
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

#include <io.h>
#include <device/pnp.h>
#include "it8716f.h"

static void enter_ext(u8 dev)
{
	outb(0x87, dev);
	outb(0x01, dev);
	outb(0x55, dev);
	outb(0x4e == dev ? 0xaa : 0x55, dev);
}

void it8716f_enable_serial(u8 dev, u8 serial, u16 iobase)
{
	enter_ext(dev);
	rawpnp_set_logical_device(dev, serial);
	rawpnp_set_enable(dev, 0);
	rawpnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	rawpnp_set_enable(dev, 1);
	rawpnp_write_config(dev, 0x02, 0x02);
}
