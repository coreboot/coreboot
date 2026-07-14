/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/rtc.h>
#include <soc/rtc_common.h>

u16 rtc_get_frequency_meter(u16 val, u16 measure_src, u16 window_size)
{
	u16 result;
	u16 osc32con;

	if (!rtc_clrset_trigger(RTC_BBPU, 0, RTC_BBPU_KEY | RTC_BBPU_RELOAD))
		return 0;

	rtc_read(RTC_OSC32CON, &osc32con);
	if (!rtc_xosc_write((osc32con & ~RTC_XOSCCALI_MASK) |
			    (val & RTC_XOSCCALI_MASK)))
		return 0;

	result = rtc_measure_frequency_meter(measure_src, window_size);
	rtc_info("%s: input=0x%x, output=%d\n", __func__, val, result);
	return result;
}

#define IS_FQMTR_IN_WINDOW(val) ((val) >= RTC_FQMTR_LOW_BASE && (val) <= RTC_FQMTR_HIGH_BASE)

static u16 get_freq_meter(u16 step)
{
	static u16 cache[RTC_XOSCCALI_END + 1];

	if (step >= ARRAY_SIZE(cache))
		die("EOSC32 cali val %d out of bounds\n", step);

	if (cache[step])
		return cache[step];

	/* select 26M as target clock */
	cache[step] = rtc_get_frequency_meter(step, PMIC_FQMTR_CON0_FQM26M_CK, 0);
	return cache[step];
}

/* 32k clock calibration */
u16 rtc_eosc_cali(void)
{
	u16 diff_left, diff_right, chosen, cksel;
	u16 val = 0;
	u16 middle = 0;
	u16 left = RTC_XOSCCALI_START;
	u16 right = RTC_XOSCCALI_END;
	u16 last_cali = 0;

	rtc_read(PMIC_RG_FQMTR_CKSEL, &cksel);
	cksel &= ~PMIC_FQMTR_CKSEL_MASK;
	/* select EOSC_32 as fixed clock */
	rtc_write(PMIC_RG_FQMTR_CKSEL, cksel | PMIC_FQMTR_FIX_CLK_EOSC_32K);
	rtc_read(PMIC_RG_FQMTR_CKSEL, &cksel);
	rtc_info("PMIC_RG_FQMTR_CKSEL=%#x\n", cksel);

	/*
	 * Fast EOSC32 Step Checking: Inspect the previous calibration step.
	 * If ambient temperature/aging has not shifted EOSC32 outside target bounds,
	 * return immediately after a single frequency check (~1.5 ms).
	 */
	rtc_read(RTC_OSC32CON, &last_cali);
	last_cali &= RTC_XOSCCALI_MASK;
	val = get_freq_meter(last_cali);
	rtc_info("Last cali XOSC=%d, val=%d (target window [%d, %d])\n",
		 last_cali, val, RTC_FQMTR_LOW_BASE, RTC_FQMTR_HIGH_BASE);

	if (IS_FQMTR_IN_WINDOW(val)) {
		rtc_info("Last cali XOSC hit window\n");
		return last_cali;
	}

	/* Test adjacent step if frequency drifted slightly high or low */
	u16 adj_step;
	if (val > RTC_FQMTR_HIGH_BASE) {
		adj_step = (last_cali > RTC_XOSCCALI_START) ? last_cali - 1 : last_cali;
		right = last_cali;
	} else {
		adj_step = (last_cali < RTC_XOSCCALI_END) ? last_cali + 1 : last_cali;
		left = last_cali;
	}
	rtc_info("Last cali XOSC missed window, testing adjacent step=%d\n", adj_step);

	if (adj_step != last_cali) {
		u16 adj_val = get_freq_meter(adj_step);
		if (IS_FQMTR_IN_WINDOW(adj_val)) {
			rtc_info("Adjacent step=%d hit window (val=%d)\n",
				 adj_step, adj_val);
			return adj_step;
		}
		/*
		 * Clamp right/left with MAX/MIN against non-monotonic FQMTR noise:
		 * If left=10 (<792) and testing adj_step=9 noisily yields >796,
		 * right=MAX(9, 10)=10 prevents range crossing (left=10 > right=9).
		 */
		if (adj_val > RTC_FQMTR_HIGH_BASE)
			right = MAX(adj_step, left);
		else /* adj_val < RTC_FQMTR_LOW_BASE */
			left = MIN(adj_step, right);
	}

	while (left <= right) {
		middle = (right + left) / 2;
		if (middle == left)
			break;

		val = get_freq_meter(middle);
		if (IS_FQMTR_IN_WINDOW(val)) {
			rtc_info("Binary search hit inside window at step=%d (val=%d)\n",
				 middle, val);
			return middle;
		}

		if (val > RTC_FQMTR_HIGH_BASE)
			right = middle;
		else
			left = middle;
	}

	val = get_freq_meter(left);
	diff_left = ABS(val - RTC_FQMTR_LOW_BASE);

	val = get_freq_meter(right);
	diff_right = ABS(val - RTC_FQMTR_LOW_BASE);

	chosen = (diff_left < diff_right) ? left : right;
	rtc_info("Binary search exhausted: left step=%d (diff=%d),\n"
		 "  middle step=%d, right step=%d (diff=%d), chosen step=%d\n",
		 left, diff_left, middle, right, diff_right, chosen);
	return chosen;
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
