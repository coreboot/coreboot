/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _NB_AGESA_CHIP_H_
#define _NB_AGESA_CHIP_H_

struct northbridge_amd_agesa_family14_config
{
	/*
	 * Here is an example of how this would be put into the devicetree.cb file
	 * Note that only Socket 0, Channel 0 is used for the Ontario
	 * (family 14, Fam 0x00-0x0F) parts.
	 * This should be placed after the device pci 18.x statements
	 *
	 * register "spdAddrLookup" = "
	 * { // Use 8-bit SPD addresses here
	 *	{ {0xA0, 0xA2}, {0x00, 0x00}, }, // socket 0 - Channel 0 & 1
	 *	{ {0x00, 0x00}, {0x00, 0x00}, }, // socket 1 - Channel 0 & 1 (Unused)
	 * }"
	 *
	 */

	u8 spdAddrLookup[2][2][4];
};

#endif
