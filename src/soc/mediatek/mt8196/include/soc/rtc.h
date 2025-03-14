/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 14.2.4
 */

#ifndef SOC_MEDIATEK_MT8196_RTC_H
#define SOC_MEDIATEK_MT8196_RTC_H

#include <soc/mt6685_rtc.h>
#include <soc/rtc_common.h>
#include <soc/rtc_reg_common.h>
#include <stdbool.h>
#include <types.h>

/* RTC registers */
enum {
	RTC_BBPU_PWREN		= BIT(0),
	RTC_BBPU_CLRPKY		= BIT(4),
	RTC_BBPU_RELOAD		= BIT(5),
	RTC_BBPU_CBUSY		= BIT(6),

	RTC_CBUSY_TIMEOUT_US	= 8000,
};

enum {
	RTC_XOSCCALI_MASK	= 0x1F << 0,
	RTC_XOSC32_ENB		= BIT(5),
	RTC_EMBCK_SEL_MODE_MASK	= 3U << 6,
	RTC_EMBCK_SRC_SEL	= BIT(8),
	RTC_EMBCK_SEL_OPTION	= BIT(9),
	RTC_GPS_CKOUT_EN	= BIT(10),
	RTC_REG_XOSC32_ENB	= BIT(15),
};

/* PMIC Frequency Meter Definition */
enum {
	PMIC_RG_FQMTR_CKSEL	= 0x0111,
	PMIC_RG_FQMTR_RST	= 0x011F,
	PMIC_RG_FQMTR_CON0	= 0x0546,
	PMIC_RG_FQMTR_WINSET	= 0x0548,
	PMIC_RG_FQMTR_DATA	= 0x054A,

	FQMTR_TIMEOUT_US	= 100000,
};

enum {
	PMIC_FQMTR_FIX_CLK_26M		= 0U,
	PMIC_FQMTR_FIX_CLK_XOSC_32K_DET	= 1U,
	PMIC_FQMTR_FIX_CLK_EOSC_32K	= 2U,
	PMIC_FQMTR_FIX_CLK_RTC_32K	= 3U,
	PMIC_FQMTR_FIX_CLK_SMPS_CK	= 4U,
	PMIC_FQMTR_FIX_CLK_TCK_SEC	= 5U,
	PMIC_FQMTR_FIX_CLK_PMU_75K	= 6U,
	PMIC_FQMTR_CKSEL_MASK		= 7U,
};

#define BBPU_RELOAD_TIMEOUT_US		100000
#define EOSC_CHECK_CLK_TIMEOUT_US	1000000
#define RECOVERY_RETRY_COUNT		3

struct rtc_clk_freq {
	u16 fqm26m_ck;
	u16 dcxo_f32k_ck;
	u16 fqm26m_target_ck;
};

void rtc_get_time(struct rtc_time *tm);

#endif /* SOC_MEDIATEK_MT8196_RTC_H */
