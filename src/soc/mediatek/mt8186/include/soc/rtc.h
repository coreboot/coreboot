/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.13
 */

#ifndef SOC_MEDIATEK_MT8186_RTC_H
#define SOC_MEDIATEK_MT8186_RTC_H

#include <soc/pmic_wrap_common.h>
#include <soc/rtc_common.h>
#include <soc/rtc_reg_common.h>
#include <stdbool.h>

/* RTC registers */
enum {
	RTC_BBPU_PWREN		= 1U << 0,
	RTC_BBPU_CLR		= 1U << 1,
	RTC_BBPU_INIT		= 1U << 2,
	RTC_BBPU_AUTO		= 1U << 3,
	RTC_BBPU_CLRPKY		= 1U << 4,
	RTC_BBPU_RELOAD		= 1U << 5,
	RTC_BBPU_CBUSY		= 1U << 6,

	RTC_CBUSY_TIMEOUT_US	= 8000,
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
	RTC_REG_XOSC32_ENB	= 1U << 15,
};

/* PMIC DCXO Register Definition */
enum {
	PMIC_RG_DCXO_CW00	= 0x0788,
	PMIC_RG_DCXO_CW00_CLR	= 0x078C,
	PMIC_RG_DCXO_CW02	= 0x0790,
	PMIC_RG_DCXO_CW03	= 0x0794,
	PMIC_RG_DCXO_CW07	= 0x079A,
	PMIC_RG_DCXO_CW09	= 0x079E,
	PMIC_RG_DCXO_CW11	= 0x07A2,
	PMIC_RG_DCXO_CW13	= 0x07AA,
	PMIC_RG_DCXO_CW15	= 0x07AE,
	PMIC_RG_DCXO_CW16	= 0x07B0,
	PMIC_RG_DCXO_CW21	= 0x07BA,
	PMIC_RG_DCXO_CW23	= 0x07BE,
	PMIC_RG_DCXO_ELR0	= 0x07C4,
};

enum {
	PMIC_RG_TOP_TMA_KEY	= 0x03A8,
};

/* PMIC Frequency Meter Definition */
enum {
	PMIC_RG_FQMTR_CKSEL	= 0x0118,
	PMIC_RG_FQMTR_RST	= 0x013E,
	PMIC_RG_FQMTR_CON0	= 0x0514,
	PMIC_RG_FQMTR_WINSET	= 0x0516,
	PMIC_RG_FQMTR_DATA	= 0x0518,

	FQMTR_TIMEOUT_US	= 8000,
};

enum {
	PMIC_FQMTR_FIX_CLK_26M		= 0U << 0,
	PMIC_FQMTR_FIX_CLK_XOSC_32K_DET	= 1U << 0,
	PMIC_FQMTR_FIX_CLK_EOSC_32K	= 2U << 0,
	PMIC_FQMTR_FIX_CLK_RTC_32K	= 3U << 0,
	PMIC_FQMTR_FIX_CLK_SMPS_CK	= 4U << 0,
	PMIC_FQMTR_FIX_CLK_TCK_SEC	= 5U << 0,
	PMIC_FQMTR_FIX_CLK_PMU_75K	= 6U << 0,
	PMIC_FQMTR_CKSEL_MASK		= 7U << 0,
};


/* external API */
void rtc_bbpu_power_on(void);
int rtc_init(int recover);
bool rtc_gpio_init(void);
void rtc_boot(void);
u16 rtc_get_frequency_meter(u16 val, u16 measure_src, u16 window_size);

static inline s32 rtc_read(u16 addr, u16 *rdata)
{
	s32 ret;

	ret = pwrap_read(addr, rdata);
	if (ret)
		rtc_info("pwrap_read failed: ret=%d\n", ret);

	return ret;
}

static inline s32 rtc_write(u16 addr, u16 wdata)
{
	s32 ret;

	ret = pwrap_write(addr, wdata);
	if (ret)
		rtc_info("pwrap_write failed: ret=%d\n", ret);

	return ret;
}

#endif /* SOC_MEDIATEK_MT8186_RTC_H */
