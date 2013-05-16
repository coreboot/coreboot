/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
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

/* EXYNOS - Thermal Management Unit */

#ifndef CPU_SAMSUNG_EXYNOS5420_TMU_H
#define CPU_SAMSUNG_EXYNOS5420_TMU_H

struct tmu_reg {
	unsigned triminfo;
	unsigned rsvd1;
	unsigned rsvd2;
	unsigned rsvd3;
	unsigned rsvd4;
	unsigned triminfo_control;
	unsigned rsvd5;
	unsigned rsvd6;
	unsigned tmu_control;
	unsigned rsvd7;
	unsigned tmu_status;
	unsigned sampling_internal;
	unsigned counter_value0;
	unsigned counter_value1;
	unsigned rsvd8;
	unsigned rsvd9;
	unsigned current_temp;
	unsigned rsvd10;
	unsigned rsvd11;
	unsigned rsvd12;
	unsigned threshold_temp_rise;
	unsigned threshold_temp_fall;
	unsigned rsvd13;
	unsigned rsvd14;
	unsigned past_temp3_0;
	unsigned past_temp7_4;
	unsigned past_temp11_8;
	unsigned past_temp15_12;
	unsigned inten;
	unsigned intstat;
	unsigned intclear;
	unsigned rsvd15;
	unsigned emul_con;
};

enum tmu_status_t {
	TMU_STATUS_INIT = 0,
	TMU_STATUS_NORMAL,
	TMU_STATUS_WARNING,
	TMU_STATUS_TRIPPED,
};

/* Tmeperature threshold values for various thermal events */
struct temperature_params {
	/* minimum value in temperature code range */
	unsigned int min_val;
	/* maximum value in temperature code range */
	unsigned int max_val;
	/* temperature threshold to start warning */
	unsigned int start_warning;
	/* temperature threshold CPU tripping */
	unsigned int start_tripping;
	/* temperature threshold for HW tripping */
	unsigned int hardware_tripping;
};

/* Pre-defined values and thresholds for calibration of current temperature */
struct tmu_data {
	/* pre-defined temperature thresholds */
	struct temperature_params ts;
	/* pre-defined efuse range minimum value */
	unsigned int efuse_min_value;
	/* pre-defined efuse value for temperature calibration */
	unsigned int efuse_value;
	/* pre-defined efuse range maximum value */
	unsigned int efuse_max_value;
	/* current temperature sensing slope */
	unsigned int slope;
};

/* TMU device specific details and status */
struct tmu_info {
	/* base Address for the TMU */
	unsigned tmu_base;
	/* mux Address for the TMU */
	int tmu_mux;
	/* pre-defined values for calibration and thresholds */
	struct tmu_data data;
	/* value required for triminfo_25 calibration */
	unsigned int te1;
	/* value required for triminfo_85 calibration */
	unsigned int te2;
	/* TMU DC value for threshold calculation */
	int dc_value;
	/* enum value indicating status of the TMU */
	int tmu_state;
};

extern struct tmu_info *tmu_info;

/*
 * Monitors status of the TMU device and exynos temperature
 *
 * @info	pointer to TMU info struct
 * @temp	pointer to the current temperature value
 * @return	enum tmu_status_t value, code indicating event to execute
 *		and -1 on error
 */
enum tmu_status_t tmu_monitor(struct tmu_info *info, int *temp);

/*
 * Initialize TMU device
 *
 * @info	pointer to TMU info struct
 * @return	int value, 0 for success
 */
int tmu_init(struct tmu_info *info);

#endif	/* CPU_SAMSUNG_EXYNOS5420_TMU_H */
