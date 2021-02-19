/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CPU_H_
#define _SOC_CPU_H_

#include <device/device.h>

/* Latency times in units of 1024ns. */
#define C_STATE_LATENCY_CONTROL_0_LIMIT	0x4e
#define C_STATE_LATENCY_CONTROL_1_LIMIT	0x76
#define C_STATE_LATENCY_CONTROL_2_LIMIT	0x94
#define C_STATE_LATENCY_CONTROL_3_LIMIT	0xfa
#define C_STATE_LATENCY_CONTROL_4_LIMIT	0x14c
#define C_STATE_LATENCY_CONTROL_5_LIMIT	0x3f2

/* Power in units of mW */
#define C1_POWER	0x3e8
#define C3_POWER	0x1f4
#define C6_POWER	0x15e
#define C7_POWER	0xc8
#define C8_POWER	0xc8
#define C9_POWER	0xc8
#define C10_POWER	0xc8

#define C_STATE_LATENCY_MICRO_SECONDS(limit, base) \
	(((1 << ((base)*5)) * (limit)) / 1000)
#define C_STATE_LATENCY_FROM_LAT_REG(reg) \
	C_STATE_LATENCY_MICRO_SECONDS(C_STATE_LATENCY_CONTROL_ ##reg## _LIMIT, \
				      (IRTL_1024_NS >> 10))

/* CPU identification */
u32 cpu_family_model(void);
u32 cpu_stepping(void);
int cpu_is_ult(void);

#endif
