/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include "cs5536.h"
#include "smbus.h"

/* initialization for SMBus Controller */
static void cs5536_enable_smbus(void)
{

	/* Set SCL freq and enable SMB controller */
	/*outb((0x20 << 1) | SMB_CTRL2_ENABLE, smbus_io_base + SMB_CTRL2); */
	outb((0x7F << 1) | SMB_CTRL2_ENABLE, SMBUS_IO_BASE + SMB_CTRL2);

	/* Setup SMBus host controller address to 0xEF */
	outb((0xEF | SMB_ADD_SAEN), SMBUS_IO_BASE + SMB_ADD);

}

static inline int smbus_read_byte(unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}
