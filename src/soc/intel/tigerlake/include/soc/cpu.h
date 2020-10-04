/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_TIGERLAKE_CPU_H_
#define _SOC_TIGERLAKE_CPU_H_

#include <intelblocks/msr.h>

/* Latency times in us */
#define C1_LATENCY	1
#define C6_LATENCY	121
#define C7_LATENCY	152
#define C8_LATENCY	256
#define C9_LATENCY	340
#define C10_LATENCY	1034

/* Power in units of mW */
#define C1_POWER	0x3e8
#define C6_POWER	0x15e
#define C7_POWER	0xc8
#define C8_POWER	0xc8
#define C9_POWER	0xc8
#define C10_POWER	0xc8

#endif
