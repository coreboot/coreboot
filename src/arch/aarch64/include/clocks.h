/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* Standard clock speeds */

/*
 * We define some commonly-used clock speeds to avoid error since long
 * numbers are hard to read.
 *
 * The format of the label is
 * CLK_x_yU where:
 *	x is the integer speed
 *	y is the fractional part which can be omitted if 0
 *	U is the units (blank for Hz, K or M for KHz and MHz)
 *
 * Please order the items by increasing Hz
 */
enum {
	CLK_32768	= 32768,
	CLK_20M		= 20000000,
	CLK_24M		= 24000000,
	CLK_144M	= 144000000,
	CLK_216M	= 216000000,
	CLK_300M	= 300000000,
};

