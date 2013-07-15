/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.  All Rights Reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

struct drivers_i2c_rtd2132_config {
	/*
	 * LVDS Spread Spectrum Clock
	 *  0x00 = DISABLED
	 *  0x05 = 0.5%
	 *  0x10 = 1.0%
	 *  0x15 = 1.5%
	 */
	u8 sscg_percent;
};
