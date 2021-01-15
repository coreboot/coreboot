/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_QUARK_NVS_H
#define SOC_INTEL_QUARK_NVS_H

#include <stdint.h>

struct __packed global_nvs {
	uint32_t	cbmc; /* 0x00 - 0x03 - coreboot Memory Console */
	uint8_t		pwrs; /* 0x4 - Power state (AC = 1) */
};

#endif /* SOC_INTEL_QUARK_NVS_H */
