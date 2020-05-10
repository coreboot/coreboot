/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ROCKCHIP_RK3399_TIMER_H__
#define __SOC_ROCKCHIP_RK3399_TIMER_H__

#include <stdint.h>
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
