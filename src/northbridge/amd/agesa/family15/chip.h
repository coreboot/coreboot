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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _NB_AGESA_CHIP_H_
#define _NB_AGESA_CHIP_H_

struct northbridge_amd_agesa_family15_config
{
	/*
	 * Here are a couple of examples of how this would be put into the
	 * devicetree.cb file.  Note the array is oversized to support different
	 * configurations of server boards.
	 * This should be placed after the device pci 18.x statements.
	 *
	 * Example: AMD Dinar
	 * register "spdAddrLookup" = "
	 * { // Use 8-bit SPD addresses here
	 * 	{ {0xAC, 0xAE}, {0xA8, 0xAA}, {0xA4, 0xA6}, {0xA0, 0xA2}, }, // socket 0 - Channel 0-3
	 * 	{ {0xAC, 0xAE}, {0xA8, 0xAA}, {0xA4, 0xA6}, {0xA0, 0xA2}, }, // socket 1 - Channel 0-3
	 * }"
	 * Example: Tyan S8226
	 * register "spdAddrLookup" = "
	 * { // Use 8-bit SPD addresses here
	 *  { {0xA0, 0xA4, 0xA8}, {0xA2, 0xA6, 0xAA}, }, // socket 0
	 *  { {0xA0, 0xA4, 0xA8}, {0xA2, 0xA6, 0xAA}, }, // socket 1
	 * }"
	 *
	 */

	u8 spdAddrLookup[8][4][4];
};

#endif
