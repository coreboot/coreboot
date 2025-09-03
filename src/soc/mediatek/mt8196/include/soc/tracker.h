/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8196_TRACKER_H
#define SOC_MEDIATEK_MT8196_TRACKER_H

#include <soc/tracker_common.h>
#include <soc/tracker_v3.h>

#define BUS_DBG_CON			0x000
#define BUS_TRACE_CON_AO_PRESCALE	0x8f8
#define BUS_TRACE_CON_AO_1_PRESCALE	0x9f8
#define VLP_TRACE_CON_AO_PRESCALE	0x114
#define BUS_TRACE_CON_1			0x800
#define BUS_TRACE_CON_AO_1		0x8FC
#define BUS_TRACE_CON_2			0x900
#define BUS_TRACE_CON_AO_2		0x9FC
#define VLP_CON_AO			0x30C
#define BUS_TRACE_EN			16

#define SYS_TRACK_ENTRY			64
#define INFRA_ENTRY_NUM			32
#define VLP_ENTRY_NUM			4

enum {
	TRACKER_SYSTRACKER = 0,
	TRACKER_INFRATRACKER,
	TRACKER_VLPSYSTRACKER,
	TRACKER_NUM,
};

#endif
