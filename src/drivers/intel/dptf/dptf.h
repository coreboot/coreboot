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
};

const struct dptf_platform_info *get_dptf_platform_info(void);

#endif /* _DRIVERS_INTEL_DPTF_H_ */
