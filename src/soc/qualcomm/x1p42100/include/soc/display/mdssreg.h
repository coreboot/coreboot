// SPDX-License-Identifier: GPL-2.0-only
#ifndef _SOC_DISPLAY_MDSS_REG_H_
#define _SOC_DISPLAY_MDSS_REG_H_

#include <types.h>
#include <edid.h>
#include <soc/addressmap.h>

/*
 * mdss_panel_config - topology parameters passed from panel driver to shared
 * MDP functions (sspp.c, mdp_intf_TG.c).  Keeps DSI-specific and eDP-specific
 * paths separate without duplicating the MDP setup code.
 *
 * pipe_count      : 1 = single pipe (eDP), 2 = dual pipe (DSI 2-2-x)
 * dsc_ratio_x1000 : 0 = no DSC (eDP), 3000 = 3:1, 3750 = 3.75:1
 *                   Formula: (bpc * 3 * 1000) / bpp  (UEFI MDPLib.c)
 * intf_num        : interface index (1 = DSI INTF_1, 5 = eDP INTF_5)
 *                   BIT(intf_num) gives both INTF_FLUSH and INTF_ACTIVE.
 */
struct mdss_panel_config {
	uint8_t  pipe_count;       /* 1=single pipe (eDP), 2=dual pipe (DSI 2-2-x) */
	uint32_t dsc_ratio_x1000;  /* 0=no DSC, 3000=3:1, 3750=3.75:1 */
	uint8_t  intf_num;         /* primary INTF index: 1=DSI0/INTF_1, 5=eDP/INTF_5 */
	uint8_t  dsi_count;        /* 1=single DSI (2-2-1), 2=dual DSI (2-2-2) */
};

/*
 * Derive INTF_FLUSH and INTF_ACTIVE bitmasks from intf_num + dsi_count.
 * For 2-2-1 (dsi_count=1): only primary INTF -> BIT(intf_num)
 * For 2-2-2 (dsi_count=2): both INTF_1 and INTF_2 -> BIT(1)|BIT(2)
 *   intf_num=1 (primary/master), intf_num+1=2 (secondary/slave)
 * For eDP  (dsi_count=1): only INTF_5 -> BIT(5)
 */
#define MDSS_INTF_FLUSH(cfg) 	((cfg)->dsi_count > 1 ? 	 (BIT((cfg)->intf_num) | BIT((cfg)->intf_num + 1)) : 	  BIT((cfg)->intf_num))
#define MDSS_INTF_ACTIVE(cfg) MDSS_INTF_FLUSH(cfg)
#define MDSS_INTF_MASTER(cfg) BIT((cfg)->intf_num)  /* always primary only */

/*
 * Compute compressed active width from ha and DSC ratio.
 * Uses integer arithmetic matching UEFI dsiDriver.c:
 *   iVisWidthPx = ceil(ha * 1000 / dsc_ratio_x1000)
 * Works for 3:1 (3000), 3.75:1 (3750), 2.5:1 (2500), etc.
 * Returns ha unchanged when dsc_ratio_x1000 == 0 (no DSC).
 */
static inline uint32_t mdss_dsc_compressed_ha(uint32_t ha,
					   uint32_t dsc_ratio_x1000)
{
	if (dsc_ratio_x1000 == 0)
		return ha;
	return (ha * 1000 + dsc_ratio_x1000 - 1) / dsc_ratio_x1000;
}

#define MDSS_MAX_SINGLE_PIPE_PIXEL_WIDTH 2560U
#define MDSS_MAX_MDP_CLK 575000000U
#define MDSS_MDP_CLK_FUDGE_FACTOR_NUMER 105U
#define MDSS_MDP_CLK_FUDGE_FACTOR_DENOM 100U

#ifndef WRITE32_LOG
#define WRITE32_LOG(addr, val)                                                               \
	do {                                                                                 \
		uintptr_t __addr_tmp = (uintptr_t)(addr);                                    \
		uint32_t __val_tmp = (uint32_t)(val);                                        \
		write32((void *)__addr_tmp, __val_tmp);                                      \
		printk(BIOS_DEBUG, "[%s:%d] write32 addr=%p val=0x%X\n", __func__, __LINE__, \
		       (void *)__addr_tmp, (unsigned int)__val_tmp);                         \
	} while (0)
#endif

struct dsi_regs {
	uint32_t hw_version;
	uint32_t ctrl;
	uint32_t reserved0[2];
	uint32_t video_mode_ctrl;
	uint32_t reserved1[4];
	uint32_t video_mode_active_h;
	uint32_t video_mode_active_v;
	uint32_t video_mode_active_total;
	uint32_t video_mode_active_hsync;
	uint32_t video_mode_active_vsync;
	uint32_t video_mode_active_vsync_vpos;
	uint32_t cmd_mode_dma_ctrl;
	uint32_t cmd_mode_mdp_ctrl;
	uint32_t cmd_mode_mdp_dcs_cmd_ctrl;
	uint32_t dma_cmd_offset;
	uint32_t dma_cmd_length;
	uint32_t reserved2[2];
	uint32_t cmd_mode_mdp_stream0_ctrl;
	uint32_t cmd_mode_mdp_stream0_total;
	uint32_t cmd_mode_mdp_stream1_ctrl;
	uint32_t cmd_mode_mdp_stream1_total;
	uint32_t reserved4[7];
	uint32_t trig_ctrl;
	uint32_t reserved5[2];
	uint32_t cmd_mode_dma_sw_trigger;
	uint32_t reserved6[3];
	uint32_t misr_cmd_ctrl;
	uint32_t misr_video_ctrl;
	uint32_t lane_swap_ctrl; /* 0xA8 - LANE_SWAP_CTRL */
	uint32_t lane_ctrl;       /* 0xAC */
	uint32_t reserved7[3];
	uint32_t hs_timer_ctrl;
	uint32_t timeout_status;
	uint32_t clkout_timing_ctrl;
	uint32_t eot_packet;
	uint32_t eot_packet_ctrl;
	uint32_t reserved8[15];
	uint32_t err_int_mask0;
	uint32_t int_ctrl;
	uint32_t iobist_ctrl;
	uint32_t soft_reset;
	uint32_t clk_ctrl;
	uint32_t reserved9[15];
	uint32_t test_pattern_gen_ctrl;
	uint32_t reserved10[7];
	uint32_t test_pattern_gen_cmd_dma_init_val;
	uint32_t reserved11[14];
	uint32_t cmd_mode_mdp_ctrl2;
	uint32_t reserved12[12];
	uint32_t tpg_dma_fifo_reset;
	uint32_t reserved13[44];
	uint32_t video_compression_mode_ctrl;
	uint32_t video_compression_mode_ctrl2;
	uint32_t cmd_compression_mode_ctrl;
	uint32_t cmd_compression_mode_ctrl2;
	uint32_t cmd_compression_mode_ctrl3;
};

check_member(dsi_regs, video_mode_active_h, 0x24);
check_member(dsi_regs, cmd_mode_mdp_stream0_ctrl, 0x58);
check_member(dsi_regs, trig_ctrl, 0x84);
check_member(dsi_regs, cmd_mode_dma_sw_trigger, 0x90);
check_member(dsi_regs, misr_cmd_ctrl, 0xA0);
check_member(dsi_regs, lane_swap_ctrl, 0xA8);
check_member(dsi_regs, hs_timer_ctrl, 0xBC);
check_member(dsi_regs, err_int_mask0, 0x10C);
check_member(dsi_regs, test_pattern_gen_ctrl, 0x15C);
check_member(dsi_regs, test_pattern_gen_cmd_dma_init_val, 0x17C);
check_member(dsi_regs, cmd_mode_mdp_ctrl2, 0x1B8);
check_member(dsi_regs, tpg_dma_fifo_reset, 0x1EC);
check_member(dsi_regs, video_compression_mode_ctrl, 0x2A0);

/*
 * DSI PHY 2.7.0 common register block (MDSS_BASE + 0x95000).
 * Confirmed from UEFI HALdsi_Phy_2_7_0.h (Purwa/Hamoa).
 * NOTE: This layout differs significantly from sc7180's 10nm PHY.
 *   - VREG_CTRL split into VREG_CTRL_0 (0x20) and VREG_CTRL_1 (0x110)
 *   - New CTRL_3 (SLEEP_B, 0x30) and CTRL_4 (0x114)
 *   - New global strength/rescode/pemph registers
 *   - Timing ctrl extended to 14 entries (adds CLK_PRE[12], CLK_POST[13])
 *   - Lane regs: CFG0/CFG1/CFG2 (3 entries), no hstx/offset/lptx per-lane
 *   - Lane TX_DCTRL values: 0x40 (DLN0-2), 0x46 (DLN3), 0x41 (CLK)
 */
struct dsi_phy_regs {
	/* 0x00: revision IDs */
	uint32_t phy_cmn_revision_id0;          /* 0x00 */
	uint32_t phy_cmn_revision_id1;          /* 0x04 */
	uint32_t phy_cmn_revision_id2;          /* 0x08 */
	uint32_t phy_cmn_revision_id3;          /* 0x0C */
	/* 0x10: clock config */
	uint32_t phy_cmn_clk_cfg0;              /* 0x10 */
	uint32_t phy_cmn_clk_cfg1;              /* 0x14 */
	/* 0x18: global control */
	uint32_t phy_cmn_glbl_ctrl;             /* 0x18 */
	uint32_t phy_cmn_rbuf_ctrl;             /* 0x1C */
	/* 0x20: vreg ctrl 0 (0P4) */
	uint32_t phy_cmn_vreg_ctrl_0;           /* 0x20 */
	/* 0x24: ctrl 0 (power-up / lane shutdown) */
	uint32_t phy_cmn_ctrl0;                 /* 0x24 */
	/* 0x28: ctrl 1 (SW reset) */
	uint32_t phy_cmn_ctrl1;                 /* 0x28 */
	/* 0x2C: ctrl 2 (DCTRL) */
	uint32_t phy_cmn_ctrl2;                 /* 0x2C */
	/* 0x30: ctrl 3 (SLEEP_B) */
	uint32_t phy_cmn_ctrl3;                 /* 0x30 */
	uint32_t reserved0[2];                  /* 0x34, 0x38 */
	/* 0x3C: PLL control */
	uint32_t phy_cmn_pll_ctrl;              /* 0x3C */
	uint32_t reserved1[24];                 /* 0x40..0x9C */
	/* 0xA0: DSI lane ctrl 0 */
	uint32_t phy_cmn_dsi_lane_ctrl0;        /* 0xA0 */
	uint32_t reserved2[4];                  /* 0xA4..0xB0 */
	/* 0xB4: timing ctrl 0..13 (14 registers) */
	uint32_t phy_cmn_timing_ctrl[14];       /* 0xB4..0xE8 */
	/* 0xEC: global HSTX strength ctrl 0 */
	uint32_t phy_cmn_glbl_hstx_str_ctrl_0;  /* 0xEC */
	uint32_t phy_cmn_glbl_hstx_str_ctrl_1;  /* 0xF0 */
	/* 0xF4: rescode offset top/mid/bot */
	uint32_t phy_cmn_glbl_rescode_offset_top_ctrl; /* 0xF4 */
	uint32_t phy_cmn_glbl_rescode_offset_mid_ctrl; /* 0xF8 */
	uint32_t phy_cmn_glbl_rescode_offset_bot_ctrl; /* 0xFC */
	/* 0x100: LPTX strength */
	uint32_t phy_cmn_glbl_lptx_str_ctrl;    /* 0x100 */
	/* 0x104: pemph ctrl 0/1 */
	uint32_t phy_cmn_glbl_pemph_ctrl_0;     /* 0x104 */
	uint32_t phy_cmn_glbl_pemph_ctrl_1;     /* 0x108 */
	/* 0x10C: SWI cal sel */
	uint32_t phy_cmn_glbl_str_swi_cal_sel_ctrl; /* 0x10C */
	/* 0x110: vreg ctrl 1 (0P2) */
	uint32_t phy_cmn_vreg_ctrl_1;           /* 0x110 */
	/* 0x114: ctrl 4 (DCTRL_GLBL_2) */
	uint32_t phy_cmn_ctrl4;                 /* 0x114 */
	uint32_t reserved3[10];                 /* 0x118..0x13C */
	/* 0x140: PHY status (refgen ready) */
	uint32_t phy_cmn_phy_status;            /* 0x140 */
	uint32_t reserved4[26];                 /* 0x144..0x1A8 */
	/* 0x1AC: DIGTOP spare10 (refgen request) */
	uint32_t phy_cmn_glbl_digtop_spare10;   /* 0x1AC */
	uint32_t reserved5[20];                 /* 0x1B0..0x1FC (20*4=80) */
	/*
	 * 0x200: lane registers (5 lanes: DLN0-3 + CLK, stride 0x80)
	 * Offsets confirmed from UEFI HALdsi_Phy_2_7_0.h DLN0 HWIO defines
	 */
	struct {
		uint32_t dln0_cfg0;              /* +0x00 */
		uint32_t dln0_cfg1;              /* +0x04 */
		uint32_t dln0_cfg2;              /* +0x08 */
		uint32_t dln0_test_datapath;     /* +0x0C */
		uint32_t dln0_pin_swap;          /* +0x10 */
		uint32_t dln0_lprx_ctrl;         /* +0x14 */
		uint32_t dln0_tx_dctrl;          /* +0x18 */
		uint32_t reserved_ln0[25];       /* +0x1C..0x7C */
	} phy_ln_regs[5];
};

check_member(dsi_phy_regs, phy_cmn_clk_cfg0, 0x10);
check_member(dsi_phy_regs, phy_cmn_ctrl3, 0x30);
check_member(dsi_phy_regs, phy_cmn_pll_ctrl, 0x3C);
check_member(dsi_phy_regs, phy_cmn_dsi_lane_ctrl0, 0xA0);
check_member(dsi_phy_regs, phy_cmn_timing_ctrl[0], 0xB4);
check_member(dsi_phy_regs, phy_cmn_glbl_hstx_str_ctrl_0, 0xEC);
check_member(dsi_phy_regs, phy_cmn_glbl_rescode_offset_top_ctrl, 0xF4);
check_member(dsi_phy_regs, phy_cmn_glbl_lptx_str_ctrl, 0x100);
check_member(dsi_phy_regs, phy_cmn_vreg_ctrl_1, 0x110);
check_member(dsi_phy_regs, phy_cmn_ctrl4, 0x114);
check_member(dsi_phy_regs, phy_cmn_phy_status, 0x140);
check_member(dsi_phy_regs, phy_cmn_glbl_digtop_spare10, 0x1AC);
check_member(dsi_phy_regs, phy_ln_regs[0], 0x200);
check_member(dsi_phy_regs, phy_ln_regs[1], 0x280);
check_member(dsi_phy_regs, phy_ln_regs[2], 0x300);
check_member(dsi_phy_regs, phy_ln_regs[3], 0x380);
check_member(dsi_phy_regs, phy_ln_regs[4], 0x400);

/*
 * DSI PHY 2.7.0 PLL (QDIG PLL) register block.
 * Base: MDSS_BASE + 0x95500 (PHY_BASE + 0x500).
 * Confirmed from UEFI HALdsi_Phy_2_7_0.h offsets.
 * Only registers used by coreboot DSI init are included;
 * gaps are covered by reserved padding.
 */
struct dsi_phy_pll_qlink_regs {
	uint32_t pll_analog_controls_one;        /* 0x000 */
	uint32_t pll_analog_controls_two;        /* 0x004 */
	uint32_t pll_int_loop_settings;          /* 0x008 */
	uint32_t pll_int_loop_settings_two;      /* 0x00C */
	uint32_t pll_analog_controls_three;      /* 0x010 */
	uint32_t pll_analog_controls_four;       /* 0x014 */
	uint32_t pll_analog_controls_five;       /* 0x018 */
	uint32_t pll_int_loop_controls;          /* 0x01C */
	uint32_t pll_dsm_divider;               /* 0x020 */
	uint32_t pll_feedback_divider;           /* 0x024 */
	uint32_t pll_system_muxes;              /* 0x028 */
	uint32_t pll_freq_update_ctrl_overrides; /* 0x02C */
	uint32_t pll_cmode;                      /* 0x030 */
	uint32_t reserved0[3];                   /* 0x034..0x03C */
	uint32_t pll_cntrl;                      /* 0x040 (PLL_START) */
	uint32_t pll_calibration_settings;       /* 0x044 */
	uint32_t pll_band_sel_cal_timer_low;     /* 0x048 */
	uint32_t pll_band_sel_cal_timer_high;    /* 0x04C */
	uint32_t pll_band_sel_cal_settings;      /* 0x050 */
	uint32_t pll_band_sel_min;               /* 0x054 */
	uint32_t pll_band_sel_max;               /* 0x058 */
	uint32_t pll_band_sel_pfilt;             /* 0x05C */
	uint32_t pll_band_sel_ifilt;             /* 0x060 */
	uint32_t pll_band_sel_cal_settings_two;  /* 0x064 */
	uint32_t pll_band_sel_cal_settings_three;/* 0x068 */
	uint32_t pll_band_sel_cal_settings_four; /* 0x06C */
	uint32_t pll_band_sel_icode_high;        /* 0x070 */
	uint32_t pll_band_sel_icode_low;         /* 0x074 */
	uint32_t pll_freq_detect_settings_one;   /* 0x078 */
	uint32_t pll_freq_detect_thresh;         /* 0x07C */
	uint32_t pll_freq_det_refclk_high;       /* 0x080 */
	uint32_t pll_freq_det_refclk_low;        /* 0x084 */
	uint32_t pll_freq_det_pllclk_high;       /* 0x088 */
	uint32_t pll_freq_det_pllclk_low;        /* 0x08C */
	uint32_t pll_pfilt;                      /* 0x090 */
	uint32_t pll_ifilt;                      /* 0x094 */
	uint32_t pll_pll_gain;                   /* 0x098 */
	uint32_t pll_icode_low;                  /* 0x09C */
	uint32_t pll_icode_high;                 /* 0x0A0 */
	uint32_t pll_lockdet;                    /* 0x0A4 */
	uint32_t pll_outdiv;                     /* 0x0A8 */
	uint32_t pll_fastlock_control;           /* 0x0AC */
	uint32_t pll_pass_out_override_one;      /* 0x0B0 */
	uint32_t pll_pass_out_override_two;      /* 0x0B4 */
	uint32_t pll_core_override;              /* 0x0B8 */
	uint32_t pll_core_input_override;        /* 0x0BC */
	uint32_t pll_rate_change;                /* 0x0C0 */
	uint32_t pll_digital_timers;             /* 0x0C4 */
	uint32_t pll_digital_timers_two;         /* 0x0C8 */
	uint32_t pll_decimal_div_start;          /* 0x0CC */
	uint32_t pll_frac_div_start_low;         /* 0x0D0 */
	uint32_t pll_frac_div_start_mid;         /* 0x0D4 */
	uint32_t pll_frac_div_start_high;        /* 0x0D8 */
	uint32_t pll_dec_frac_muxes;             /* 0x0DC */
	uint32_t pll_decimal_div_start_1;        /* 0x0E0 */
	uint32_t pll_frac_div_start_low_1;       /* 0x0E4 */
	uint32_t pll_frac_div_start_mid_1;       /* 0x0E8 */
	uint32_t pll_frac_div_start_high_1;      /* 0x0EC */
	uint32_t reserved1[12];                  /* 0x0F0..0x11C */
	uint32_t pll_ssc_stepsize_low_1;         /* 0x120 */
	uint32_t pll_ssc_stepsize_high_1;        /* 0x124 */
	uint32_t pll_ssc_div_per_low_1;          /* 0x128 */
	uint32_t pll_ssc_div_per_high_1;         /* 0x12C */
	uint32_t pll_ssc_adjper_low_1;           /* 0x130 */
	uint32_t pll_ssc_adjper_high_1;          /* 0x134 */
	uint32_t reserved2[6];                   /* 0x138..0x14C */
	uint32_t pll_ssc_control;               /* 0x150 */
	uint32_t pll_outdiv_rate;               /* 0x154 */
	uint32_t pll_lockdet_rate_1;             /* 0x158 */
	uint32_t reserved3[1];                   /* 0x15C */
	uint32_t pll_prop_gain_rate_1;           /* 0x160 */
	uint32_t reserved4[1];                   /* 0x164 */
	uint32_t pll_band_sel_rate_1;            /* 0x168 */
	uint32_t reserved5[1];                   /* 0x16C */
	uint32_t pll_int_gain_ifilt_band_1;      /* 0x170 */
	uint32_t reserved6[1];                   /* 0x174 */
	uint32_t pll_fl_int_gain_pfilt_band_1;   /* 0x178 */
	uint32_t reserved7[4];                   /* 0x17C..0x188 */
	uint32_t pll_fastlock_en_band;           /* 0x18C (unused, kept for layout) */
	uint32_t pll_lock_override;              /* 0x190 */
	uint32_t pll_lock_delay;                 /* 0x194 */
	uint32_t pll_lock_min_delay;             /* 0x198 */
	uint32_t reserved8[5];                   /* 0x19C..0x1AC */
	uint32_t pll_common_status_one;          /* 0x1B0 */
	uint32_t reserved9[1];                   /* 0x1B4 */
	uint32_t pll_band_sel_cal;               /* 0x1B8 */
	uint32_t pll_icode_accum_status_low;     /* 0x1BC */
	uint32_t pll_icode_accum_status_high;    /* 0x1C0 */
	uint32_t reserved10[27];                 /* 0x1C4..0x22C */
	uint32_t pll_rst_controls;              /* 0x230 */
	uint32_t reserved11[3];                  /* 0x234..0x23C */
	uint32_t pll_vco_config_1;              /* 0x240 */
	uint32_t reserved12[1];                  /* 0x244 */
	uint32_t pll_clock_inverters_1;          /* 0x248 */
	uint32_t reserved13[1];                  /* 0x24C */
	uint32_t pll_cmode_1;                    /* 0x250 */
	uint32_t reserved14[1];                  /* 0x254 */
	uint32_t pll_analog_controls_five_1;     /* 0x258 */
	uint32_t reserved15[1];                  /* 0x25C */
	uint32_t pll_perf_optimize;              /* 0x260 */
};

check_member(dsi_phy_pll_qlink_regs, pll_cntrl, 0x040);
check_member(dsi_phy_pll_qlink_regs, pll_calibration_settings, 0x044);
check_member(dsi_phy_pll_qlink_regs, pll_system_muxes, 0x028);
check_member(dsi_phy_pll_qlink_regs, pll_decimal_div_start_1, 0x0E0);
check_member(dsi_phy_pll_qlink_regs, pll_frac_div_start_low_1, 0x0E4);
check_member(dsi_phy_pll_qlink_regs, pll_frac_div_start_mid_1, 0x0E8);
check_member(dsi_phy_pll_qlink_regs, pll_frac_div_start_high_1, 0x0EC);
check_member(dsi_phy_pll_qlink_regs, pll_ssc_control, 0x150);
check_member(dsi_phy_pll_qlink_regs, pll_outdiv_rate, 0x154);
check_member(dsi_phy_pll_qlink_regs, pll_lockdet_rate_1, 0x158);
check_member(dsi_phy_pll_qlink_regs, pll_prop_gain_rate_1, 0x160);
check_member(dsi_phy_pll_qlink_regs, pll_band_sel_rate_1, 0x168);
check_member(dsi_phy_pll_qlink_regs, pll_int_gain_ifilt_band_1, 0x170);
check_member(dsi_phy_pll_qlink_regs, pll_fl_int_gain_pfilt_band_1, 0x178);
check_member(dsi_phy_pll_qlink_regs, pll_lock_override, 0x190);
check_member(dsi_phy_pll_qlink_regs, pll_lock_delay, 0x194);
check_member(dsi_phy_pll_qlink_regs, pll_lock_min_delay, 0x198);
check_member(dsi_phy_pll_qlink_regs, pll_common_status_one, 0x1B0);
check_member(dsi_phy_pll_qlink_regs, pll_rst_controls, 0x230);
check_member(dsi_phy_pll_qlink_regs, pll_vco_config_1, 0x240);
check_member(dsi_phy_pll_qlink_regs, pll_clock_inverters_1, 0x248);
check_member(dsi_phy_pll_qlink_regs, pll_cmode_1, 0x250);
check_member(dsi_phy_pll_qlink_regs, pll_analog_controls_five_1, 0x258);
check_member(dsi_phy_pll_qlink_regs, pll_perf_optimize, 0x260);

// MDSS_BASE + 0x3A000 (0xAE3A000)
struct mdp_intf_regs {
	uint32_t timing_eng_enable;          // 0x000
	uint32_t intf_config;                // 0x004
	uint32_t intf_hsync_ctl;             // 0x008
	uint32_t intf_vysnc_period_f0;       // 0x00C
	uint32_t intf_vysnc_period_f1;       // 0x010
	uint32_t intf_vysnc_pulse_width_f0;  // 0x014
	uint32_t intf_vysnc_pulse_width_f1;  // 0x018
	uint32_t intf_disp_v_start_f0;       // 0x01C
	uint32_t intf_disp_v_start_f1;       // 0x020
	uint32_t intf_disp_v_end_f0;         // 0x024
	uint32_t intf_disp_v_end_f1;         // 0x028
	uint32_t intf_active_v_start_f0;     // 0x02C
	uint32_t intf_active_v_start_f1;     // 0x030
	uint32_t intf_active_v_end_f0;       // 0x034
	uint32_t intf_active_v_end_f1;       // 0x038
	uint32_t intf_disp_hctl;             // 0x03C
	uint32_t intf_active_hctl;           // 0x040
	uint32_t intf_border_color;          // 0x044
	uint32_t intf_underflow_color;       // 0x048
	uint32_t hsync_skew;                 // 0x04C
	uint32_t polarity_ctl;               // 0x050
	uint32_t test_ctl;                   // 0x054
	uint32_t tp_color0;                  // 0x058
	uint32_t tp_color1;                  // 0x05C
	uint32_t intf_config2;               // 0x060
	uint32_t display_data_hctl;          // 0x064
	uint32_t reserved0[10];              // 0x068-0x08C
	uint32_t intf_panel_format;          // 0x090
	uint32_t reserved1[55];              // 0x094-0x16C
	uint32_t intf_prof_fetch_start;      // 0x170
	uint32_t reserved2a[20];             // 0x174-0x1C0
	uint32_t intf_intr_status;           // 0x1C4
	uint32_t intf_intr_clear;            // 0x1C8
	uint32_t reserved2b[9];              // 0x1CC-0x1EC
	uint32_t throttling_count;           // 0x1F0
	uint32_t throttling_count_msb;       // 0x1F4
	uint32_t reserved2c[2];              // 0x1F8-0x1FC
	uint32_t wd_timer_0_ltj_ctl;         // 0x200
	uint32_t wd_timer_0_ltj_ctl_1;       // 0x204
	uint32_t wd_timer_0_main_max_status; // 0x208
	uint32_t reserved2d[1];              // 0x20C
	uint32_t vsync_timestamp_ctrl;       // 0x210
	uint32_t vsync_timestamp0;           // 0x214
	uint32_t vsync_timestamp1;           // 0x218
	uint32_t mdp_vsync_timestamp0;       // 0x21C
	uint32_t mdp_vsync_timestamp1;       // 0x220
	uint32_t wd_timer_0_jitter_ctl;      // 0x224
	uint32_t wd_timer_0_ltj_slope;       // 0x228
	uint32_t wd_timer_0_ltj_max;         // 0x22C
	uint32_t wd_timer_0_ctl;             // 0x230
	uint32_t wd_timer_0_ctl2;            // 0x234
	uint32_t wd_timer_0_load_value;      // 0x238
	uint32_t wd_timer_0_status_value;    // 0x23C
	uint32_t wd_timer_0_ltj_int_status;  // 0x240
	uint32_t wd_timer_0_ltj_frac_status; // 0x244
	uint32_t wd_timer_0_lfsr_status;     // 0x248
	uint32_t edp_psr_override2;          // 0x24C
	uint32_t prog_line_intr_conf;        // 0x250
	uint32_t edp_psr_update_ctrl;        // 0x254
	uint32_t edp_psr_override;           // 0x258
	uint32_t intf_mux;                   // 0x25C
};

check_member(mdp_intf_regs, intf_panel_format, 0x90);
check_member(mdp_intf_regs, intf_prof_fetch_start, 0x170);
check_member(mdp_intf_regs, intf_mux, 0x25C);

// Base: MDP_0_CTL_BASE = 0xAE16000
struct mdp_ctl_regs {
	uint32_t ctl_layer0;                 // 0x000
	uint32_t ctl_layer1;                 // 0x004
	uint32_t layer_2;                    // 0x008
	uint32_t layer_3;                    // 0x00C
	uint32_t layer_4;                    // 0x010
	uint32_t ctl_top;                    // 0x014
	uint32_t ctl_flush;                  // 0x018
	uint32_t ctl_start;                  // 0x01C
	uint32_t pad0[1];                    // 0x020
	uint32_t layer_5;                    // 0x024
	uint32_t pad1[2];                    // 0x028-0x02C
	uint32_t sw_reset;                   // 0x030
	uint32_t pad2[2];                    // 0x034-0x038
	uint32_t ltm_start_dep;              // 0x03C
	uint32_t ctl_layer_0_ext;            // 0x040
	uint32_t ctl_layer_1_ext;            // 0x044
	uint32_t layer_2_ext;                // 0x048
	uint32_t layer_3_ext;                // 0x04C
	uint32_t layer_4_ext;                // 0x050
	uint32_t layer_5_ext;                // 0x054
	uint32_t pad3[2];                    // 0x058-0x05C
	uint32_t sw_reset_override;          // 0x060
	uint32_t ctl_status;                 // 0x064
	uint32_t sw_reset_ctrl;              // 0x068
	uint32_t pad4[1];                    // 0x06C
	uint32_t ctl_layer_0_ext2;           // 0x070
	uint32_t ctl_layer_1_ext2;           // 0x074
	uint32_t layer_2_ext2;               // 0x078
	uint32_t layer_3_ext2;               // 0x07C
	uint32_t layer_4_ext2;               // 0x080
	uint32_t layer_5_ext2;               // 0x084
	uint32_t pad5[2];                    // 0x088-0x08C
	uint32_t flush_mask;                 // 0x090
	uint32_t null_flush_status;          // 0x094
	uint32_t null_flush_clear;           // 0x098
	uint32_t flush_complete;             // 0x09C
	uint32_t layer_0_ext3;               // 0x0A0
	uint32_t layer_1_ext3;               // 0x0A4
	uint32_t layer_2_ext3;               // 0x0A8
	uint32_t layer_3_ext3;               // 0x0AC
	uint32_t layer_4_ext3;               // 0x0B0
	uint32_t layer_5_ext3;               // 0x0B4
	uint32_t ctl_layer_0_ext4;           // 0x0B8
	uint32_t ctl_layer_1_ext4;           // 0x0BC
	uint32_t layer_2_ext4;               // 0x0C0
	uint32_t layer_3_ext4;               // 0x0C4
	uint32_t layer_4_ext4;               // 0x0C8
	uint32_t layer_5_ext4;               // 0x0CC
	uint32_t ctl_prepare;                // 0x0D0
	uint32_t lut_dma_trigger;            // 0x0D4
	uint32_t lut_dma_queue_0_flush_ctrl; // 0x0D8
	uint32_t lut_dma_queue_1_flush_ctrl; // 0x0DC
	uint32_t lut_dma_start_dep;          // 0x0E0
	uint32_t ctl_merge_3d_active;        // 0x0E4
	uint32_t dsc_active;                 // 0x0E8
	uint32_t wb_active;                  // 0x0EC
	uint32_t cwb_active;                 // 0x0F0
	uint32_t ctl_intf_active;            // 0x0F4
	uint32_t ctl_cdm_active;             // 0x0F8
	uint32_t ctl_fetch_pipe_active;      // 0x0FC
	uint32_t merge_3d_flush;             // 0x100
	uint32_t dsc_flush;                  // 0x104
	uint32_t wb_flush;                   // 0x108
	uint32_t cwb_flush;                  // 0x10C
	uint32_t ctl_intf_flush;             // 0x110
	uint32_t cdm_flush;                  // 0x114
	uint32_t lut_dma_sb_flush_ctrl;      // 0x118
	uint32_t pad6[3];                    // 0x11C-0x124
	uint32_t periph_flush;               // 0x128
	uint32_t pad7[2];                    // 0x12C-0x130
	uint32_t ctl_intf_master;            // 0x134
	uint32_t uidle_active;               // 0x138
	uint32_t dspp_0_flush;               // 0x13C
	uint32_t dspp_1_flush;               // 0x140
	uint32_t dspp_2_flush;               // 0x144
	uint32_t dspp_3_flush;               // 0x148
	uint32_t dspp_4_flush;               // 0x14C
	uint32_t merge_3d_flush_complete;    // 0x150
	uint32_t dsc_flush_complete;         // 0x154
	uint32_t cwb_flush_complete;         // 0x158
	uint32_t intf_flush_complete;        // 0x15C
	uint32_t periph_flush_complete;      // 0x160
	uint32_t wb_flush_complete;          // 0x164
};

check_member(mdp_ctl_regs, ctl_top, 0x14);
check_member(mdp_ctl_regs, ctl_intf_active, 0xF4);
check_member(mdp_ctl_regs, ctl_intf_flush, 0x110);
check_member(mdp_ctl_regs, ctl_intf_master, 0x134);

// 0xAE4F000
struct mdp_merge_3d_regs {
	uint32_t reserved0;
	uint32_t mode; // 0x004
};

struct mdp_layer_mixer_regs {
	uint32_t layer_op_mode;        // 0x000
	uint32_t layer_out_size;       // 0x004
	uint32_t layer_border_color_0; // 0x008
	uint32_t layer_border_color_1; // 0x00C
	uint32_t layer_config_0x10;    // 0x010
	uint32_t reserved0[3];         // 0x014, 0x018, 0x01C
	struct {
		uint32_t layer_blend_op;                   // 0x00
		uint32_t layer_blend_const_alpha;          // 0x04
		uint32_t layer_blend_fg_color_fill_color0; // 0x08
		uint32_t layer_blend_fg_color_fill_color1; // 0x0C
		uint32_t layer_blend_fg_fill_size;         // 0x10
		uint32_t layer_blend_fg_fill_xy;           // 0x14
	} layer_blend[6]; // 0x020 start, array of 6 stages (0..5)
};

struct mdp_sspp_regs {                            // MDP_VP_0_SSPP_BASE = MDSS_BASE + 0x5000
	uint32_t sspp_src_size;                   // 0x0000
	uint32_t sspp_src_img_size;               // 0x0004
	uint32_t sspp_src_xy;                     // 0x0008
	uint32_t sspp_out_size;                   // 0x000C
	uint32_t sspp_out_xy;                     // 0x0010
	uint32_t sspp_src0;                       // 0x0014
	uint32_t sspp_src1;                       // 0x0018
	uint32_t sspp_src2;                       // 0x001C
	uint32_t sspp_src3;                       // 0x0020
	uint32_t sspp_src_ystride0;               // 0x0024
	uint32_t sspp_src_ystride1;               // 0x0028
	uint32_t sspp_tile_frame_size;            // 0x002C
	uint32_t sspp_src_format;                 // 0x0030
	uint32_t sspp_src_unpack_pattern;         // 0x0034
	uint32_t sspp_src_op_mode;                // 0x0038
	uint32_t reserved0_a[9];                  // 0x003C-0x05F
	uint32_t sspp_danger_lut;                 // 0x0060
	uint32_t sspp_safe_lut;                   // 0x0064
	uint32_t reserved0_b[1];                  // 0x0068
	uint32_t sspp_qos_ctrl;                   // 0x006C
	uint32_t reserved0_c[1];                  // 0x0070
	uint32_t sspp_creq_lut_0;                 // 0x0074
	uint32_t sspp_creq_lut_1;                 // 0x0078
	uint32_t reserved0_d[33];                 // 0x007C-0x0FF
	uint32_t sspp_sw_pix_ext_c0_lr;           // 0x0100
	uint32_t sspp_sw_pix_ext_c0_tb;           // 0x0104
	uint32_t sspp_sw_pic_ext_c0_req_pixels;   // 0x0108
	uint32_t reserved1[1];                    // 0x010C
	uint32_t sspp_sw_pix_ext_c1c2_lr;         // 0x0110
	uint32_t sspp_sw_pix_ext_c1c2_tb;         // 0x0114
	uint32_t sspp_sw_pic_ext_c1c2_req_pixels; // 0x0118
	uint32_t reserved2[1];                    // 0x011C
	uint32_t sspp_sw_pix_ext_c3_lr;           // 0x0120
	uint32_t sspp_sw_pix_ext_c3_tb;           // 0x0124
	uint32_t sspp_sw_pic_ext_c3_req_pixels;   // 0x0128
};

check_member(mdp_sspp_regs, sspp_sw_pic_ext_c0_req_pixels, 0x108);
check_member(mdp_sspp_regs, sspp_sw_pic_ext_c1c2_req_pixels, 0x118);
check_member(mdp_sspp_regs, sspp_sw_pic_ext_c3_req_pixels, 0x128);

// Defined relative to Pipe Base + 0xA00
struct mdp_qseed3_regs {
	uint32_t reserved_0;        // 0xA00
	uint32_t op_mode;           // 0xA04
	uint32_t reserved_1[2];     // 0xA08-0xA0C
	uint32_t phase_step_y_h;    // 0xA10
	uint32_t phase_step_y_v;    // 0xA14
	uint32_t phase_step_uv_h;   // 0xA18
	uint32_t phase_step_uv_v;   // 0xA1C
	uint32_t preload;           // 0xA20
	uint32_t reserved_2[7];     // 0xA24-0xA3F
	uint32_t src_size_y_rgb_a;  // 0xA40
	uint32_t src_size_uv;       // 0xA44
	uint32_t dst_size;          // 0xA48
	uint32_t coef_lut_ctrl;     // 0xA4C
	uint32_t reserved_3[16];    // 0xA50-0xA8F
	uint32_t phase_init_y_h;    // 0xA90
	uint32_t phase_init_y_v;    // 0xA94
	uint32_t phase_init_uv_h;   // 0xA98
	uint32_t phase_init_uv_v;   // 0xA9C
	uint32_t reserved_4[24];    // 0xAA0-0xAFF
	uint32_t coef_lut_data[33]; // 0xB00-0xB80
};

/*
 * x1p42100 SDE 10.0.0 UBWC configuration constants
 * Source: UEFI hal_sde_10_0_0.c HAL_MDP_10_0_0_UBWC_DEFAULT_* defines
 * UBWC_STATIC_CTRL field positions from UEFI mdssreg_920.h
 */
#define MDSS_UBWC_MAL_SIZE          2   /* MAL=32 bytes, bits[1:0] - confirmed from golden dump */
#define MDSS_UBWC_LVL1_SWIZ         1   /* enable,  bit[2] - confirmed from golden dump */
#define MDSS_UBWC_LVL2_SWIZ         1   /* enable,  bit[3] */
#define MDSS_UBWC_LVL3_SWIZ         1   /* enable,  bit[4] */
#define MDSS_UBWC_MAX_BANK_BIT      1   /* bank15,  bits[7:5] */
#define MDSS_UBWC_BANK_SPREAD       0   /* disable, bit[8] */
#define MDSS_UBWC_DDR_CHANNELS      1   /* 8ch,     bit[12] */
#define MDSS_UBWC_VERSION           2   /* UBWC 4.x, bits[1:0] of CTRL_2 */
#define MDSS_UBWC_PREDICTION_MODE   1   /* X=C mode */

/* Computed register values */
#define MDSS_UBWC_STATIC_CTRL_VAL \
	((MDSS_UBWC_MAL_SIZE       << 0)  | \
	 (MDSS_UBWC_LVL1_SWIZ      << 2)  | \
	 (MDSS_UBWC_LVL2_SWIZ      << 3)  | \
	 (MDSS_UBWC_LVL3_SWIZ      << 4)  | \
	 (MDSS_UBWC_MAX_BANK_BIT   << 5)  | \
	 (MDSS_UBWC_BANK_SPREAD    << 8)  | \
	 (MDSS_UBWC_DDR_CHANNELS   << 12))

struct mdss_hw_regs {
	uint32_t hw_version;          /* 0x000 */
	uint32_t _rsvd0[0x50];        /* 0x004..0x140 */
	uint32_t ubwc_static_ctrl;    /* 0x144 */
	uint32_t _rsvd1[2];           /* 0x148..0x14C */
	uint32_t ubwc_ctrl_2;         /* 0x150 */
	uint32_t ubwc_prediction_mode;/* 0x154 */
};

check_member(mdss_hw_regs, hw_version, 0x0);
check_member(mdss_hw_regs, ubwc_static_ctrl, 0x144);
check_member(mdss_hw_regs, ubwc_ctrl_2, 0x150);
check_member(mdss_hw_regs, ubwc_prediction_mode, 0x154);
static struct mdss_hw_regs *const mdss_hw = (void *)MDSS_BASE;

struct vbif_rt_regs {
	uint32_t _rsvd_0000;                             // 0x0000
	uint32_t vbif_clkon;                             // 0x0004
	uint32_t _rsvd_0008_00D4[(0x00D8 - 0x0008) / 4]; // 0x0008-0x00D4
	uint32_t vbif_ddr_out_max_burst;                 // 0x00D8
	uint32_t _rsvd_00DC_0120[(0x0124 - 0x00DC) / 4]; // 0x00DC-0x0120
	uint32_t vbif_round_robin_qos_arb;               // 0x0124
	uint32_t _rsvd_0128_015C[(0x0160 - 0x0128) / 4]; // 0x0128-0x015C
	uint32_t vbif_out_axi_amemtype_conf0;            // 0x0160
	uint32_t vbif_out_axi_amemtype_conf1;            // 0x0164
	uint32_t _rsvd_0168_016C[(0x0170 - 0x0168) / 4]; // 0x0168-0x016C
	uint32_t vbif_out_axi_ashared;                   // 0x0170
	uint32_t vbif_out_axi_innershared;               // 0x0174
	uint32_t vbif_out_axi_aooo_en;                   // 0x0178
	uint32_t vbif_out_axi_aooo;                      // 0x017C
	uint32_t _rsvd_0180_054C[(0x0550 - 0x0180) / 4]; // 0x0180-0x054C
	struct {
		uint32_t vbif_xinl_qos_rp_remap; // 0x0550 + idx*0x8
		uint32_t vbif_xinh_qos_rp_remap; // 0x0554 + idx*0x8
	} qos_rp_remap[8];
	struct {
		uint32_t vbif_xinl_qos_lvl_remap; // 0x0590 + idx*0x8
		uint32_t vbif_xinh_qos_lvl_remap; // 0x0594 + idx*0x8
	} qos_lvl_remap[8];
};

struct vbif_nrt_regs {
	uint32_t _rsvd_0000;                             // 0x0000
	uint32_t vbif_clkon;                             // 0x0004
	uint32_t vbif_clk_force_ctrl0;                   // 0x0008
	uint32_t vbif_clk_force_ctrl1;                   // 0x000C
	uint32_t vbif_qos_override_en;                   // 0x0010
	uint32_t vbif_prilvl_act_prop_en;                // 0x0014
	uint32_t vbif_qos_override_reqpri0;              // 0x0018
	uint32_t _rsvd_001C;                             // 0x001C
	uint32_t vbif_qos_override_prilvl0;              // 0x0020
	uint32_t _rsvd_0024_00A8[(0x00AC - 0x0024) / 4]; // 0x0024-0x00A8
	uint32_t vbif_write_gather_en;                   // 0x00AC
	uint32_t vbif_in_rd_lim_conf0;                   // 0x00B0
	uint32_t _rsvd_00B4_00BC[(0x00C0 - 0x00B4) / 4]; // 0x00B4-0x00BC
	uint32_t vbif_in_wr_lim_conf0;                   // 0x00C0
	uint32_t _rsvd_00C4_00CC[(0x00D0 - 0x00C4) / 4]; // 0x00C4-0x00CC
	uint32_t vbif_out_rd_lim_conf0;                  // 0x00D0
	uint32_t vbif_out_wr_lim_conf0;                  // 0x00D4
	uint32_t vbif_ddr_out_max_burst;                 // 0x00D8
	uint32_t _rsvd_00DC_00EC[(0x00F0 - 0x00DC) / 4]; // 0x00DC-0x00EC
	uint32_t vbif_arb_ctl;                           // 0x00F0
	uint32_t vbif_ddr_arb_conf0;                     // 0x00F4
	uint32_t _rsvd_00F8_00FC[(0x0100 - 0x00F8) / 4]; // 0x00F8-0x00FC
	uint32_t vbif_fixed_arb_conf0;                   // 0x0100
	uint32_t _rsvd_0104;                             // 0x0104
	uint32_t vbif_arb_lcb;                           // 0x0108
	uint32_t _rsvd_010C_0120[(0x0124 - 0x010C) / 4]; // 0x010C-0x0120
	uint32_t vbif_round_robin_qos_arb;               // 0x0124
	uint32_t _rsvd_0128_015C[(0x0160 - 0x0128) / 4]; // 0x0128-0x015C
	uint32_t vbif_out_axi_amemtype_conf0;            // 0x0160
	uint32_t vbif_out_axi_amemtype_conf1;            // 0x0164
	uint32_t vbif_out_axi_ainst_override_en;         // 0x0168
	uint32_t vbif_out_axi_ainst;                     // 0x016C
	uint32_t vbif_out_axi_ashared;                   // 0x0170
	uint32_t vbif_out_axi_innershared;               // 0x0174
	uint32_t vbif_out_axi_aooo_en;                   // 0x0178
	uint32_t vbif_out_axi_aooo;                      // 0x017C
};

check_member(vbif_rt_regs, vbif_out_axi_amemtype_conf0, 0x160);
check_member(vbif_rt_regs, qos_rp_remap[0], 0x550);

struct mdp_periph_top1_regs {
	/* 0x000-0x014: reserved area up to first known register */
	uint32_t reserved_000[0x018u / 4]; // 0x000-0x014

	/* 0x018: HDMI/DP core select */
	uint32_t hdmi_dp_core_select; // 0x018

	/* 0x01C-0x090: reserved area until HPD select */
	uint32_t reserved_01C[(0x094u - (0x018u + 4)) / 4];

	/* 0x094: DP HPD select */
	uint32_t dp_hpd_select; // 0x094
};

enum {
	INTF = BIT(31),
	PERIPH = BIT(30),
	CWB = BIT(28),
	ROT = BIT(27),
	CDM_0 = BIT(26),
	DMA_3 = BIT(25),
	DMA_2 = BIT(24),
	MERGE_3D = BIT(23),
	DSC = BIT(22),
	DSPP_3 = BIT(21),
	LAYER_MIXER_5 = BIT(20),
	DSPP_PA_LUTV_3 = BIT(19),
	VIG_3 = BIT(18),
	CTL = BIT(17),
	WB = BIT(16),
	DSPP_2 = BIT(15),
	DSPP_1 = BIT(14),
	DSPP_0 = BIT(13),
	DMA_1 = BIT(12),
	DMA_0 = BIT(11),
	LAYER_MIXER_4 = BIT(10),
	LAYER_MIXER_3 = BIT(9),
	LAYER_MIXER_2 = BIT(8),
	LAYER_MIXER_1 = BIT(7),
	LAYER_MIXER_0 = BIT(6),
	DSPP_PA_LUTV_2 = BIT(5),
	DSPP_PA_LUTV_1 = BIT(4),
	DSPP_PA_LUTV_0 = BIT(3),
	VIG_2 = BIT(2),
	VIG_1 = BIT(1),
	VIG_0 = BIT(0),
};

enum {
	DSI_AHBS_HCLK_ON = BIT(0),
	DSI_AHBM_SCLK_ON = BIT(1),
	DSI_PCLK_ON = BIT(2),
	DSI_DSICLK_ON = BIT(3),
	DSI_BYTECLK_ON = BIT(4),
	DSI_ESCCLK_ON = BIT(5),
	DSI_FORCE_ON_DYN_AHBS_HCLK = BIT(8),
	DSI_FORCE_ON_DYN_AHBM_HCLK = BIT(9),
	DSI_FORCE_ON_DYN_DSICLK = BIT(10),
	DSI_FORCE_ON_DYN_BYTECLK = BIT(11),
	DSI_AHBS_HCLK_HYSTERISIS1_CTRL = (3 << 11),
	DSI_AHBM_HCLK_HYSTERISIS1_CTRL = (3 << 13),
	DSI_DSICLK_HYSTERISIS1_CTRL = (3 << 15),
	DSI_FORCE_ON_DYN_PCLK = BIT(20),
	DSI_FORCE_ON_LANE_LAYER_TG_BYTECLK = BIT(21),
	DSI_DMA_CLK_STOP = BIT(22),
};

enum {
	DSI_CMD_MODE_DMA_DONE_AK = BIT(0),
	DSI_CMD_MODE_DMA_DONE_STAT = BIT(0),
	DSI_CMD_MODE_DMA_DONE_MASK = BIT(1),
	DSI_CMD_MODE_MDP_DONE_AK = BIT(8),
	DSI_CMD_MODE_MDP_DONE_STAT = BIT(8),
	DSI_CMD_MODE_MDP_DONE_MASK = BIT(9),
	DSI_CMD_MDP_STREAM0_DONE_AK = BIT(10),
	DSI_CMD_MDP_STREAM0_DONE_STAT = BIT(10),
	DSI_CMD_MDP_STREAM0_DONE_MASK = BIT(11),
	DSI_VIDEO_MODE_DONE_AK = BIT(16),
	DSI_VIDEO_MODE_DONE_STAT = BIT(16),
	DSI_VIDEO_MODE_DONE_MASK = BIT(17),
	DSI_BTA_DONE_AK = BIT(20),
	DSI_BTA_DONE_STAT = BIT(20),
	DSI_BTA_DONE_MASK = BIT(21),
	DSI_ERROR_AK = BIT(24),
	DSI_ERROR_STAT = BIT(24),
	DSI_ERROR_MASK = BIT(25),
	DSI_DYNAMIC_BLANKING_DMA_DONE_AK = BIT(26),
	DSI_DYNAMIC_BLANKING_DMA_DONE_STAT = BIT(26),
	DSI_DYNAMIC_BLANKING_DMA_DONE_MASK = BIT(27),
	DSI_DYNAMIC_REFRESH_DONE_AK = BIT(28),
	DSI_DYNAMIC_REFRESH_DONE_STAT = BIT(28),
	DSI_DYNAMIC_REFRESH_DONE_MASK = BIT(29),
	DSI_DESKEW_DONE_AK = BIT(30),
	DSI_DESKEW_DONE_STAT = BIT(30),
	DSI_DESKEW_DONE_MASK = BIT(31),
};

enum {
	WR_MEM_START = 255,
	WR_MEM_CONTINUE = 255 << 8,
	INSERT_DCS_COMMAND = BIT(16),
};

enum {
	PACKET_TYPE = BIT(24),
	POWER_MODE = BIT(26),
	EMBEDDED_MODE = BIT(28),
	WC_SEL = BIT(29),
	BROADCAST_MASTER = BIT(30),
	BROADCAST_EN = BIT(31),
};

enum {
	BWC_DEC_EN = BIT(0),
	SW_PIX_EXT_OVERRIDE = BIT(31),
};

enum {
	INTERLACE_MODE = BIT(0),
	REPEAT_PIXEL = BIT(1),
	INTERLACE_INIT_SEL = BIT(2),
	BORDER_ENABLE = BIT(3),
	EDP_PSR_OVERRIDE_EN = BIT(7),
	PACK_ALIGN = BIT(10),
	DSI_VIDEO_STOP_MODE = BIT(23),
	ACTIVE_H_EN = BIT(29),
	ACTIVE_V_EN = BIT(30),
	PROG_FETCH_START_EN = BIT(31),
};

/* INTF_CONFIG2 field bits (from UEFI mdssreg_1000.h INTF_0_INTF_CONFIG2) */
#define INTF_CONFIG2_DCE_DATA_COMPRESS   BIT(12) /* DSC compression enable */
#define INTF_CONFIG2_DATA_HCTL_EN        BIT(4)  /* enable DISPLAY_DATA_HCTL register */
#define INTF_CONFIG2_DATABUS_WIDEN       BIT(0)  /* 2 pixels/clk - required for DSC */

enum {
	VIG_0_OUT = BIT(0),
	VIG_1_OUT = BIT(0),
	BORDER_OUT = BIT(24),
};

enum {
	FETCH_PIPE_VIG0_ACTIVE = BIT(16),
	FETCH_PIPE_VIG1_ACTIVE = BIT(17),
};

enum {
	INTF_ACTIVE_0 = BIT(0),
	INTF_ACTIVE_1 = BIT(1),
	INTF_ACTIVE_2 = BIT(2),
	INTF_ACTIVE_5 = BIT(5),
};

enum {
	INTF_FLUSH_0 = BIT(0),
	INTF_FLUSH_1 = BIT(1),
	INTF_FLUSH_2 = BIT(2),
	INTF_FLUSH_5 = BIT(5),
};

/*
 * DSC encoder register blocks (PHY 2.7.0 / SDE 9.2.x, purwa x1p42100).
 * Confirmed from UEFI hwio.cmm:
 *   DSC0_CMN  = MDSS_BASE + 0x81000  (ENC_MAIN_CONF)
 *   DSC0_ENC0 = MDSS_BASE + 0x81100  (ENC_DF_CTRL, DSC_MAIN_CONF, ...)
 *   DSC0_ENC1 = MDSS_BASE + 0x81200  (same layout, stride 0x100)
 *   DSC1_CMN  = MDSS_BASE + 0x82000
 *   DSC1_ENC0 = MDSS_BASE + 0x82100
 *   DSC1_ENC1 = MDSS_BASE + 0x82200
 * For 2-2-1 topology: use DSC0 (CMN + ENC0 + ENC1).
 */

/* DSC common block - one per DSC core */
struct dsc_cmn_regs {
	uint32_t enc_main_conf;          /* 0x00: SPLIT_PANEL[0] MULTIPLEX_MODE[1] NUM_OF_SS[8:7] */
};

/* DSC wrapper registers - at offset 0xF00/0xF80 from DSC CMN base */
struct dsc_ctl_regs {
	uint32_t dsc_ctl;          /* 0x00: DSC enable/disable */
	uint32_t dsc_cfg;          /* 0x04: DSC config (0x3801 = enable) */
	uint32_t dsc_data_in_swap; /* 0x08: HLOS golden=0x2C688 */
	uint32_t dsc_clk_ctrl;     /* 0x0C: DSC clock control (0xC0000000) */
};

/* DSC encoder block - two per DSC core (ENC0 at +0x100, ENC1 at +0x200 from CMN) */
struct dsc_enc_regs {
	uint32_t enc_df_ctrl;            /* 0x00: INITIAL_LINES[7:0] VIDEO_MODE[9] OB_MAX_ADDR[31:18] */
	uint32_t enc_general_status;     /* 0x04 */
	uint32_t enc_hslice_status;      /* 0x08 */
	uint32_t enc_out_status;         /* 0x0C */
	uint32_t enc_int_stat;           /* 0x10 */
	uint32_t enc_int_clr;            /* 0x14 */
	uint32_t enc_int_mask;           /* 0x18 */
	uint32_t reserved0[5];           /* 0x1C..0x2C */
	uint32_t dsc_main_conf;          /* 0x30: BPC[3:0] CONVERT_RGB[4] LINEBUF_DEPTH[9:6] BPP[19:10] BLOCK_PRED[20] DSC_MINOR[31:28] */
	uint32_t dsc_picture_size;       /* 0x34: WIDTH[15:0] HEIGHT[31:16] */
	uint32_t dsc_slice_size;         /* 0x38: SLICE_WIDTH[15:0] SLICE_HEIGHT[31:16] */
	uint32_t dsc_misc_size;          /* 0x3C: CHUNK_SIZE[15:0] */
	uint32_t dsc_hrd_delays;         /* 0x40: INITIAL_XMIT_DELAY[9:0] INITIAL_DEC_DELAY[31:16] */
	uint32_t dsc_rc_scale;           /* 0x44: INITIAL_SCALE_VALUE[5:0] */
	uint32_t dsc_rc_scale_inc_dec;   /* 0x48: SCALE_INCREMENT_INTERVAL[15:0] SCALE_DECREMENT_INTERVAL[27:16] */
	uint32_t dsc_rc_offsets_1;       /* 0x4C: FIRST_LINE_BPG_OFFSET[4:0] SECOND_LINE_BPG_OFFSET[9:5] */
	uint32_t dsc_rc_offsets_2;       /* 0x50: NFL_BPG_OFFSET[15:0] SLICE_BPG_OFFSET[31:16] */
	uint32_t dsc_rc_offsets_3;       /* 0x54: INITIAL_OFFSET[15:0] FINAL_OFFSET[31:16] */
	uint32_t dsc_rc_offsets_4;       /* 0x58 */
	uint32_t dsc_flatness_qp;        /* 0x5C: FLATNESS_MIN_QP[4:0] FLATNESS_MAX_QP[9:5] FLATNESS_DET_THRESH[17:10] */
	uint32_t dsc_rc_model_size;      /* 0x60: RC_MODEL_SIZE[15:0] */
	uint32_t dsc_rc_config;          /* 0x64: RC_TGT_OFFSET_LOW[3:0] RC_TGT_OFFSET_HI[7:4] RC_EDGE_FACTOR[11:8] RC_QUANT_INCR_LIMIT1[20:16] RC_QUANT_INCR_LIMIT0[28:24] */
	uint32_t dsc_rc_buf_thresh_0;    /* 0x68: THRESH[0..3] each >>6, packed [7:0][15:8][23:16][31:24] */
	uint32_t dsc_rc_buf_thresh_1;    /* 0x6C: THRESH[4..7] */
	uint32_t dsc_rc_buf_thresh_2;    /* 0x70: THRESH[8..11] */
	uint32_t dsc_rc_buf_thresh_3;    /* 0x74: THRESH[12..13] */
	uint32_t dsc_rc_min_qp_0;        /* 0x78: ranges 0..4, each 5 bits */
	uint32_t dsc_rc_min_qp_1;        /* 0x7C: ranges 5..9 */
	uint32_t dsc_rc_min_qp_2;        /* 0x80: ranges 10..14 */
	uint32_t dsc_rc_max_qp_0;        /* 0x84: ranges 0..4 */
	uint32_t dsc_rc_max_qp_1;        /* 0x88: ranges 5..9 */
	uint32_t dsc_rc_max_qp_2;        /* 0x8C: ranges 10..14 */
	uint32_t dsc_rc_range_bpg_offsets_0; /* 0x90: ranges 0..4, each 6 bits */
	uint32_t dsc_rc_range_bpg_offsets_1; /* 0x94: ranges 5..9 */
	uint32_t dsc_rc_range_bpg_offsets_2; /* 0x98: ranges 10..14 */
};

check_member(dsc_enc_regs, dsc_main_conf,   0x30);
check_member(dsc_enc_regs, dsc_picture_size, 0x34);
check_member(dsc_enc_regs, dsc_hrd_delays,  0x40);
check_member(dsc_enc_regs, dsc_rc_model_size, 0x60);
check_member(dsc_enc_regs, dsc_rc_buf_thresh_0, 0x68);
check_member(dsc_enc_regs, dsc_rc_min_qp_0, 0x78);
check_member(dsc_enc_regs, dsc_rc_max_qp_0, 0x84);
check_member(dsc_enc_regs, dsc_rc_range_bpg_offsets_0, 0x90);

enum {
	MDP_DSC0_CMN_BASE  = MDSS_BASE + 0x81000,
	MDP_DSC0_ENC0_BASE = MDSS_BASE + 0x81100,
	MDP_DSC0_ENC1_BASE = MDSS_BASE + 0x81200,
	MDP_DSC1_CMN_BASE  = MDSS_BASE + 0x82000,
	MDP_DSC1_ENC0_BASE = MDSS_BASE + 0x82100,
	MDP_DSC1_ENC1_BASE = MDSS_BASE + 0x82200,
};

static struct dsc_cmn_regs *const dsc0_cmn  = (void *)MDP_DSC0_CMN_BASE;
static struct dsc_enc_regs *const dsc0_enc0 = (void *)MDP_DSC0_ENC0_BASE;
static struct dsc_enc_regs *const dsc0_enc1 = (void *)MDP_DSC0_ENC1_BASE;
/* DSC wrapper CTL/CFG/CLK_CTRL at CMN_BASE+0xF00 and CMN_BASE+0xF80 */
static struct dsc_ctl_regs *const dsc0_ctl0 = (void *)(MDP_DSC0_CMN_BASE + 0xF00);
static struct dsc_ctl_regs *const dsc0_ctl1 = (void *)(MDP_DSC0_CMN_BASE + 0xF80);
static struct dsc_cmn_regs *const dsc1_cmn  = (void *)MDP_DSC1_CMN_BASE;
static struct dsc_enc_regs *const dsc1_enc0 = (void *)MDP_DSC1_ENC0_BASE;
static struct dsc_enc_regs *const dsc1_enc1 = (void *)MDP_DSC1_ENC1_BASE;
static struct dsc_ctl_regs *const dsc1_ctl0 = (void *)(MDP_DSC1_CMN_BASE + 0xF00);
static struct dsc_ctl_regs *const dsc1_ctl1 = (void *)(MDP_DSC1_CMN_BASE + 0xF80);

enum mdss_flush_bits {
	FLUSH_INTF        = BIT(31),
	FLUSH_PERIPH      = BIT(30),
	FLUSH_MERGE_3D    = BIT(23),
	FLUSH_DSC         = BIT(22), /* DSC flush enable in CTL_FLUSH */
	FLUSH_CTL         = BIT(17),
	FLUSH_LM1         = BIT(7),
	FLUSH_LM0         = BIT(6),
	FLUSH_VIG1        = BIT(1),
	FLUSH_VIG0        = BIT(0),
};

#define MDP_CTL_LAYER_VIG0_OUT_SHIFT   0
#define MDP_CTL_LAYER_VIG1_OUT_SHIFT   3
#define MDP_CTL_LAYER_BORDER_OUT      BIT(24)
#define MDP_LM_STAGE3_FG              0x5

#define MDP_CTL_LAYER0_VIG0_STAGE3_CFG \
	((MDP_LM_STAGE3_FG << MDP_CTL_LAYER_VIG0_OUT_SHIFT) | \
	MDP_CTL_LAYER_BORDER_OUT)

#define MDP_CTL_LAYER1_VIG1_STAGE3_CFG \
	((MDP_LM_STAGE3_FG << MDP_CTL_LAYER_VIG1_OUT_SHIFT) | \
	MDP_CTL_LAYER_BORDER_OUT)

#define LM_LAYER_OP_MODE_SPLIT_RIGHT	BIT(31)

enum {
	MDSS_HW_REG_BASE = MDSS_BASE + 0x00000000,
	MDSS_HW_REG_BASE_SIZE = 0x1000,
	MDSS_HW_REG_BASE_USED = 0x154,
	MDSS_HW_REG_BASE_OFFS = 0x00000000,
};
enum edp_init_values {
	EDP_HPD_INT_ACK_CLEAR_ALL = 0x0000000F,    // Clear any pending HPD interrupts
	EDP_AUX_CTRL_ENABLE_BASIC = 0x00000011,    // AUX_CTRL = 0x11
	EDP_AUX_CTRL_ENABLE_RST = 0x00000013,      // AUX_CTRL = 0x13 (AUX reset toggle)
	EDP_HPD_REFTIMER_ENABLE_0013 = 0x00010013, // REFTIMER_ENABLE + interval
	EDP_HPD_CTRL_ENABLE = 0x00000001,          // Enable HPD logic with default polarity
	MDP_DP_HPD_SELECT_HPDDPT3 = 0x00000322,    // Select HPDDPT3
	MDP_HDMI_DP_CORE_SELECT_DP = 0x00000001,   // Select DP core
	EDP_AUX_TIMEOUT_COUNT_FFFF = 0x0000FFFF,   // Timeout count
	EDP_AUX_LIMITS_FFFF = 0x0000FFFF,          // Retry, NACK/DEFER, read-zero limits
	READ_WAIT2_WR_WAIT1 = 0x00000012,
	AHB2EDPPHY_AHB2PHY_AHB2PHY_TOP_CFG = 0xAEC6010,
};

#define MDP_SSPP_TOP0_REG_BASE                 (MDSS_BASE + 0x00001000)
#define HWIO_MDP_INTR_EN_ADDR                  (MDP_SSPP_TOP0_REG_BASE + 0x10) // 0xAE01010
#define HWIO_MDP_INTR_STATUS_ADDR              (MDP_SSPP_TOP0_REG_BASE + 0x14) // 0xAE01014
#define HWIO_MDP_INTR_CLEAR_ADDR               (MDP_SSPP_TOP0_REG_BASE + 0x18) // 0xAE01018
#define HWIO_MDP_INTR_EN_INTF_5_VSYNC_BMSK     0x800000
#define HWIO_MDP_INTR_EN_PING_PONG_0_DONE_BMSK 0x100

#define GPIO_PANEL_POWER_ON GPIO(70)  // INTERNAL_EDP_REG_EN_GPIO
#define GPIO_PANEL_HPD      GPIO(119) // INTERNAL_EDP_HPD_GPIO

static struct dsi_regs *const dsi0 = (void *)DSI0_CTL_BASE;
static struct dsi_phy_regs *const dsi0_phy = (void *)DSI0_PHY_BASE;
static struct dsi_phy_pll_qlink_regs *const phy_pll_qlink = (void *)DSI0_PHY_PLL_QLINK_COM;

/* DSI1 register pointers - used for dual DSI (2-2-2 topology) */
static struct dsi_regs *const dsi1 = (void *)DSI1_CTL_BASE;
static struct dsi_phy_regs *const dsi1_phy = (void *)DSI1_PHY_BASE;
static struct dsi_phy_pll_qlink_regs *const phy_pll_qlink1 = (void *)DSI1_PHY_PLL_QLINK_COM;

/* DSI0 uses INTF_1, DSI1 uses INTF_2, eDP uses INTF_5 on x1p42100 */
static struct mdp_intf_regs *const mdp_intf     = (void *)MDP_1_INTF_BASE;
static struct mdp_intf_regs *const mdp_intf2    = (void *)MDP_2_INTF_BASE;
static struct mdp_intf_regs *const mdp_edp_intf = (void *)MDP_5_INTF_BASE;
static struct mdp_ctl_regs *const mdp_ctl_0 = (void *)MDP_0_CTL_BASE;
static struct mdp_layer_mixer_regs *const mdp_layer_mixer = (void *)MDP_VP_0_LAYER_MIXER_BASE;
static struct mdp_sspp_regs *const mdp_sspp = (void *)MDP_VP_0_SSPP_BASE;
static struct vbif_rt_regs *const vbif_rt = (void *)MDP_VBIF_RT_BASE;
static struct mdp_merge_3d_regs *const mdp_merge_3d_0 = (void *)0xAE4F000;
static struct vbif_nrt_regs *const vbif_nrt = (struct vbif_nrt_regs *)VBIF_NRT_BASE;
static struct mdp_periph_top1_regs *const mdp_periph_top1 =
	(struct mdp_periph_top1_regs *)MDP_PERIPH_TOP1;

void merge_3d_active(struct edid *edid, const struct mdss_panel_config *cfg);
void configure_vbif_qos(void);
void intf_tg_setup(struct edid *edid, const struct mdss_panel_config *cfg);
void intf_fetch_start_config(struct edid *edid, const struct mdss_panel_config *cfg);
void mdss_source_pipe_config(struct edid *edid, uintptr_t dram_display,
				     const struct mdss_panel_config *cfg);
void mdss_layer_mixer_setup(struct edid *edid,
				    const struct mdss_panel_config *cfg);
uint32_t calculate_mode_mdp_clk(const struct edid_mode *mode);

#endif // _SOC_DISPLAY_MDSS_REG_H_
