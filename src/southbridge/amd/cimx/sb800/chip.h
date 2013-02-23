/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _CIMX_SB800_CHIP_H_
#define _CIMX_SB800_CHIP_H_
#include "fan.h" /* include for #defines used in devicetree.cb */

/*
 * configuration set in mainboard/devicetree.cb
 *  boot_switch_sata_ide:
 *   0 -set SATA as primary, PATA(IDE) as secondary.
 *   1 -set PATA(IDE) as primary, SATA as secondary. if you want to boot from IDE,
 *  gpp_configuration - The configuration of General Purpose Port A/B/C/D
 *   0(GPP_CFGMODE_X4000) -PortA Lanes[3:0]
 *   2(GPP_CFGMODE_X2200) -PortA Lanes[1:0], PortB Lanes[3:2]
 *   3(GPP_CFGMODE_X2110) -PortA Lanes[1:0], PortB Lane2, PortC Lane3
 *   4(GPP_CFGMODE_X1111) -PortA Lanes0, PortB Lane1, PortC Lane2, PortD Lane3
 */
struct southbridge_amd_cimx_sb800_config
{
	u32 boot_switch_sata_ide : 1;
	u8  gpp_configuration;

	/*
	 * SB800 IMC and fan control
	 */

	u16 imc_port_address;

	u32 fan0_enabled : 1;
	u32 fan1_enabled : 1;
	u32 fan2_enabled : 1;
	u32 fan3_enabled : 1;
	u32 fan4_enabled : 1;
	u32 imc_fan_zone0_enabled : 1;
	u32 imc_fan_zone1_enabled : 1;
	u32 imc_fan_zone2_enabled : 1;
	u32 imc_fan_zone3_enabled : 1;
	u32 imc_tempin0_enabled : 1;
	u32 imc_tempin1_enabled : 1;
	u32 imc_tempin2_enabled : 1;
	u32 imc_tempin3_enabled : 1;

	union {
		struct {
			u8  fan0_control_reg_value;
			u8  fan0_frequency_reg_value;
			u8  fan0_low_duty_reg_value;
			u8  fan0_med_duty_reg_value;
			u8  fan0_multiplier_reg_value;
			u8  fan0_low_temp_lo_reg_value;
			u8  fan0_low_temp_hi_reg_value;
			u8  fan0_med_temp_lo_reg_value;
			u8  fan0_med_temp_hi_reg_value;
			u8  fan0_high_temp_lo_reg_value;
			u8  fan0_high_temp_hi_reg_value;
			u8  fan0_linear_range_reg_value;
			u8  fan0_linear_hold_reg_value;
		};
		u8  fan0_config_vals[FAN_REGISTER_COUNT];
	};

	union {
		struct {
			u8  fan1_control_reg_value;
			u8  fan1_frequency_reg_value;
			u8  fan1_low_duty_reg_value;
			u8  fan1_med_duty_reg_value;
			u8  fan1_multiplier_reg_value;
			u8  fan1_low_temp_lo_reg_value;
			u8  fan1_low_temp_hi_reg_value;
			u8  fan1_med_temp_lo_reg_value;
			u8  fan1_med_temp_hi_reg_value;
			u8  fan1_high_temp_lo_reg_value;
			u8  fan1_high_temp_hi_reg_value;
			u8  fan1_linear_range_reg_value;
			u8  fan1_linear_hold_reg_value;
		};
		u8  fan1_config_vals[FAN_REGISTER_COUNT];
	};

	union {
		struct {
			u8  fan2_control_reg_value;
			u8  fan2_frequency_reg_value;
			u8  fan2_low_duty_reg_value;
			u8  fan2_med_duty_reg_value;
			u8  fan2_multiplier_reg_value;
			u8  fan2_low_temp_lo_reg_value;
			u8  fan2_low_temp_hi_reg_value;
			u8  fan2_med_temp_lo_reg_value;
			u8  fan2_med_temp_hi_reg_value;
			u8  fan2_high_temp_lo_reg_value;
			u8  fan2_high_temp_hi_reg_value;
			u8  fan2_linear_range_reg_value;
			u8  fan2_linear_hold_reg_value;
		};
		u8  fan2_config_vals[FAN_REGISTER_COUNT];
	};

	union {
		struct {
			u8  fan3_control_reg_value;
			u8  fan3_frequency_reg_value;
			u8  fan3_low_duty_reg_value;
			u8  fan3_med_duty_reg_value;
			u8  fan3_multiplier_reg_value;
			u8  fan3_low_temp_lo_reg_value;
			u8  fan3_low_temp_hi_reg_value;
			u8  fan3_med_temp_lo_reg_value;
			u8  fan3_med_temp_hi_reg_value;
			u8  fan3_high_temp_lo_reg_value;
			u8  fan3_high_temp_hi_reg_value;
			u8  fan3_linear_range_reg_value;
			u8  fan3_linear_hold_reg_value;
		};
		u8  fan3_config_vals[FAN_REGISTER_COUNT];
	};

	union {
		struct {
			u8  fan4_control_reg_value;
			u8  fan4_frequency_reg_value;
			u8  fan4_low_duty_reg_value;
			u8  fan4_med_duty_reg_value;
			u8  fan4_multiplier_reg_value;
			u8  fan4_low_temp_lo_reg_value;
			u8  fan4_low_temp_hi_reg_value;
			u8  fan4_med_temp_lo_reg_value;
			u8  fan4_med_temp_hi_reg_value;
			u8  fan4_high_temp_lo_reg_value;
			u8  fan4_high_temp_hi_reg_value;
			u8  fan4_linear_range_reg_value;
			u8  fan4_linear_hold_reg_value;
		};
		u8  fan4_config_vals[FAN_REGISTER_COUNT];
	};

	union {
		struct {
			u8 imc_zone0_mode1;
			u8 imc_zone0_mode2;
			u8 imc_zone0_temp_offset;
			u8 imc_zone0_hysteresis;
			u8 imc_zone0_smbus_addr;
			u8 imc_zone0_smbus_num;
			u8 imc_zone0_pwm_step;
			u8 imc_zone0_ramping;
		};
		u8  imc_zone0_config_vals[IMC_FAN_CONFIG_COUNT];
	};
	u8  imc_zone0_thresholds[IMC_FAN_THRESHOLD_COUNT];
	u8  imc_zone0_fanspeeds[IMC_FAN_SPEED_COUNT];

	union {
		struct {
		u8  imc_zone1_mode1;
		u8  imc_zone1_mode2;
		u8  imc_zone1_temp_offset;
		u8  imc_zone1_hysteresis;
		u8  imc_zone1_smbus_addr;
		u8  imc_zone1_smbus_num;
		u8  imc_zone1_pwm_step;
		u8  imc_zone1_ramping;
		};
		u8  imc_zone1_config_vals[IMC_FAN_CONFIG_COUNT];
	};
	u8  imc_zone1_thresholds[IMC_FAN_THRESHOLD_COUNT];
	u8  imc_zone1_fanspeeds[IMC_FAN_SPEED_COUNT];

	union {
		struct {
			u8  imc_zone2_mode1;
			u8  imc_zone2_mode2;
			u8  imc_zone2_temp_offset;
			u8  imc_zone2_hysteresis;
			u8  imc_zone2_smbus_addr;
			u8  imc_zone2_smbus_num;
			u8  imc_zone2_pwm_step;
			u8  imc_zone2_ramping;
		};
		u8  imc_zone2_config_vals[IMC_FAN_CONFIG_COUNT];
	};
	u8  imc_zone2_thresholds[IMC_FAN_THRESHOLD_COUNT];
	u8  imc_zone2_fanspeeds[IMC_FAN_SPEED_COUNT];

	union {
		struct {
			u8  imc_zone3_mode1;
			u8  imc_zone3_mode2;
			u8  imc_zone3_temp_offset;
			u8  imc_zone3_hysteresis;
			u8  imc_zone3_smbus_addr;
			u8  imc_zone3_smbus_num;
			u8  imc_zone3_pwm_step;
			u8  imc_zone3_ramping;
		};
		u8 imc_zone3_config_vals[IMC_FAN_CONFIG_COUNT];
	};
	u8  imc_zone3_thresholds[IMC_FAN_THRESHOLD_COUNT];
	u8  imc_zone3_fanspeeds[IMC_FAN_SPEED_COUNT];

	u32 imc_tempin0_at;
	u32 imc_tempin0_ct;
	u8 imc_tempin0_tuning_param;

	u32 imc_tempin1_at;
	u32 imc_tempin1_ct;
	u8  imc_tempin1_tuning_param;

	u32 imc_tempin2_at;
	u32 imc_tempin2_ct;
	u8  imc_tempin2_tuning_param;

	u32 imc_tempin3_at;
	u32 imc_tempin3_ct;
	u8  imc_tempin3_tuning_param;

};
#endif /* _CIMX_SB800_CHIP_H_ */
