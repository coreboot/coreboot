/*
 * This file is part of the coreboot project.
 *
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

#ifndef _SOC_CANNONLAKE_CPU_H_
#define _SOC_CANNONLAKE_CPU_H_

#include <device/device.h>
#include <intelblocks/msr.h>

/* Latency times in units of 32768ns */
#define C_STATE_LATENCY_CONTROL_0_LIMIT	0x9d
#define C_STATE_LATENCY_CONTROL_1_LIMIT	0x9d
#define C_STATE_LATENCY_CONTROL_2_LIMIT	0x9d
#define C_STATE_LATENCY_CONTROL_3_LIMIT	0x9d
#define C_STATE_LATENCY_CONTROL_4_LIMIT	0x9d
#define C_STATE_LATENCY_CONTROL_5_LIMIT	0x9d

/* Power in units of mW */
#define C1_POWER	0x3e8
#define C6_POWER	0x15e
#define C7_POWER	0xc8
#define C8_POWER	0xc8
#define C9_POWER	0xc8
#define C10_POWER	0xc8

/* Common Timer Copy (CTC) frequency - 24MHz. */
#define CTC_FREQ		24000000

#define C_STATE_LATENCY_MICRO_SECONDS(limit, base) \
	(((1 << ((base)*5)) * (limit)) / 1000)
#define C_STATE_LATENCY_FROM_LAT_REG(reg) \
	C_STATE_LATENCY_MICRO_SECONDS(C_STATE_LATENCY_CONTROL_ ##reg## _LIMIT, \
				      (IRTL_1024_NS >> 10))

/* Configure power limits for turbo mode */
void set_power_limits(u8 power_limit_1_time);

#endif
