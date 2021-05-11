/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_COMMON_ALIB_H
#define AMD_COMMON_ALIB_H

#define ALIB_FUNCTION_REPORT_AC_DC_STATE		0x1
#define ALIB_FUNCTION_DYNAMIC_POWER_THERMAL_CONFIG	0xc

#ifndef __ACPI__

/* parameter IDs for the ALIB_FUNCTION_DYNAMIC_POWER_THERMAL_CONFIG function */
enum alib_dptc_parameter_ids {
	ALIB_DPTC_THERMAL_CONTROL_LIMIT_ID = 0x3,
	ALIB_DPTC_SUSTAINED_POWER_LIMIT_ID = 0x5,
	ALIB_DPTC_FAST_PPT_LIMIT_ID = 0x6,
	ALIB_DPTC_SLOW_PPT_LIMIT_ID = 0x7,
};

struct alib_dptc_param {
	uint8_t id;
	uint32_t value;
} __packed;

void acpigen_write_alib_dptc(uint8_t *default_param, size_t default_param_len,
	uint8_t *tablet_param, size_t tablet_param_len);

#endif /* !__ACPI__ */

#endif /* AMD_COMMON_ALIB_H */
