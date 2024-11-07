/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/rtc.h>
#include <soc/rtc_common.h>

/* 32k clock calibration */
u16 rtc_eosc_cali(void)
{
	u16 diff_left, diff_right, cksel;
	u16 val = 0;
	u16 middle;
	u16 left = RTC_XOSCCALI_START;
	u16 right = RTC_XOSCCALI_END;

	rtc_read(PMIC_RG_FQMTR_CKSEL, &cksel);
	cksel &= ~PMIC_FQMTR_CKSEL_MASK;
	/* select EOSC_32 as fixed clock */
	rtc_write(PMIC_RG_FQMTR_CKSEL, cksel | PMIC_FQMTR_FIX_CLK_EOSC_32K);
	rtc_read(PMIC_RG_FQMTR_CKSEL, &cksel);
	rtc_info("PMIC_RG_FQMTR_CKSEL=%#x\n", cksel);

	while (left <= right) {
		middle = (right + left) / 2;
		if (middle == left)
			break;

		/* select 26M as target clock */
		val = rtc_get_frequency_meter(middle, PMIC_FQMTR_CON0_FQM26M_CK, 0);
		if (val >= RTC_FQMTR_LOW_BASE && val <= RTC_FQMTR_HIGH_BASE)
			break;

		if (val > RTC_FQMTR_HIGH_BASE)
			right = middle;
		else
			left = middle;
	}

	if (val >= RTC_FQMTR_LOW_BASE && val <= RTC_FQMTR_HIGH_BASE)
		return middle;

	val = rtc_get_frequency_meter(left, PMIC_FQMTR_CON0_FQM26M_CK, 0);
	diff_left = ABS(val - RTC_FQMTR_LOW_BASE);

	val = rtc_get_frequency_meter(right, PMIC_FQMTR_CON0_FQM26M_CK, 0);
	diff_right = ABS(val - RTC_FQMTR_LOW_BASE);

	rtc_info("left: %d, middle: %d, right: %d\n", left, middle, right);
	if (diff_left < diff_right)
		return left;
	else
		return right;
}

void rtc_osc_init(void)
{
	u16 osc32con;

	/* enable 32K export */
	rtc_gpio_init();
	/* calibrate eosc32 for powerdown clock */
	rtc_read(RTC_OSC32CON, &osc32con);
	rtc_info("osc32con val = %#x\n", osc32con);
	osc32con &= ~RTC_XOSCCALI_MASK;
	osc32con |= rtc_eosc_cali() & RTC_XOSCCALI_MASK;
	rtc_xosc_write(osc32con);
	rtc_info("EOSC32 cali val = %#x\n", osc32con);
}
