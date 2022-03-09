/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_WDT_H
#define SOC_MEDIATEK_COMMON_WDT_H

#include <stdint.h>
#include <soc/addressmap.h>

struct mtk_wdt_regs {
	u32 wdt_mode;
	u32 wdt_length;
	u32 wdt_restart;
	u32 wdt_status;
	u32 wdt_interval;
	u32 wdt_swrst;
	u32 wdt_swsysrst;
	u32 reserved0[5];
	u32 wdt_req_mode;
	u32 reserved1[3];
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

static struct mtk_wdt_regs *const mtk_wdt = (void *)RGU_BASE;

void mtk_wdt_preinit(void);
int mtk_wdt_init(void);
void mtk_wdt_clr_status(void);

#endif /* SOC_MEDIATEK_COMMON_WDT_H */
