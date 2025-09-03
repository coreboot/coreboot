/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8189_TRACKER_H
#define SOC_MEDIATEK_MT8189_TRACKER_H

#include <soc/tracker_common.h>
#include <soc/tracker_v3.h>

#define BUS_DBG_CON			0x000
#define BUS_DBG_TIMER_CON0_26M		0x050
#define BUS_TRACE_CON_AO_1		0x8FC
#define BUS_TRACE_CON_AO_2		0x9FC
#define BUS_TRACE_EN			16

#define SYS_TRACK_ENTRY			64
#define INFRA_ENTRY_NUM			32

enum {
	TRACKER_SYSTRACKER = 0,
	TRACKER_INFRATRACKER,
	TRACKER_NUM,
};

#endif
