/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ROCKCHIP_RK3288_TIMER_H__
#define __SOC_ROCKCHIP_RK3288_TIMER_H__

#include <stdint.h>
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
