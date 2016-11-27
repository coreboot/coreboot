/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
 */

#include <reset.h>
#include "mcp55.h"

void soft_reset(void)
{
	set_bios_reset();
	/* link reset */
	outb(0x02, 0x0cf9);
	outb(0x06, 0x0cf9);
}

void hard_reset(void)
{
	set_bios_reset();

	/* full reset */
	outb(0x0a, 0x0cf9);
	outb(0x0e, 0x0cf9);
}

void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn)
{
	/* The default value for MCP55 is good. */
	/* Set VFSMAF (VID/FID System Management Action Field) to 2. */
}
