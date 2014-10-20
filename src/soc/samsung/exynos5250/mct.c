/*
 * This file is part of the coreboot project.
 *
 * Copyright 2012 Google Inc.
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

#include <arch/io.h>
#include <soc/clk.h>
#include <stdint.h>

uint64_t mct_raw_value(void)
{
	uint64_t upper = readl(&exynos_mct->g_cnt_u);
	uint64_t lower = readl(&exynos_mct->g_cnt_l);

	return (upper << 32) | lower;
}

void mct_start(void)
{
	writel(readl(&exynos_mct->g_tcon) | (0x1 << 8),
		&exynos_mct->g_tcon);
}
