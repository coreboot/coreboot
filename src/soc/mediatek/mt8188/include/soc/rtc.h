/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8188_RTC_H
#define SOC_MEDIATEK_MT8188_RTC_H

#include <soc/pmif.h>
#include <soc/rtc_reg_common.h>
#include <stdbool.h>

/* RTC registers */
enum {
	RTC_BBPU_ENABLE_ALARM	= 1U << 0,
	RTC_BBPU_SPAR_SW	= 1U << 1,
	RTC_BBPU_RESET_SPAR	= 1U << 2,
	RTC_BBPU_RESET_ALARM	= 1U << 3,
	RTC_BBPU_CLRPKY	= 1U << 4,
	RTC_BBPU_RELOAD	= 1U << 5,
	RTC_BBPU_CBUSY	= 1U << 6,
	RTC_CBUSY_TIMEOUT_US = 1000000,
};

enum {
	RTC_XOSCCALI_MASK	= 0x1F << 0,
	RTC_XOSC32_ENB		= 1U << 5,
	RTC_EMB_HW_MODE		= 0U << 6,
	RTC_EMB_K_EOSC32_MODE	= 1U << 6,
	RTC_EMB_SW_DCXO_MODE	= 2U << 6,
	RTC_EMB_SW_EOSC32_MODE	= 3U << 6,
	RTC_EMBCK_SEL_MODE_MASK	= 3U << 6,
	RTC_EMBCK_SRC_SEL	= 1U << 8,
	RTC_EMBCK_SEL_OPTION	= 1U << 9,
	RTC_GPS_CKOUT_EN	= 1U << 10,
	RTC_EOSC32_VCT_EN	= 1U << 11,
	RTC_EOSC32_CHOP_EN	= 1U << 12,
	RTC_GP_OSC32_CON	= 2U << 13,
	RTC_REG_XOSC32_ENB	= 1U << 15,
};

enum {
	OSC32CON_ANALOG_SETTING = RTC_GP_OSC32_CON | RTC_EOSC32_CHOP_EN |
		RTC_EOSC32_VCT_EN | RTC_GPS_CKOUT_EN | RTC_EMBCK_SEL_OPTION |
		RTC_EMB_K_EOSC32_MODE,
};

enum {
	PMIC_RG_BANK_FQMTR_RST = 0x522,
};

enum {
	PMIC_RG_FQMTR_DCXO26M_EN_SHIFT = 4,
	PMIC_RG_BANK_FQMTR_RST_SHIFT = 6,
};

/* PMIC frequency meter definition */
enum {
	PMIC_RG_FQMTR_CKSEL	= 0x0118,
	PMIC_RG_FQMTR_RST	= 0x013A,
	PMIC_RG_FQMTR_CON0	= 0x0546,
	PMIC_RG_FQMTR_WINSET	= 0x0548,
	PMIC_RG_FQMTR_DATA	= 0x054A,
	FQMTR_TIMEOUT_US	= 8000,
};

enum {
	PMIC_FQMTR_FIX_CLK_26M		= 0U << 0,
	PMIC_FQMTR_FIX_CLK_XOSC_32K_DET	= 1U << 0,
	PMIC_FQMTR_FIX_CLK_EOSC_32K	= 2U << 0,
	PMIC_FQMTR_FIX_CLK_RTC_32K	= 3U << 0,
	PMIC_FQMTR_FIX_CLK_DCXO1M_CK	= 4U << 0,
	PMIC_FQMTR_FIX_CLK_TCK_SEC	= 5U << 0,
	PMIC_FQMTR_FIX_CLK_PMU_32K	= 6U << 0,
	PMIC_FQMTR_CKSEL_MASK		= 7U << 0,
};

enum {
	RTC_TC_MTH_MASK = 0xf,
};

enum {
	RTC_K_EOSC_RSV_0	= 1 << 8,
	RTC_K_EOSC_RSV_1	= 1 << 9,
	RTC_K_EOSC_RSV_2	= 1 << 10,
};

void rtc_read(u16 addr, u16 *rdata);
void rtc_write(u16 addr, u16 wdata);
void rtc_bbpu_power_on(void);
int rtc_init(int recover);
bool rtc_gpio_init(void);
void rtc_boot(void);
u16 rtc_get_frequency_meter(u16 val, u16 measure_src, u16 window_size);

#endif /* SOC_MEDIATEK_MT8188_RTC_H */
