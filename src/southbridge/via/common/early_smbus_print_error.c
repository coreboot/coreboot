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
 */

#include "via_early_smbus.h"
#include <console/console.h>

/**
 * \brief Print an error, should it occur. If no error, just exit.
 *
 * @param smbus_dev The base SMBus IO port
 * @param host_status The data returned on the host status register after
 *		      a transaction is processed.
 * @param loops The number of times a transaction was attempted.
 * @return	0 if no error occurred
 *		1 if an error was detected
 */
int smbus_print_error(u32 smbus_dev, u8 host_status, int loops)
{
	/* Check if there actually was an error. */
	if ((host_status == 0x00 || host_status == 0x40 ||
	     host_status == 0x42) && (loops < SMBUS_TIMEOUT))
		return 0;

	if (loops >= SMBUS_TIMEOUT)
		printsmbus("SMBus timeout\n");
	if (host_status & (1 << 4))
		printsmbus("Interrupt/SMI# was Failed Bus Transaction\n");
	if (host_status & (1 << 3))
		printsmbus("Bus error\n");
	if (host_status & (1 << 2))
		printsmbus("Device error\n");
	if (host_status & (1 << 1))
		printsmbus("Interrupt/SMI# completed successfully\n");
	if (host_status & (1 << 0))
		printsmbus("Host busy\n");
	return 1;
}
