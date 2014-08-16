/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#ifndef __SOC_ROCKCHIP_RK3288_CPU_H__
#define __SOC_ROCKCHIP_RK3288_CPU_H__

#include <arch/io.h>

#define RK_CLRSETBITS(clr, set) ((((clr) | (set)) << 16) | set)
#define RK_SETBITS(set) RK_CLRSETBITS(0, set)
#define RK_CLRBITS(clr) RK_CLRSETBITS(clr, 0)

#define FB_SIZE_KB  4096
#define RAM_BASE_KB (CONFIG_SYS_SDRAM_BASE >> 10)
#define RAM_SIZE_KB (CONFIG_DRAM_SIZE_MB << 10UL)

static inline u32 get_fb_base_kb(void)
{
	return RAM_BASE_KB + RAM_SIZE_KB - FB_SIZE_KB;
}
#endif
