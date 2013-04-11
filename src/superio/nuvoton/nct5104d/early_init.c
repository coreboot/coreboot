/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#include <arch/io.h>
#include <arch/romcc_io.h>
#include <device/pnp_def.h>
#include "nct5104d.h"

static void pnp_enter_extended_mode(device_t dev)
{
	u16 port = dev >> 8;
	outb(0x87,port);
	outb(0x87,port);
}

static void pnp_exit_extended_mode(device_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa,port);
}

static void nct5104d_enable_serial(device_t dev, u16 iobase)
{
	pnp_enter_extended_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev,0);
	pnp_set_iobase(dev,PNP_IDX_IO0, iobase);
	pnp_set_enable(dev,1);
	pnp_exit_extended_mode(dev);
}
