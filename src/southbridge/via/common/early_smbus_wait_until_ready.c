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
#include <console/console.h>

/**
 * \brief Wait for the SMBus to become ready to process a new transaction.
 *
 * @param smbus_dev The base SMBus IO port
 */
int smbus_wait_until_ready(u32 smbus_dev)
{
	int loops;

	printsmbus("Waiting until SMBus ready\n");

	/* Loop up to SMBUS_TIMEOUT times, waiting for bit 0 of the
	 * SMBus Host Status register to go to 0, indicating the operation
	 * was completed successfully. I don't remember why I did it this way,
	 * but I think it was because ROMCC was running low on registers */
	loops = 0;
	while (smbus_is_busy(smbus_dev) && loops < SMBUS_TIMEOUT)
		++loops;

	return smbus_print_error(smbus_dev, inb(SMBHSTSTAT(smbus_dev)), loops);
}
