/* SPDX-License-Identifier: GPL-2.0-or-later */

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
