/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <bootblock_common.h>
#include "dock.h"

static int dock_err;

void bootblock_mainboard_early_init(void)
{
	/* Minimal setup to detect dock */
	dock_err = pc87382_early();
	if (dock_err == 0)
		dock_connect();
}

void bootblock_mainboard_init(void)
{
	/* Console is not yet initialized in bootblock_mainboard_early_init,
	   so we print the dock information here */
	if (dock_err)
		printk(BIOS_ERR, "DOCK: Failed to init pc87382\n");
	else
		dock_info();
}
