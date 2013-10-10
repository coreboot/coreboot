/*
 * Copyright (c) 2010 - 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TEGRA124_PMC_H_
#define _TEGRA124_PMC_H_

#include <stdint.h>

enum {
	POWER_PARTID_CRAIL = 0,
	POWER_PARTID_TD = 1,
	POWER_PARTID_VE = 2,
	POWER_PARTID_VDE = 4,
	POWER_PARTID_L2C = 5,
	POWER_PARTID_MPE = 6,
	POWER_PARTID_HEG = 7,
	POWER_PARTID_CE1 = 9,
	POWER_PARTID_CE2 = 10,
	POWER_PARTID_CE3 = 11,
	POWER_PARTID_CELP = 12,
	POWER_PARTID_CE0 = 14,
	POWER_PARTID_C0NC = 15,
	POWER_PARTID_C1NC = 16,
	POWER_PARTID_DIS = 18,
	POWER_PARTID_DISB = 19,
	POWER_PARTID_XUSBA = 20,
	POWER_PARTID_XUSBB = 21,
	POWER_PARTID_XUSBC = 22
};

struct tegra_pmc_regs {
	u32 cntrl;
	u32 sec_disable;
	u32 pmc_swrst;
	u32 wake_mask;
	u32 wake_lvl;
	u32 wake_status;
	u32 sw_wake_status;
	u32 dpd_pads_oride;
	u32 dpd_sample;
	u32 dpd_enable;
	u32 pwrgate_timer_off;
	u32 clamp_status;
	u32 pwrgate_toggle;
	u32 remove_clamping_cmd;
	u32 pwrgate_status;
	u32 pwrgood_timer;
	u32 blink_timer;
	u32 no_iopower;
	u32 pwr_det;
	u32 pwr_det_latch;
	u32 scratch[24];
	u32 secure_scratch[6];
	u32 cpupwrgood_timer;
	u32 cpupwroff_timer;
	u32 pg_mask;
	u32 pg_mask_1;
	u32 auto_wake_lvl;
	u32 auto_wake_lvl_mask;
	u32 wake_delay;
	u32 pwr_det_val;
	u32 ddr_pwr;
	u32 usb_debounce_del;
	u32 usb_a0;
	u32 crypto_op;
	u32 pllp_wb0_override;
	u32 scratch24[43 - 24];
	u32 bondout_mirror[3];
	u32 sys_33v_en;
	u32 bondout_mirror_access;
	u32 gate;
	u32 wake2_mask;
	u32 wake2_lvl;
	u32 wake2_status;
	u32 sw_wake2_status;
	u32 auto_wake2_lvl_mask;
	u32 pg_mask_2;
	u32 pg_mask_ce1;
	u32 pg_mask_ce2;
	u32 pg_mask_ce3;
	u32 pwrgate_timer_ce[7];
	u32 pcx_edpd_cntrl;
	u32 osc_edpd_over;
	u32 clk_out_cntrl;
	u32 sata_pwrgt;
	u32 sensor_ctrl;
	u32 rst_status;
	u32 io_dpd_req;
	u32 io_dpd_status;
	u32 io_dpd2_req;
	u32 io_dpd2_status;
	u32 sel_dpd_tim;
	u32 vddp_sel;
	u32 ddr_cfg;
	u32 e_no_vttgen;
	u8 _rsv0[4];
	u32 pllm_wb0_override_freq;
	u32 test_pwrgate;
	u32 pwrgate_timer_mult;
	u32 dis_sel_dpd;
	u32 utmip_uhsic_triggers;
	u32 utmip_uhsic_saved_state;
	u32 utmip_pad_cfg;
	u32 utmip_term_pad_cfg;
	u32 utmip_uhsic_sleep_cfg;
	u32 utmip_uhsic_sleepwalk_cfg;
	u32 utmip_sleepwalk_p[3];
	u32 uhsic_sleepwalk_p0;
	u32 utmip_uhsic_status;
	u32 utmip_uhsic_fake;
	u32 bondout_mirror3[5 - 3];
	u32 secure_scratch6[8 - 6];
	u32 scratch43[56 - 43];
	u32 scratch_eco[3];
	u32 utmip_uhsic_line_wakeup;
	u32 utmip_bias_master_cntrl;
	u32 utmip_master_config;
	u32 td_pwrgate_inter_part_timer;
	u32 utmip_uhsic2_triggers;
	u32 utmip_uhsic2_saved_state;
	u32 utmip_uhsic2_sleep_cfg;
	u32 utmip_uhsic2_sleepwalk_cfg;
	u32 uhsic2_sleepwalk_p1;
	u32 utmip_uhsic2_status;
	u32 utmip_uhsic2_fake;
	u32 utmip_uhsic2_line_wakeup;
	u32 utmip_master2_config;
	u32 utmip_uhsic_rpd_cfg;
	u32 pg_mask_ce0;
	u32 pg_mask3[5 - 3];
	u32 pllm_wb0_override2;
	u32 tsc_mult;
	u32 cpu_vsense_override;
	u32 glb_amap_cfg;
	u32 sticky_bits;
	u32 sec_disable2;
	u32 weak_bias;
	u32 reg_short;
	u32 pg_mask_andor;
	u8 _rsv1[0x2c];
	u32 secure_scratch8[24 - 8];
	u32 scratch56[120 - 56];
};

enum {
	PMC_PWRGATE_TOGGLE_PARTID_MASK = 0x1f,
	PMC_PWRGATE_TOGGLE_PARTID_SHIFT = 0,
	PMC_PWRGATE_TOGGLE_START = 0x1 << 8
};

enum {
	PMC_CNTRL_KBC_CLK_DIS = 0x1 << 0,
	PMC_CNTRL_RTC_CLK_DIS = 0x1 << 1,
	PMC_CNTRL_RTC_RST = 0x1 << 2,
	PMC_CNTRL_KBC_RST = 0x1 << 3,
	PMC_CNTRL_MAIN_RST = 0x1 << 4,
	PMC_CNTRL_LATCHWAKE_EN = 0x1 << 5,
	PMC_CNTRL_GLITCHDET_DIS = 0x1 << 6,
	PMC_CNTRL_BLINK_EN = 0x1 << 7,
	PMC_CNTRL_PWRREQ_POLARITY = 0x1 << 8,
	PMC_CNTRL_PWRREQ_OE = 0x1 << 9,
	PMC_CNTRL_SYSCLK_POLARITY = 0x1 << 10,
	PMC_CNTRL_SYSCLK_OE = 0x1 << 11,
	PMC_CNTRL_PWRGATE_DIS = 0x1 << 12,
	PMC_CNTRL_AOINIT = 0x1 << 13,
	PMC_CNTRL_SIDE_EFFECT_LP0 = 0x1 << 14,
	PMC_CNTRL_CPUPWRREQ_POLARITY = 0x1 << 15,
	PMC_CNTRL_CPUPWRREQ_OE = 0x1 << 16,
	PMC_CNTRL_INTR_POLARITY = 0x1 << 17,
	PMC_CNTRL_FUSE_OVERRIDE = 0x1 << 18,
	PMC_CNTRL_CPUPWRGOOD_EN = 0x1 << 19,
	PMC_CNTRL_CPUPWRGOOD_SEL_SHIFT = 20,
	PMC_CNTRL_CPUPWRGOOD_SEL_MASK =
		0x3 << PMC_CNTRL_CPUPWRGOOD_SEL_SHIFT
};

enum {
	PMC_CNTRL2_HOLD_CKE_LOW_EN = 0x1 << 12
};

enum {
	PMC_OSC_EDPD_OVER_XOFS_SHIFT = 1,
	PMC_OSC_EDPD_OVER_XOFS_MASK =
		0x3f << PMC_OSC_EDPD_OVER_XOFS_SHIFT
};

#endif	/* _TEGRA124_PMC_H_ */
