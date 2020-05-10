/* SPDX-License-Identifier: GPL-2.0-only */

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
