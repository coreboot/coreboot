/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_QUARK_NVS_H
#define SOC_INTEL_QUARK_NVS_H

#include <stdint.h>

struct __packed global_nvs {
	uint32_t	cbmc; /* 0x00 - 0x03 - coreboot Memory Console */
	uint8_t		unused_was_pwrs; /* 0x4 - Power state (AC = 1) */

	/* Required for future unified acpi_save_wake_source. */
	uint32_t	pm1i;
	uint32_t	gpei;
};

#endif /* SOC_INTEL_QUARK_NVS_H */
