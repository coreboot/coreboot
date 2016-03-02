/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_ROCKCHIP_RK3399_TIMER_H__
#define __SOC_ROCKCHIP_RK3399_TIMER_H__

#include <inttypes.h>
#include <soc/addressmap.h>
#include <timer.h>

static const u32 clocks_per_usec = (24 * 1000 * 1000) / USECS_PER_SEC;

struct rk3399_timer {
	u32 timer_load_count0;
	u32 timer_load_count1;
	u32 timer_cur_value0;
	u32 timer_cur_value1;
	u32 timer_load_count2;
	u32 timer_load_count3;
	u32 timer_int_status;
	u32 timer_ctrl_reg;
};

static struct rk3399_timer * const timer0_ptr = (void *)TIMER0_BASE;
#define TIMER_LOAD_VAL	0xffffffff

#endif	/* __SOC_ROCKCHIP_RK3399_TIMER_H__ */
