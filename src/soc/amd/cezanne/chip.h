/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CEZANNE_CHIP_H
#define CEZANNE_CHIP_H

#include <amdblocks/chip.h>
#include <soc/i2c.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <types.h>

struct soc_amd_cezanne_config {
	struct soc_amd_common_config common_config;
	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[I2C_CTRLR_COUNT];

	/* Enable S0iX support */
	bool s0ix_enable;

	enum {
		DOWNCORE_AUTO = 0,
		DOWNCORE_1 = 1, /* Run with 1 physical core */
		DOWNCORE_2 = 3, /* Run with 2 physical cores */
		DOWNCORE_3 = 4, /* Run with 3 physical cores */
		DOWNCORE_4 = 6, /* Run with 4 physical cores */
		DOWNCORE_5 = 8, /* Run with 5 physical cores */
		DOWNCORE_6 = 9, /* Run with 6 physical cores */
		DOWNCORE_7 = 10, /* Run with 7 physical cores */
	} downcore_mode;
	bool disable_smt; /* disable second thread on all physical cores */

	uint8_t stt_control;
	uint8_t stt_pcb_sensor_count;
	uint16_t stt_min_limit;
	uint16_t stt_m1;
	uint16_t stt_m2;
	uint16_t stt_m3;
	uint16_t stt_m4;
	uint16_t stt_m5;
	uint16_t stt_m6;
	uint16_t stt_c_apu;
	uint16_t stt_c_gpu;
	uint16_t stt_c_hs2;
	uint16_t stt_alpha_apu;
	uint16_t stt_alpha_gpu;
	uint16_t stt_alpha_hs2;
	uint16_t stt_skin_temp_apu;
	uint16_t stt_skin_temp_gpu;
	uint16_t stt_skin_temp_hs2;
	uint16_t stt_error_coeff;
	uint16_t stt_error_rate_coefficient;

	uint8_t stapm_boost;
	uint32_t stapm_time_constant;
	uint32_t apu_only_sppt_limit;
	uint32_t sustained_power_limit;
	uint32_t fast_ppt_limit;
	uint32_t slow_ppt_limit;

	uint8_t smartshift_enable;

	uint8_t system_configuration;

	uint8_t cppc_ctrl;
	uint8_t cppc_perf_limit_max_range;
	uint8_t cppc_perf_limit_min_range;
	uint8_t cppc_epp_max_range;
	uint8_t cppc_epp_min_range;
	uint8_t cppc_preferred_cores;
};

#endif /* CEZANNE_CHIP_H */
