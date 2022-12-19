/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRIVERS_INTEL_DPTF_H_
#define _DRIVERS_INTEL_DPTF_H_

#include <types.h>

struct dptf_platform_info {
	/*
	 * True indicates the platform-specific HIDs are to be emitted in EISA
	 * format instead of a string.
	 */
	bool use_eisa_hids;
	const char *dptf_device_hid;
	const char *generic_hid;
	const char *fan_hid;
	const char *tpch_device_hid;
	const char *tpwr_device_hid;
	const char *tbat_device_hid;
	struct {
		const char *set_fivr_low_clock_method;
		const char *set_fivr_high_clock_method;
		const char *get_fivr_low_clock_method;
		const char *get_fivr_high_clock_method;
		const char *get_fivr_ssc_method;
		const char *get_fivr_switching_fault_status;
		const char *get_fivr_switching_freq_mhz;
	} tpch_method_names;
};

#endif /* _DRIVERS_INTEL_DPTF_H_ */
