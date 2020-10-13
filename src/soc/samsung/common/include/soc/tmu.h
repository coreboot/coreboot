/* SPDX-License-Identifier: GPL-2.0-only */

/* EXYNOS - Thermal Management Unit */

#ifndef SOC_SAMSUNG_COMMON_INCLUDE_SOC_TMU_H
#define SOC_SAMSUNG_COMMON_INCLUDE_SOC_TMU_H

struct tmu_reg {
	unsigned int triminfo;
	unsigned int rsvd1;
	unsigned int rsvd2;
	unsigned int rsvd3;
	unsigned int rsvd4;
	unsigned int triminfo_control;
	unsigned int rsvd5;
	unsigned int rsvd6;
	unsigned int tmu_control;
	unsigned int rsvd7;
	unsigned int tmu_status;
	unsigned int sampling_internal;
	unsigned int counter_value0;
	unsigned int counter_value1;
	unsigned int rsvd8;
	unsigned int rsvd9;
	unsigned int current_temp;
	unsigned int rsvd10;
	unsigned int rsvd11;
	unsigned int rsvd12;
	unsigned int threshold_temp_rise;
	unsigned int threshold_temp_fall;
	unsigned int rsvd13;
	unsigned int rsvd14;
	unsigned int past_temp3_0;
	unsigned int past_temp7_4;
	unsigned int past_temp11_8;
	unsigned int past_temp15_12;
	unsigned int inten;
	unsigned int intstat;
	unsigned int intclear;
	unsigned int rsvd15;
	unsigned int emul_con;
};
check_member(tmu_reg, emul_con, 0x80);

enum tmu_status_t {
	TMU_STATUS_INIT = 0,
	TMU_STATUS_NORMAL,
	TMU_STATUS_WARNING,
	TMU_STATUS_TRIPPED,
};

/* Temperature threshold values for various thermal events */
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
	unsigned int tmu_base;
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

#endif	/* SOC_SAMSUNG_COMMON_INCLUDE_SOC_TMU_H */
