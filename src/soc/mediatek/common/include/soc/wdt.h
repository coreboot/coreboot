/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef SOC_MEDIATEK_COMMON_WDT_H
#define SOC_MEDIATEK_COMMON_WDT_H

#include <stdint.h>

struct mtk_wdt_regs {
	u32 wdt_mode;
	u32 wdt_length;
	u32 wdt_restart;
	u32 wdt_status;
	u32 wdt_interval;
	u32 wdt_swrst;
	u32 wdt_swsysrst;
	u32 reserved[9];
	u32 wdt_debug_ctrl;
};

/* WDT_MODE */
enum {
	MTK_WDT_MODE_KEY	= 0x22000000,
	MTK_WDT_MODE_DUAL_MODE	= 1 << 6,
	MTK_WDT_MODE_IRQ	= 1 << 3,
	MTK_WDT_MODE_EXTEN	= 1 << 2,
	MTK_WDT_MODE_EXT_POL	= 1 << 1,
	MTK_WDT_MODE_ENABLE	= 1 << 0
};

/* WDT_RESET */
enum {
	MTK_WDT_SWRST_KEY	= 0x1209,
	MTK_WDT_STA_SPM_RST	= 1 << 1,
	MTK_WDT_STA_SW_RST	= 1 << 30,
	MTK_WDT_STA_HW_RST	= 1 << 31
};

int mtk_wdt_init(void);

#endif /* SOC_MEDIATEK_COMMON_WDT_H */
