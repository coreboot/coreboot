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
	ALIB_DPTC_SLOW_PPT_TIME_CONSTANT_ID = 0x8,
	ALIB_DPTC_PROCHOT_L_DEASSERTION_RAMP_TIME_ID = 0x9,
	ALIB_DPTC_VRM_CURRENT_LIMIT_ID = 0xb,
	ALIB_DPTC_VRM_MAXIMUM_CURRENT_LIMIT = 0xc,
	/* Picasso: SetVrmSocCurrentLimit (0xe) is not implemented in alib. */
	ALIB_DPTC_VRM_SOC_CURRENT_LIMIT_ID = 0xe,

	ALIB_DPTC_STT_SKIN_TEMPERATURE_LIMIT_APU_ID = 0x22,
	ALIB_DPTC_STT_M1_ID = 0x26,
	ALIB_DPTC_STT_M2_ID = 0x27,
	ALIB_DPTC_STT_C_APU_ID = 0x2C,
	ALIB_DPTC_STT_MIN_LIMIT_ID = 0x2E,
};

struct alib_dptc_param {
	uint8_t id;
	uint32_t value;
} __packed;

void acpigen_write_alib_dptc_default(uint8_t *default_param, size_t default_param_len);
void acpigen_write_alib_dptc_no_battery(uint8_t *no_battery_param, size_t no_battery_param_len);
void acpigen_write_alib_dptc_tablet(uint8_t *tablet_param, size_t tablet_param_len);
void acpigen_write_alib_dptc_thermal_B(uint8_t *thermal_param_B, size_t thermal_param_B_len);
void acpigen_write_alib_dptc_thermal_C(uint8_t *thermal_param_C, size_t thermal_param_C_len);
void acpigen_write_alib_dptc_thermal_D(uint8_t *thermal_param_D, size_t thermal_param_D_len);
void acpigen_write_alib_dptc_thermal_E(uint8_t *thermal_param_E, size_t thermal_param_E_len);
void acpigen_write_alib_dptc_thermal_F(uint8_t *thermal_param_F, size_t thermal_param_F_len);
#endif /* !__ACPI__ */

#endif /* AMD_COMMON_ALIB_H */
