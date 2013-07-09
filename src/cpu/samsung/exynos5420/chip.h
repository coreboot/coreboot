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

#ifndef CPU_SAMSUNG_EXYNOS5420_H
#define CPU_SAMSUNG_EXYNOS5420_H

struct cpu_samsung_exynos5420_config {
	/* special magic numbers! */
	int clkval_f;
	int upper_margin;
	int lower_margin;
	int vsync;
	int left_margin;
	int right_margin;
	int hsync;

	int xres;
	int yres;
	int bpp;

	u32 lcdbase;
};

#endif /* CPU_SAMSUNG_EXYNOS5420_H */
