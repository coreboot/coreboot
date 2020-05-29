/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRIVERS_INTEL_DPTF_CHIP_H_
#define _DRIVERS_INTEL_DPTF_CHIP_H_

#include <acpi/acpigen_dptf.h>

struct drivers_intel_dptf_config {
	struct {
		struct dptf_active_policy active[DPTF_MAX_ACTIVE_POLICIES];
		struct dptf_critical_policy critical[DPTF_MAX_CRITICAL_POLICIES];
		struct dptf_passive_policy passive[DPTF_MAX_PASSIVE_POLICIES];
	} policies;

	struct {
		struct dptf_charger_perf charger_perf[DPTF_MAX_CHARGER_PERF_STATES];
		struct dptf_fan_perf fan_perf[DPTF_MAX_FAN_PERF_STATES];
	} controls;
};

#endif /* _DRIVERS_INTEL_DPTF_CHIP_H_ */
