/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include "ps8625.h"

#include <device/i2c.h>
#include <stdint.h>
#include <stdlib.h>

struct parade_write {
	uint8_t offset;
	uint8_t reg;
	uint8_t val;
};

static const struct parade_write parade_writes[] = {
	{ 0x02, 0xa1, 0x01 },  /* HPD low */
	 /*
	  * SW setting
	  * [1:0] SW output 1.2V voltage is lower to 96%
	  */
	{ 0x04, 0x14, 0x01 },
	 /*
	  * RCO SS setting
	  * [5:4] = b01 0.5%, b10 1%, b11 1.5%
	  */
	{ 0x04, 0xe3, 0x20 },
	{ 0x04, 0xe2, 0x80 }, /* [7] RCO SS enable */
	 /*
	  *  RPHY Setting
	  * [3:2] CDR tune wait cycle before
	  * measure for fine tune b00: 1us,
	  * 01: 0.5us, 10:2us, 11:4us.
	  */
	{ 0x04, 0x8a, 0x0c },
	{ 0x04, 0x89, 0x08 }, /* [3] RFD always on */
	 /*
	  * CTN lock in/out:
	  * 20000ppm/80000ppm. Lock out 2
	  * times.
	  */
	{ 0x04, 0x71, 0x2d },
	 /*
	  * 2.7G CDR settings
	  * NOF=40LSB for HBR CDR setting
	  */
	{ 0x04, 0x7d, 0x07 },
	{ 0x04, 0x7b, 0x00 },  /* [1:0] Fmin=+4bands */
	{ 0x04, 0x7a, 0xfd },  /* [7:5] DCO_FTRNG=+-40% */
	 /*
	  * 1.62G CDR settings
	  * [5:2]NOF=64LSB [1:0]DCO scale is 2/5
	  */
	{ 0x04, 0xc0, 0x12 },
	{ 0x04, 0xc1, 0x92 },  /* Gitune=-37% */
	{ 0x04, 0xc2, 0x1c },  /* Fbstep=100% */
	{ 0x04, 0x32, 0x80 },  /* [7] LOS signal disable */
	 /*
	  * RPIO Setting
	  * [7:4] LVDS driver bias current :
	  * 75% (250mV swing)
	  */
	{ 0x04, 0x00, 0xb0 },
	 /*
	  * [7:6] Right-bar GPIO output strength is 8mA
	  */
	{ 0x04, 0x15, 0x40 },
	 /* EQ Training State Machine Setting */
	{ 0x04, 0x54, 0x10 },  /* RCO calibration start */
	 /* [4:0] MAX_LANE_COUNT set to one lane */
	{ 0x01, 0x02, 0x81 },
	 /* [4:0] LANE_COUNT_SET set to one lane */
	{ 0x01, 0x21, 0x81 },
	{ 0x00, 0x52, 0x20 },
	{ 0x00, 0xf1, 0x03 },  /* HPD CP toggle enable */
	{ 0x00, 0x62, 0x41 },
	 /* Counter number, add 1ms counter delay */
	{ 0x00, 0xf6, 0x01 },
	 /*
	  * [6]PWM function control by
	  * DPCD0040f[7], default is PWM
	  * block always works.
	  */
	{ 0x00, 0x77, 0x06 },
	 /*
	  * 04h Adjust VTotal tolerance to
	  * fix the 30Hz no display issue
	  */
	{ 0x00, 0x4c, 0x04 },
	 /* DPCD00400='h00, Parade OUI = 'h001cf8 */
	{ 0x01, 0xc0, 0x00 },
	{ 0x01, 0xc1, 0x1c },  /* DPCD00401='h1c */
	{ 0x01, 0xc2, 0xf8 },  /* DPCD00402='hf8 */
	 /*
	  * DPCD403~408 = ASCII code
	  * D2SLV5='h4432534c5635
	  */
	{ 0x01, 0xc3, 0x44 },
	{ 0x01, 0xc4, 0x32 },  /* DPCD404 */
	{ 0x01, 0xc5, 0x53 },  /* DPCD405 */
	{ 0x01, 0xc6, 0x4c },  /* DPCD406 */
	{ 0x01, 0xc7, 0x56 },  /* DPCD407 */
	{ 0x01, 0xc8, 0x35 },  /* DPCD408 */
	 /*
	  * DPCD40A, Initial Code major  revision
	  * '01'
	  */
	{ 0x01, 0xca, 0x01 },
	 /* DPCD40B, Initial Code minor revision '05' */
	{ 0x01, 0xcb, 0x05 },
	 /* DPCD720, Select internal PWM */
	{ 0x01, 0xa5, 0xa0 },
	 /*
	  * FFh for 100% PWM of brightness, 0h for 0%
	  * brightness
	  */
	{ 0x01, 0xa7, 0xff },
	 /*
	  * Set LVDS output as 6bit-VESA mapping,
	  * single LVDS channel
	  */
	{ 0x01, 0xcc, 0x13 },
	 /* Enable SSC set by register */
	{ 0x02, 0xb1, 0x20 },
	 /*
	  * Set SSC enabled and +/-1% central
	  * spreading
	  */
	{ 0x04, 0x10, 0x16 },
	 /* MPU Clock source: LC => RCO */
	{ 0x04, 0x59, 0x60 },
	{ 0x04, 0x54, 0x14 },  /* LC -> RCO */
	{ 0x02, 0xa1, 0x91 }  /* HPD high */
};

void parade_ps8625_bridge_setup(unsigned bus, unsigned chip_base)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(parade_writes); i++) {
		const struct parade_write *w = &parade_writes[i];
		i2c_write(bus, chip_base + w->offset, w->reg, sizeof(w->reg),
			  &w->val, sizeof(w->val));
	}
}
