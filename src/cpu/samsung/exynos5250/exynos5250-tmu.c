/*
 * Copyright (C) 2013 Google Inc.
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * This file contains Exynos5250-specific TMU information.
 */

#include <cpu/samsung/exynos5250/exynos-tmu.h>
#include <cpu/samsung/exynos5250/cpu.h>

struct tmu_info exynos5250_tmu_info = {
	.tmu_base = 0x10060000,
	.tmu_mux = 6,
	.data = {
		.ts = {
			.min_val = 25,
			.max_val = 125,
			.start_warning = 95,
			.start_tripping = 105,
			.hardware_tripping = 110,
		},
		.efuse_min_value = 40,
		.efuse_value = 55,
		.efuse_max_value = 100,
		.slope = 0x10008802,
	},
	.dc_value = 25,
};
