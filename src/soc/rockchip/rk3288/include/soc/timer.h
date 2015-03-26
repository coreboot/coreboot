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
 * Foundation, Inc.
 */

#ifndef __SOC_ROCKCHIP_RK3288_TIMER_H__
#define __SOC_ROCKCHIP_RK3288_TIMER_H__

#include <inttypes.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <timer.h>

static const u32 clocks_per_usec = OSC_HZ/USECS_PER_SEC;

struct rk3288_timer {
	u32 timer_load_count0;
	u32 timer_load_count1;
	u32 timer_curr_value0;
	u32 timer_curr_value1;
	u32 timer_ctrl_reg;
	u32 timer_int_status;
};

static struct rk3288_timer * const timer7_ptr = (void *)TIMER7_BASE;

#define TIMER_LOAD_VAL	0xffffffff

#endif	/* __SOC_ROCKCHIP_RK3288_TIMER_H__ */
