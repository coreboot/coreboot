/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 * (Written by Nico Huber <nico.huber@secunet.com> for secunet)
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

#include <stdint.h>
#include <delay.h>
#include <console/console.h>
#include "gm45.h"

static const int ddr3_lookup_schedule[6][2] = {
	{ 0, 1 }, { 2, 3 }, { 4, 5 }, { 4, 5 }, { 6, 7 }, { 6, 7 }
};
/* Look-up table:
 *   a1step X idx X (group || pull-up/-down)
 */
static const u8 ddr3_lut[2][64][8] = {
	{	/* Stepping B3 and below. */
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   8,  8,  3,  3,  3,  3,  5,  7  },
		{   9,  9,  3,  3,  3,  3,  6,  7  },
		{   9,  9,  3,  3,  3,  3,  6,  7  },
		{  10, 10,  3,  3,  3,  3,  7,  8  },
		{  11, 10,  3,  3,  3,  3,  7,  8  },
		{  12, 11,  3,  3,  3,  3,  8,  9  },
		{  13, 11,  3,  3,  3,  3,  9,  9  },
		{  14, 12,  3,  3,  3,  3,  9, 10  },
		{  15, 13,  3,  3,  3,  3,  9, 10  },
		{  16, 14,  3,  3,  3,  3,  9, 11  },
		{  18, 16,  3,  3,  3,  3, 10, 12  },
		{  20, 18,  4,  3,  4,  4, 10, 12  },
		{  22, 22,  4,  4,  4,  4, 11, 12  },
		{  24, 24,  4,  4,  4,  4, 11, 12  },
		{  28, 26,  4,  4,  4,  4, 12, 12  },
		{  32, 28,  5,  4,  5,  5, 12, 12  },
		{  36, 32,  5,  5,  5,  5, 13, 13  },
		{  40, 36,  5,  5,  5,  5, 14, 13  },
		{  43, 40,  5,  5,  5,  5, 15, 14  },
		{  43, 43,  5,  5,  6,  5, 15, 14  },
		{  43, 43,  6,  5,  6,  5, 15, 15  },
		{  43, 43,  6,  5,  6,  6, 15, 15  },
		{  43, 43,  6,  6,  6,  6, 15, 15  },
		{  43, 43,  6,  6,  7,  6, 15, 15  },
		{  43, 43,  7,  6,  7,  6, 15, 15  },
		{  43, 43,  7,  7,  7,  7, 15, 15  },
		{  43, 43,  7,  7,  7,  7, 15, 15  },
		{  43, 43,  7,  7,  7,  7, 15, 15  },
		{  43, 43,  8,  7,  8,  7, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
	},
	{	/* Stepping A1 and above. */
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   8,  8,  3,  3,  3,  3,  5,  5  },
		{   9,  9,  3,  3,  3,  3,  6,  6  },
		{   9,  9,  3,  3,  3,  3,  6,  6  },
		{  10, 10,  3,  3,  3,  3,  7,  7  },
		{  10, 10,  3,  3,  3,  3,  7,  7  },
		{  12, 11,  3,  3,  3,  3,  8,  8  },
		{  13, 11,  3,  3,  3,  3,  9,  9  },
		{  14, 12,  3,  3,  3,  3,  9,  9  },
		{  15, 13,  3,  3,  3,  3,  9,  9  },
		{  16, 14,  3,  3,  3,  3,  9,  9  },
		{  18, 16,  3,  3,  3,  3, 10, 10  },
		{  20, 18,  4,  3,  4,  4, 10, 10  },
		{  22, 22,  4,  4,  4,  4, 11, 11  },
		{  24, 24,  4,  4,  4,  4, 11, 11  },
		{  28, 26,  4,  4,  4,  4, 12, 12  },
		{  32, 28,  5,  4,  5,  5, 12, 12  },
		{  36, 32,  5,  5,  5,  5, 13, 13  },
		{  40, 36,  5,  5,  5,  5, 14, 14  },
		{  43, 40,  5,  5,  5,  5, 15, 15  },
		{  43, 43,  5,  5,  6,  5, 15, 15  },
		{  43, 43,  6,  5,  6,  5, 15, 15  },
		{  43, 43,  6,  5,  6,  6, 15, 15  },
		{  43, 43,  6,  6,  6,  6, 15, 15  },
		{  43, 43,  6,  6,  7,  6, 15, 15  },
		{  43, 43,  7,  6,  7,  6, 15, 15  },
		{  43, 43,  7,  7,  7,  7, 15, 15  },
		{  43, 43,  7,  7,  7,  7, 15, 15  },
		{  43, 43,  7,  7,  7,  7, 15, 15  },
		{  43, 43,  8,  7,  8,  7, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
		{  43, 43,  8,  8,  8,  8, 15, 15  },
	}
};
static void lookup_and_write(const int a1step,
				    const int row, const int col,
				    unsigned int mchbar)
{
	int i;

	/* Write 4 32-bit registers, 4 values each. */
	for (i = row; i < row + 16; i += 4) {
		MCHBAR32(mchbar) =
			((ddr3_lut[a1step][i + 0][col] & 0x3f) <<  0) |
			((ddr3_lut[a1step][i + 1][col] & 0x3f) <<  8) |
			((ddr3_lut[a1step][i + 2][col] & 0x3f) << 16) |
			((ddr3_lut[a1step][i + 3][col] & 0x3f) << 24);
		mchbar += 4;
	}
}
void raminit_rcomp_calibration(const stepping_t stepping) {
	const int a1step = stepping >= STEPPING_CONVERSION_A1;

	int i;

	enum {
		PULL_UP = 0,
		PULL_DOWN = 1,
	};
	/* channel X group X pull-up/-down */
	char lut_idx[2][6][2];
	for (i = 0; i < 2 * 6 * 2; ++i)
		((char *)lut_idx)[i] = -1;

	MCHBAR32(0x400) |= (1 << 2);
	MCHBAR32(0x418) |= (1 << 17);
	MCHBAR32(0x40c) &= ~(1 << 23);
	MCHBAR32(0x41c) &= ~((1 << 7) | (1 << 3));
	MCHBAR32(0x400) |= 1;

	/* Read lookup indices. */
	for (i = 0; i < 12; ++i) {
		do {
			MCHBAR32(0x400) |= (1 << 3);
			udelay(10);
			MCHBAR32(0x400) &= ~(1 << 3);
		} while ((MCHBAR32(0x530) & 0x7) != 0x4);
		u32 reg = MCHBAR32(0x400);
		const unsigned int group = (reg >> 13) & 0x7;
		const unsigned int channel = (reg >> 12) & 0x1;
		if (group > 5)
			break;
		reg = MCHBAR32(0x518);
		lut_idx[channel][group][PULL_UP] = (reg >> 24) & 0x7f;
		lut_idx[channel][group][PULL_DOWN] = (reg >> 16) & 0x7f;
	}
	/* Cleanup? */
	MCHBAR32(0x400) |= (1 << 3);
	udelay(10);
	MCHBAR32(0x400) &= ~(1 << 3);
	MCHBAR32(0x400) &= ~(1 << 2);

	/* Check for consistency. */
	for (i = 0; i < 2 * 6 * 2; ++i) {
		const char idx = ((char *)lut_idx)[i];
		if ((idx < 7) || (idx > 55))
			die("Bad RCOMP calibration lookup index.\n");
	}

	/* Lookup values and fill registers. */
	int channel, group, pu_pd;
	unsigned int mchbar = 0x0680;
	for (channel = 0; channel < 2; ++channel) {
		for (group = 0; group < 6; ++group) {
			for (pu_pd = PULL_DOWN; pu_pd >= PULL_UP; --pu_pd) {
				lookup_and_write(
					a1step,
					lut_idx[channel][group][pu_pd] - 7,
					ddr3_lookup_schedule[group][pu_pd],
					mchbar);
				mchbar += 0x0018;
			}
			mchbar += 0x0010;
			/* Channel B knows only the first two groups. */
			if ((1 == channel) && (1 == group))
				break;
		}
		mchbar += 0x0040;
	}
}

