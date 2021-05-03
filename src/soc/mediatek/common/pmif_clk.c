/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <soc/pmif_clk_common.h>
#include <soc/pmif_sw.h>

int pmif_ulposc_cali(u32 target_val)
{
	u32 current_val, min = 0, max = CAL_MAX_VAL, middle;
	int diff_by_min, diff_by_max, cal_result;

	do {
		middle = (min + max) / 2;
		if (middle == min)
			break;

		current_val = pmif_get_ulposc_freq_mhz(middle);
		if (current_val > target_val)
			max = middle;
		else
			min = middle;
	} while (min <= max);

	diff_by_min = pmif_get_ulposc_freq_mhz(min) - target_val;
	diff_by_min = ABS(diff_by_min);

	diff_by_max = pmif_get_ulposc_freq_mhz(max) - target_val;
	diff_by_max = ABS(diff_by_max);

	cal_result = (diff_by_min < diff_by_max) ? min : max;
	current_val = pmif_get_ulposc_freq_mhz(cal_result);

	/* check if calibrated value is in the range of target value +- 15% */
	if (current_val < (target_val * (1000 - CAL_TOL_RATE) / 1000) ||
	    current_val > (target_val * (1000 + CAL_TOL_RATE) / 1000)) {
		printk(BIOS_ERR, "[%s] calibration fail: cur=%d, CAL_RATE=%d, target=%dM\n",
		       __func__, current_val, CAL_TOL_RATE, target_val);
		return 1;
	}

	return 0;
}
