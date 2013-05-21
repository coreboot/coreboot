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
 * \brief Checks if the SMBus is currently busy with a transaction
 *
 * @param smbus_dev The base SMBus IO port
 */
int smbus_is_busy(u32 smbus_dev)
{
	/* Check if bit 0 of the status register is 1 (busy) or 0 (ready) */
	return ((inb(SMBHSTSTAT(smbus_dev)) & (1 << 0)) == 1);
}
