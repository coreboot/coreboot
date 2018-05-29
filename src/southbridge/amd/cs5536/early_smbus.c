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
 */

#include <arch/io.h>
#include "cs5536.h"
#include "smbus.h"

/* initialization for SMBus Controller */
void cs5536_enable_smbus(void)
{

	if (IS_ENABLED(CONFIG_NO_EARLY_SMBUS))
		return;

	/* Set SCL freq and enable SMB controller */
	/*outb((0x20 << 1) | SMB_CTRL2_ENABLE, smbus_io_base + SMB_CTRL2); */
	outb((0x7F << 1) | SMB_CTRL2_ENABLE, SMBUS_IO_BASE + SMB_CTRL2);

	/* Setup SMBus host controller address to 0xEF */
	outb((0xEF | SMB_ADD_SAEN), SMBUS_IO_BASE + SMB_ADD);

}

int smbus_read_byte(unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}
