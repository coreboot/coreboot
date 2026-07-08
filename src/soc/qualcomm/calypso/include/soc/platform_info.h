/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_CALYPSO_PLATFORM_INFO_H__
#define __SOC_QUALCOMM_CALYPSO_PLATFORM_INFO_H__

#include <stdint.h>

#define CALYPSO_ID_SCP 0x02B5

/* Device number read from TCSR_SOC_HW_VERSION bits[27:16] */
#define TCSR_SOC_HW_VERSION_DEVICE_NUM_CALYPSO 0x21

enum qclib_soc_id {
	SOC_ID_UNKNOWN,
	SOC_ID_CALYPSO,
};

union tcsr_soc_hw_version {
	struct {
		uint32_t minor_version  : 8;  /* bits[7:0]   */
		uint32_t major_version  : 8;  /* bits[15:8]  */
		uint32_t device_number  : 12; /* bits[27:16] */
		uint32_t family_number  : 4;  /* bits[31:28] */
	};
	uint32_t data;
};

enum qclib_soc_id platform_get_soc_id(void);

#endif /* __SOC_QUALCOMM_CALYPSO_PLATFORM_INFO_H__ */
