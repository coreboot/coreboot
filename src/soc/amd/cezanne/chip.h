/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CEZANNE_CHIP_H
#define CEZANNE_CHIP_H

#include <amdblocks/chip.h>
#include <amdblocks/i2c.h>
#include <gpio.h>
#include <soc/i2c.h>
#include <soc/southbridge.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <types.h>
#include <vendorcode/amd/fsp/cezanne/FspUsb.h>

enum gpp_clk_req {
	GPP_CLK_ON,  /* GPP clock always on; default */
	GPP_CLK_REQ, /* GPP clock controlled by corresponding #CLK_REQx pin */
	GPP_CLK_OFF, /* GPP clk off */
};

struct soc_amd_cezanne_config {
	struct soc_amd_common_config common_config;
	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[I2C_CTRLR_COUNT];
	struct i2c_pad_control i2c_pad[I2C_CTRLR_COUNT];

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
	uint32_t stapm_time_constant_s;
	uint32_t apu_only_sppt_limit;
	uint32_t sustained_power_limit_mW;
	uint32_t fast_ppt_limit_mW;
	uint32_t slow_ppt_limit_mW;
	uint32_t slow_ppt_time_constant_s;
	uint32_t thermctl_limit_degreeC;

	uint8_t smartshift_enable;

	uint8_t system_configuration;

	uint8_t cppc_ctrl;
	uint8_t cppc_perf_limit_max_range;
	uint8_t cppc_perf_limit_min_range;
	uint8_t cppc_epp_max_range;
	uint8_t cppc_epp_min_range;
	uint8_t cppc_preferred_cores;

	/* telemetry settings */
	uint32_t telemetry_vddcrvddfull_scale_current_mA;
	uint32_t telemetry_vddcrvddoffset;
	uint32_t telemetry_vddcrsocfull_scale_current_mA;
	uint32_t telemetry_vddcrsocoffset;

	/* Enable dptc for tablet mode */
	bool dptc_enable;

	/* STAPM Configuration for tablet mode (need enable dptc_enable first) */
	uint32_t fast_ppt_limit_tablet_mode_mW;
	uint32_t slow_ppt_limit_tablet_mode_mW;
	uint32_t sustained_power_limit_tablet_mode_mW;
	uint32_t thermctl_limit_tablet_mode_degreeC;

	/* The array index is the general purpose PCIe clock output number. Values in here
	   aren't the values written to the register to have the default to be always on. */
	enum gpp_clk_req gpp_clk_config[GPP_CLK_OUTPUT_COUNT];

	/* performance policy for the PCIe links: power consumption vs. link speed */
	enum {
		DXIO_PSPP_DISABLED = 0,
		DXIO_PSPP_PERFORMANCE,
		DXIO_PSPP_BALANCED,
		DXIO_PSPP_POWERSAVE,
	} pspp_policy;

	uint8_t usb_phy_custom;
	struct usb_phy_config usb_phy;

	/* eDP phy tuning settings */
	uint8_t edp_phy_override;
	/* bit vector of phy, bit0=1: DP0, bit1=1: DP1, bit2=1: DP2 bit3=1: DP3 */
	uint8_t edp_physel;

	struct {
		uint8_t dp_vs_pemph_level;
		uint8_t tx_eq_main;
		uint8_t tx_eq_pre;
		uint8_t tx_eq_post;
		uint8_t tx_vboost_lvl;
	} edp_tuningset;
};

#endif /* CEZANNE_CHIP_H */
