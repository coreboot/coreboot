/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_TRACKER_V3_H
#define SOC_MEDIATEK_TRACKER_V3_H

#include <stdint.h>

#define TRACKER_V3_OFFSETS_SIZE		8
#define AR_TRACK_LOG_OFFSET		0x0200
#define AR_ENTRY_ID_OFFSET		0x0300
#define AR_TRACK_L_OFFSET		0x0400
#define AR_TRACK_H_OFFSET		0x0600
#define AW_TRACK_LOG_OFFSET		0x0800
#define AW_ENTRY_ID_OFFSET		0x0900
#define AW_TRACK_L_OFFSET		0x0A00
#define AW_TRACK_H_OFFSET		0x0C00

extern const u32 tracker_v3_offsets[];

#endif
