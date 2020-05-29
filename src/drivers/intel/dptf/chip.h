/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRIVERS_INTEL_DPTF_CHIP_H_
#define _DRIVERS_INTEL_DPTF_CHIP_H_

#include <acpi/acpigen_dptf.h>

struct drivers_intel_dptf_config {
	struct {
		struct dptf_active_policy active[DPTF_MAX_ACTIVE_POLICIES];
		struct dptf_passive_policy passive[DPTF_MAX_PASSIVE_POLICIES];
	} policies;
};

#endif /* _DRIVERS_INTEL_DPTF_CHIP_H_ */
