/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8173_RTC_H
#define SOC_MEDIATEK_MT8173_RTC_H

#include <soc/pmic_wrap_common.h>
#include <soc/rtc_common.h>
#include <stdbool.h>
#include <stdint.h>
#include "mt6391.h"

/* RTC registers */
enum {
	RTC_BBPU = 0xE000,
	RTC_IRQ_STA = 0xE002,
	RTC_IRQ_EN = 0xE004,
	RTC_CII_EN = 0xE006
};

enum {
	RTC_TC_SEC = 0xE00A,
	RTC_TC_MIN = 0xE00C,
	RTC_TC_HOU = 0xE00E,
	RTC_TC_DOM = 0xE010,
	RTC_TC_DOW = 0xE012,
	RTC_TC_MTH = 0xE014,
	RTC_TC_YEA = 0xE016
};

enum {
	RTC_AL_SEC = 0xE018,
	RTC_AL_MIN = 0xE01A,
	RTC_AL_HOU = 0xE01C,
	RTC_AL_DOM = 0xE01E,
	RTC_AL_DOW = 0xE020,
	RTC_AL_MTH = 0xE022,
	RTC_AL_YEA = 0xE024,
	RTC_AL_MASK = 0xE008
};

enum {
	RTC_OSC32CON = 0xE026,
	RTC_CON = 0xE03E,
	RTC_WRTGR = 0xE03C
};

enum {
	RTC_POWERKEY1 = 0xE028,
	RTC_POWERKEY2 = 0xE02A
};

enum {
	RTC_PDN1 = 0xE02C,
	RTC_PDN2 = 0xE02E,
	RTC_SPAR0 = 0xE030,
	RTC_SPAR1 = 0xE032,
	RTC_PROT = 0xE036,
	RTC_DIFF = 0xE038,
	RTC_CALI = 0xE03A
};

enum {
	RTC_BBPU_PWREN	= 1U << 0,
	RTC_BBPU_BBPU	= 1U << 2,
	RTC_BBPU_AUTO	= 1U << 3,
	RTC_BBPU_CLRPKY	= 1U << 4,
	RTC_BBPU_RELOAD	= 1U << 5,
	RTC_BBPU_CBUSY	= 1U << 6,

	RTC_CBUSY_TIMEOUT_US = 8000
};

enum {
	RTC_OSC32CON_AMPEN	= 1U << 8,
	RTC_OSC32CON_LNBUFEN	= 1U << 11
};

enum {
	RTC_CON_LPEN		= 1U << 2,
	RTC_CON_LPRST		= 1U << 3,
	RTC_CON_CDBO		= 1U << 4,
	RTC_CON_F32KOB		= 1U << 5,
	RTC_CON_GPO		= 1U << 6,
	RTC_CON_GOE		= 1U << 7,
	RTC_CON_GSR		= 1U << 8,
	RTC_CON_GSMT		= 1U << 9,
	RTC_CON_GPEN		= 1U << 10,
	RTC_CON_GPU		= 1U << 11,
	RTC_CON_GE4		= 1U << 12,
	RTC_CON_GE8		= 1U << 13,
	RTC_CON_GPI		= 1U << 14,
	RTC_CON_LPSTA_RAW	= 1U << 15
};

enum {
	RTC_CALI_BBPU_2SEC_EN		= 1U << 8,
	RTC_CALI_BBPU_2SEC_MODE_SHIFT	= 9,
	RTC_CALI_BBPU_2SEC_MODE_MSK	= 3U << RTC_CALI_BBPU_2SEC_MODE_SHIFT,
	RTC_CALI_BBPU_2SEC_STAT		= 1U << 11
};

/* external API */
int rtc_init(int recover);
void rtc_boot(void);
s32 rtc_read(u16 addr, u16 *rdata);
s32 rtc_write(u16 addr, u16 wdata);

#endif /* SOC_MEDIATEK_MT8173_RTC_H */
