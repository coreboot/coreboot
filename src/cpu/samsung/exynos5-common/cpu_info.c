/*
 * Copyright (C) 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
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
 */

#include <console/console.h>
#include <common.h>
#include <arch/io.h>

#include <cpu/samsung/exynos5-common/clk.h>
#include <cpu/samsung/exynos5-common/clock.h>
#include <cpu/samsung/exynos5-common/cpu.h>

#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5-common/cpu.h>	/* for EXYNOS_PRO_ID */

/* FIXME(dhendrix): consolidate samsung ID code/#defines to a common location */
#include <cpu/samsung/exynos5250/setup.h>	/* cpu_info_init() prototype */

static unsigned int s5p_cpu_id;
static unsigned int s5p_cpu_rev;

static void s5p_set_cpu_id(void)
{
	s5p_cpu_id = readl((void *)EXYNOS_PRO_ID);
	s5p_cpu_id = (0xC000 | ((s5p_cpu_id & 0x00FFF000) >> 12));

	/*
	 * 0xC200: EXYNOS4210 EVT0
	 * 0xC210: EXYNOS4210 EVT1
	 */
	if (s5p_cpu_id == 0xC200) {
		s5p_cpu_id |= 0x10;
		s5p_cpu_rev = 0;
	} else if (s5p_cpu_id == 0xC210) {
		s5p_cpu_rev = 1;
	}
}

int arch_cpu_init(void)
{
	s5p_set_cpu_id();

	printk(BIOS_INFO, "CPU:   S5P%X @ %ldMHz\n",
			s5p_cpu_id, get_arm_clk() / (1024*1024));

	return 0;
}
