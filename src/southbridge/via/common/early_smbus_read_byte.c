/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "via_early_smbus.h"

/**
 * \brief Read a byte from the SMBus.
 *
 * @param smbus_dev The base SMBus IO port
 * @param addr The address location of the DIMM on the SMBus.
 * @param offset The offset the data is located at.
 */
u8 smbus_read_byte(u32 smbus_dev, u8 addr, u8 offset)
{
	u8 val;

	/* Initialize SMBus sequence */
	smbus_reset(smbus_dev);
	/* Clear host data port. */
	outb(0x00, SMBHSTDAT0(smbus_dev));

	smbus_wait_until_ready(smbus_dev);

	/* Actual addr to reg format. */
	addr = (addr << 1);
	addr |= 1;		/* read command */
	outb(addr, SMBXMITADD(smbus_dev));
	outb(offset, SMBHSTCMD(smbus_dev));
	/* Start transaction, byte data read. */
	outb(0x48, SMBHSTCTL(smbus_dev));
	smbus_wait_until_ready(smbus_dev);

	val = inb(SMBHSTDAT0(smbus_dev));
	return val;
}
