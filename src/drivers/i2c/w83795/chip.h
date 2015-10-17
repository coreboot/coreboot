/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Raptor Engineering
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
 * Foundation, Inc.
 */

struct drivers_i2c_w83795_config {
	uint8_t fanin_ctl1;
	uint8_t fanin_ctl2;

	uint8_t temp_ctl1;
	uint8_t temp_ctl2;
	uint8_t temp_dtse;

	uint8_t volt_ctl1;
	uint8_t volt_ctl2;

	uint8_t temp1_fan_select;
	uint8_t temp2_fan_select;
	uint8_t temp3_fan_select;
	uint8_t temp4_fan_select;
	uint8_t temp5_fan_select;
	uint8_t temp6_fan_select;

	uint8_t temp1_source_select;
	uint8_t temp2_source_select;
	uint8_t temp3_source_select;
	uint8_t temp4_source_select;
	uint8_t temp5_source_select;
	uint8_t temp6_source_select;

	uint32_t vcore1_high_limit_mv;		/* mV */
	uint32_t vcore1_low_limit_mv;		/* mV */
	uint32_t vcore2_high_limit_mv;		/* mV */
	uint32_t vcore2_low_limit_mv;		/* mV */
	uint32_t vtt_high_limit_mv;		/* mV */
	uint32_t vtt_low_limit_mv;		/* mV */
	uint32_t vsen3_high_limit_mv;		/* mV */
	uint32_t vsen3_low_limit_mv;		/* mV */
	uint32_t vsen4_high_limit_mv;		/* mV */
	uint32_t vsen4_low_limit_mv;		/* mV */
	uint32_t vsen5_high_limit_mv;		/* mV */
	uint32_t vsen5_low_limit_mv;		/* mV */
	uint32_t vsen6_high_limit_mv;		/* mV */
	uint32_t vsen6_low_limit_mv;		/* mV */
	uint32_t vsen7_high_limit_mv;		/* mV */
	uint32_t vsen7_low_limit_mv;		/* mV */
	uint32_t vsen8_high_limit_mv;		/* mV */
	uint32_t vsen8_low_limit_mv;		/* mV */
	uint32_t vsen9_high_limit_mv;		/* mV */
	uint32_t vsen9_low_limit_mv;		/* mV */
	uint32_t vsen10_high_limit_mv;		/* mV */
	uint32_t vsen10_low_limit_mv;		/* mV */
	uint32_t vsen11_high_limit_mv;		/* mV */
	uint32_t vsen11_low_limit_mv;		/* mV */
	uint32_t vsen12_high_limit_mv;		/* mV */
	uint32_t vsen12_low_limit_mv;		/* mV */
	uint32_t vsen13_high_limit_mv;		/* mV */
	uint32_t vsen13_low_limit_mv;		/* mV */
	uint32_t vdd_high_limit_mv;		/* mV */
	uint32_t vdd_low_limit_mv;		/* mV */
	uint32_t vsb_high_limit_mv;		/* mV */
	uint32_t vsb_low_limit_mv;		/* mV */
	uint32_t vbat_high_limit_mv;		/* mV */
	uint32_t vbat_low_limit_mv;		/* mV */

	int8_t tr1_critical_temperature;	/* °C */
	int8_t tr1_critical_hysteresis;		/* °C */
	int8_t tr1_warning_temperature;		/* °C */
	int8_t tr1_warning_hysteresis;		/* °C */
	int8_t tr2_critical_temperature;	/* °C */
	int8_t tr2_critical_hysteresis;		/* °C */
	int8_t tr2_warning_temperature;		/* °C */
	int8_t tr2_warning_hysteresis;		/* °C */
	int8_t tr3_critical_temperature;	/* °C */
	int8_t tr3_critical_hysteresis;		/* °C */
	int8_t tr3_warning_temperature;		/* °C */
	int8_t tr3_warning_hysteresis;		/* °C */
	int8_t tr4_critical_temperature;	/* °C */
	int8_t tr4_critical_hysteresis;		/* °C */
	int8_t tr4_warning_temperature;		/* °C */
	int8_t tr4_warning_hysteresis;		/* °C */
	int8_t tr5_critical_temperature;	/* °C */
	int8_t tr5_critical_hysteresis;		/* °C */
	int8_t tr5_warning_temperature;		/* °C */
	int8_t tr5_warning_hysteresis;		/* °C */
	int8_t tr6_critical_temperature;	/* °C */
	int8_t tr6_critical_hysteresis;		/* °C */
	int8_t tr6_warning_temperature;		/* °C */
	int8_t tr6_warning_hysteresis;		/* °C */
	int8_t dts_critical_temperature;	/* °C */
	int8_t dts_critical_hysteresis;		/* °C */
	int8_t dts_warning_temperature;		/* °C */
	int8_t dts_warning_hysteresis;		/* °C */

	int8_t temp1_critical_temperature;	/* °C */
	int8_t temp2_critical_temperature;	/* °C */
	int8_t temp3_critical_temperature;	/* °C */
	int8_t temp4_critical_temperature;	/* °C */
	int8_t temp5_critical_temperature;	/* °C */
	int8_t temp6_critical_temperature;	/* °C */

	int8_t temp1_target_temperature;	/* °C */
	int8_t temp2_target_temperature;	/* °C */
	int8_t temp3_target_temperature;	/* °C */
	int8_t temp4_target_temperature;	/* °C */
	int8_t temp5_target_temperature;	/* °C */
	int8_t temp6_target_temperature;	/* °C */

	uint8_t fan1_nonstop;			/* % of full speed (0-100) */
	uint8_t fan2_nonstop;			/* % of full speed (0-100) */
	uint8_t fan3_nonstop;			/* % of full speed (0-100) */
	uint8_t fan4_nonstop;			/* % of full speed (0-100) */
	uint8_t fan5_nonstop;			/* % of full speed (0-100) */
	uint8_t fan6_nonstop;			/* % of full speed (0-100) */
	uint8_t fan7_nonstop;			/* % of full speed (0-100) */
	uint8_t fan8_nonstop;			/* % of full speed (0-100) */

	uint8_t default_speed;			/* % of full speed (0-100) */

	uint8_t fan1_duty;			/* % of full speed (0-100) */
	uint8_t fan2_duty;			/* % of full speed (0-100) */
	uint8_t fan3_duty;			/* % of full speed (0-100) */
	uint8_t fan4_duty;			/* % of full speed (0-100) */
	uint8_t fan5_duty;			/* % of full speed (0-100) */
	uint8_t fan6_duty;			/* % of full speed (0-100) */
	uint8_t fan7_duty;			/* % of full speed (0-100) */
	uint8_t fan8_duty;			/* % of full speed (0-100) */

	uint8_t smbus_aux;			/* 0   == device located on primary SMBUS,
						 * 1   == device located on first auxiliary
						 *        SMBUS channel,
						 * <n> == device located on <n> auxiliary
						 *        SMBUS channel
						 */
};
