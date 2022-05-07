/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef HASWELL_RAMINIT_REG_STRUCTS_H
#define HASWELL_RAMINIT_REG_STRUCTS_H

union ddr_data_rx_train_rank_reg {
	struct __packed {
		uint32_t rcven : 9; // Bits  8:0
		uint32_t dqs_p : 6; // Bits 14:9
		uint32_t rx_eq : 5; // Bits 19:15
		uint32_t dqs_n : 6; // Bits 25:20
		int32_t  vref  : 6; // Bits 31:26
	};
	uint32_t raw;
};

union ddr_data_tx_train_rank_reg {
	struct __packed {
		uint32_t dq_delay  : 9; // Bits  8:0
		uint32_t dqs_delay : 9; // Bits 17:9
		uint32_t           : 2; // Bits 19:18
		uint32_t tx_eq     : 6; // Bits 25:20
		uint32_t           : 6; // Bits 31:26
	};
	uint32_t raw;
};

union ddr_data_control_0_reg {
	struct __packed {
		uint32_t rx_training_mode      : 1; // Bits  0:0
		uint32_t wl_training_mode      : 1; // Bits  1:1
		uint32_t rl_training_mode      : 1; // Bits  2:2
		uint32_t samp_train_mode       : 1; // Bits  3:3
		uint32_t tx_on                 : 1; // Bits  4:4
		uint32_t rf_on                 : 1; // Bits  5:5
		uint32_t rx_pi_on              : 1; // Bits  6:6
		uint32_t tx_pi_on              : 1; // Bits  7:7
		uint32_t internal_clocks_on    : 1; // Bits  8:8
		uint32_t repeater_clocks_on    : 1; // Bits  9:9
		uint32_t tx_disable            : 1; // Bits 10:10
		uint32_t rx_disable            : 1; // Bits 11:11
		uint32_t tx_long               : 1; // Bits 12:12
		uint32_t rx_dqs_ctle           : 2; // Bits 14:13
		uint32_t rx_read_pointer       : 3; // Bits 17:15
		uint32_t driver_segment_enable : 1; // Bits 18:18
		uint32_t data_vccddq_hi        : 1; // Bits 19:19
		uint32_t read_rf_rd            : 1; // Bits 20:20
		uint32_t read_rf_wr            : 1; // Bits 21:21
		uint32_t read_rf_rank          : 2; // Bits 23:22
		uint32_t force_odt_on          : 1; // Bits 24:24
		uint32_t odt_samp_off          : 1; // Bits 25:25
		uint32_t disable_odt_static    : 1; // Bits 26:26
		uint32_t ddr_cr_force_odt_on   : 1; // Bits 27:27
		uint32_t lpddr_mode            : 1; // Bits 28:28
		uint32_t en_read_preamble      : 1; // Bits 29:29
		uint32_t odt_samp_extend_en    : 1; // Bits 30:30
		uint32_t early_rleak_en        : 1; // Bits 31:31
	};
	uint32_t raw;
};

union ddr_data_control_1_reg {
	struct __packed {
		int32_t  ref_pi               : 4; // Bits 3:0
		uint32_t dll_mask             : 2; // Bits 5:4
		uint32_t dll_weaklock         : 1; // Bits 6:6
		uint32_t sdll_segment_disable : 3; // Bits 9:7
		uint32_t rx_bias_ctl          : 3; // Bits 12:10
		int32_t  odt_delay            : 4; // Bits 16:13
		uint32_t odt_duration         : 3; // Bits 19:17
		int32_t  sense_amp_delay      : 4; // Bits 23:20
		uint32_t sense_amp_duration   : 3; // Bits 26:24
		uint32_t burst_end_odt_delay  : 3; // Bits 29:27   *** TODO: Check Broadwell ***
		uint32_t lpddr_long_odt_en    : 1; // Bits 30:30
		uint32_t                      : 1; // Bits 31:31
	};
	uint32_t raw;
};

/* NOTE: Bits 31:19 are only valid for Broadwell onwards */
union ddr_data_control_2_reg {
	struct __packed {
		uint32_t rx_stagger_ctl    : 5; // Bits  4:0
		uint32_t force_bias_on     : 1; // Bits  5:5
		uint32_t force_rx_on       : 1; // Bits  6:6
		uint32_t leaker_comp       : 2; // Bits  8:7
		uint32_t rx_dqs_amp_offset : 4; // Bits 12:9
		uint32_t rx_clk_stg_num    : 5; // Bits 17:13
		uint32_t wl_long_delay     : 1; // Bits 18:18
		uint32_t enable_vref_pwrdn : 1; // Bits 19:19
		uint32_t ddr4_mode         : 1; // Bits 20:20
		uint32_t en_vddq_odt       : 1; // Bits 21:21
		uint32_t en_vtt_odt        : 1; // Bits 22:22
		uint32_t en_const_z_eq_tx  : 1; // Bits 23:23
		uint32_t tx_eq_dis         : 1; // Bits 24:24
		uint32_t rx_vref_prog_mfc  : 1; // Bits 25:25
		uint32_t cben              : 3; // Bits 28:26
		uint32_t tx_deskew_disable : 1; // Bits 29:29
		uint32_t rx_deskew_disable : 1; // Bits 30:30
		uint32_t dq_slew_dly_byp   : 1; // Bits 31:31
	};
	uint32_t raw;
};

union ddr_comp_data_comp_1_reg {
	struct __packed {
		uint32_t rcomp_odt_up   : 6; // Bits  5:0
		uint32_t                : 3; // Bits  8:6
		uint32_t rcomp_odt_down : 6; // Bits 14:9
		uint32_t                : 1; // Bits 15:15
		uint32_t panic_drv_down : 6; // Bits 21:16
		uint32_t panic_drv_up   : 6; // Bits 27:22
		uint32_t ls_comp        : 3; // Bits 30:28
		uint32_t                : 1; // Bits 31:31
	};
	uint32_t raw;
};

union ddr_comp_ctl_0_reg {
	struct __packed {
		uint32_t                    : 3; // Bits  2:0
		uint32_t disable_odt_static : 1; // Bits  3:3
		uint32_t odt_up_down_off    : 6; // Bits  9:4
		uint32_t fixed_odt_offset   : 1; // Bits 10:10
		int32_t  dq_drv_vref        : 4; // Bits 14:11
		int32_t  dq_odt_vref        : 5; // Bits 19:15
		int32_t  cmd_drv_vref       : 4; // Bits 23:20
		int32_t  ctl_drv_vref       : 4; // Bits 27:24
		int32_t  clk_drv_vref       : 4; // Bits 31:28
	};
	uint32_t raw;
};

union ddr_comp_ctl_1_reg {
	struct __packed {
		uint32_t dq_scomp       : 5; // Bits  4:0
		uint32_t cmd_scomp      : 5; // Bits  9:5
		uint32_t ctl_scomp      : 5; // Bits 14:10
		uint32_t clk_scomp      : 5; // Bits 19:15
		uint32_t tco_cmd_offset : 4; // Bits 23:20
		uint32_t comp_clk_on    : 1; // Bits 24:24
		uint32_t vccddq_hi      : 1; // Bits 25:25
		uint32_t                : 3; // Bits 28:26
		uint32_t dis_quick_comp : 1; // Bits 29:29
		uint32_t sin_step       : 1; // Bits 30:30
		uint32_t sin_step_adv   : 1; // Bits 31:31
	};
	uint32_t raw;
};

union ddr_data_vref_adjust_reg {
	struct __packed {
		int32_t  ca_vref_ctrl     : 7;// Bits  6:0
		int32_t  ch1_vref_ctrl    : 7;// Bits 13:7
		int32_t  ch0_vref_ctrl    : 7;// Bits 20:14
		uint32_t en_dimm_vref_ca  : 1;// Bits 21:21
		uint32_t en_dimm_vref_ch1 : 1;// Bits 22:22
		uint32_t en_dimm_vref_ch0 : 1;// Bits 23:23
		uint32_t hi_z_timer_ctrl  : 2;// Bits 25:24
		uint32_t vccddq_hi_qnnn_h : 1;// Bits 26:26
		uint32_t                  : 2;// Bits 28:27
		uint32_t ca_slow_bw       : 1;// Bits 29:29
		uint32_t ch0_slow_bw      : 1;// Bits 30:30
		uint32_t ch1_slow_bw      : 1;// Bits 31:31
	};
	uint32_t raw;
};

union ddr_data_vref_control_reg {
	struct __packed {
		uint32_t hi_bw_divider   : 2; // Bits  1:0
		uint32_t lo_bw_divider   : 2; // Bits  3:2
		uint32_t sample_divider  : 3; // Bits  6:4
		uint32_t open_loop       : 1; // Bits  7:7
		uint32_t slow_bw_error   : 2; // Bits  9:8
		uint32_t hi_bw_enable    : 1; // Bits 10:10
		uint32_t                 : 1; // Bits 11:11
		uint32_t vt_slope_b      : 3; // Bits 14:12
		uint32_t vt_slope_a      : 3; // Bits 17:15
		uint32_t vt_offset       : 3; // Bits 20:18
		uint32_t sel_code        : 3; // Bits 23:21
		uint32_t output_code     : 8; // Bits 31:24
	};
	uint32_t raw;
};

union ddr_comp_vsshi_reg {
	struct __packed {
		uint32_t panic_drv_down_vref : 6; // Bits  5:0
		uint32_t panic_drv_up_vref   : 6; // Bits 11:6
		uint32_t vt_offset           : 5; // Bits 16:12
		uint32_t vt_slope_a          : 3; // Bits 19:17
		uint32_t vt_slope_b          : 3; // Bits 22:20
		uint32_t                     : 9; // Bits 31:23
	};
	uint32_t raw;
};

union ddr_comp_vsshi_control_reg {
	struct __packed {
		uint32_t vsshi_target    : 6; // Bits  5:0
		uint32_t hi_bw_divider   : 2; // Bits  7:6
		uint32_t lo_bw_divider   : 2; // Bits  9:8
		uint32_t sample_divider  : 3; // Bits 12:10
		uint32_t open_loop       : 1; // Bits 13:13
		uint32_t bw_error        : 2; // Bits 15:14
		uint32_t panic_driver_en : 1; // Bits 16:16
		uint32_t                 : 1; // Bits 17:17
		uint32_t panic_voltage   : 4; // Bits 21:18
		uint32_t gain_boost      : 1; // Bits 22:22
		uint32_t sel_code        : 1; // Bits 23:23
		uint32_t output_code     : 8; // Bits 31:24
	};
	uint32_t raw;
};

union ddr_clk_controls_reg {
	struct __packed {
		uint32_t ref_pi             : 4; // Bits  3:0
		uint32_t dll_mask           : 2; // Bits  5:4
		uint32_t                    : 1; // Bits  6:6
		uint32_t tx_on              : 1; // Bits  7:7
		uint32_t internal_clocks_on : 1; // Bits  8:8
		uint32_t repeater_clocks_on : 1; // Bits  9:9
		uint32_t io_lb_ctl          : 2; // Bits 11:10
		uint32_t odt_mode           : 1; // Bits 12:12
		uint32_t                    : 8; // Bits 20:13
		uint32_t rx_vref            : 6; // Bits 26:21
		uint32_t vccddq_hi          : 1; // Bits 27:27
		uint32_t dll_weaklock       : 1; // Bits 28:28
		uint32_t lpddr_mode         : 1; // Bits 29:29
		uint32_t                    : 2; // Bits 31:30
	};
	uint32_t raw;
};

union ddr_cmd_controls_reg {
	struct __packed {
		int32_t  ref_pi             : 4; // Bits  3:0
		uint32_t dll_mask           : 2; // Bits  5:4
		uint32_t                    : 1; // Bits  6:6
		uint32_t tx_on              : 1; // Bits  7:7
		uint32_t internal_clocks_on : 1; // Bits  8:8
		uint32_t repeater_clocks_on : 1; // Bits  9:9
		uint32_t io_lb_ctl          : 2; // Bits 11:10
		uint32_t odt_mode           : 1; // Bits 12:12
		uint32_t cmd_tx_eq          : 2; // Bits 14:13
		uint32_t early_weak_drive   : 2; // Bits 16:15
		uint32_t                    : 4; // Bits 20:17
		int32_t  rx_vref            : 6; // Bits 26:21
		uint32_t vccddq_hi          : 1; // Bits 27:27
		uint32_t dll_weaklock       : 1; // Bits 28:28
		uint32_t lpddr_mode         : 1; // Bits 29:29
		uint32_t lpddr_ca_a_dis     : 1; // Bits 30:30
		uint32_t lpddr_ca_b_dis     : 1; // Bits 31:31
	};
	uint32_t raw;
};

/* Same register definition for CKE and CTL fubs */
union ddr_cke_ctl_controls_reg {
	struct __packed {
		int32_t  ref_pi             : 4; // Bits  3:0
		uint32_t dll_mask           : 2; // Bits  5:4
		uint32_t                    : 1; // Bits  6:6
		uint32_t tx_on              : 1; // Bits  7:7
		uint32_t internal_clocks_on : 1; // Bits  8:8
		uint32_t repeater_clocks_on : 1; // Bits  9:9
		uint32_t io_lb_ctl          : 2; // Bits 11:10
		uint32_t odt_mode           : 1; // Bits 12:12
		uint32_t cmd_tx_eq          : 2; // Bits 14:13
		uint32_t early_weak_drive   : 2; // Bits 16:15
		uint32_t ctl_tx_eq          : 2; // Bits 18:17
		uint32_t ctl_sr_drv         : 2; // Bits 20:19
		int32_t  rx_vref            : 6; // Bits 26:21
		uint32_t vccddq_hi          : 1; // Bits 27:27
		uint32_t dll_weaklock       : 1; // Bits 28:28
		uint32_t lpddr_mode         : 1; // Bits 29:29
		uint32_t la_drv_en_ovrd     : 1; // Bits 30:30
		uint32_t lpddr_ca_a_dis     : 1; // Bits 31:31
	};
	uint32_t raw;
};

union ddr_scram_misc_control_reg {
	struct __packed {
		uint32_t wl_wake_cycles       :  2; // Bits  1:0
		uint32_t wl_sleep_cycles      :  3; // Bits  4:2
		uint32_t force_comp_update    :  1; // Bits  5:5
		uint32_t weaklock_latency     :  4; // Bits  9:6
		uint32_t ddr_no_ch_interleave :  1; // Bits 10:10
		uint32_t lpddr_mode           :  1; // Bits 11:11
		uint32_t cke_mapping_ch0      :  4; // Bits 15:12
		uint32_t cke_mapping_ch1      :  4; // Bits 19:16
		uint32_t                      : 12; // Bits 31:20
	};
	uint32_t raw;
};

union mcscheds_cbit_reg {
	struct __packed {
		uint32_t dis_opp_cas    : 1; // Bits  0:0
		uint32_t dis_opp_is_cas : 1; // Bits  1:1
		uint32_t dis_opp_ras    : 1; // Bits  2:2
		uint32_t dis_opp_is_ras : 1; // Bits  3:3
		uint32_t dis_1c_byp     : 1; // Bits  4:4
		uint32_t dis_2c_byp     : 1; // Bits  5:5
		uint32_t dis_deprd_opt  : 1; // Bits  6:6
		uint32_t dis_pt_it      : 1; // Bits  7:7
		uint32_t dis_prcnt_ring : 1; // Bits  8:8
		uint32_t dis_prcnt_sa   : 1; // Bits  9:9
		uint32_t dis_blkr_ph    : 1; // Bits 10:10
		uint32_t dis_blkr_pe    : 1; // Bits 11:11
		uint32_t dis_blkr_pm    : 1; // Bits 12:12
		uint32_t dis_odt        : 1; // Bits 13:13
		uint32_t oe_always_off  : 1; // Bits 14:14
		uint32_t                : 1; // Bits 15:15
		uint32_t dis_aom        : 1; // Bits 16:16
		uint32_t block_rpq      : 1; // Bits 17:17
		uint32_t block_wpq      : 1; // Bits 18:18
		uint32_t invert_align   : 1; // Bits 19:19
		uint32_t dis_write_gap  : 1; // Bits 20:20
		uint32_t dis_zq         : 1; // Bits 21:21
		uint32_t dis_tt         : 1; // Bits 22:22
		uint32_t dis_opp_ref    : 1; // Bits 23:23
		uint32_t long_zq        : 1; // Bits 24:24
		uint32_t dis_srx_zq     : 1; // Bits 25:25
		uint32_t serialize_zq   : 1; // Bits 26:26
		uint32_t zq_fast_exec   : 1; // Bits 27:27
		uint32_t dis_drive_nop  : 1; // Bits 28:28
		uint32_t pres_wdb_ent   : 1; // Bits 29:29
		uint32_t dis_clk_gate   : 1; // Bits 30:30
		uint32_t                : 1; // Bits 31:31
	};
	uint32_t raw;
};

union mcmain_command_rate_limit_reg {
	struct __packed {
		uint32_t enable_cmd_limit :  1; // Bits  0:0
		uint32_t cmd_rate_limit   :  3; // Bits  3:1
		uint32_t reset_on_command :  4; // Bits  7:4
		uint32_t reset_delay      :  4; // Bits 11:8
		uint32_t ck_to_cke_delay  :  2; // Bits 13:12
		uint32_t                  : 17; // Bits 30:14
		uint32_t init_mrw_2n_cs   :  1; // Bits 31:31
	};
	uint32_t raw;
};

union mad_chnl_reg {
	struct __packed {
		uint32_t ch_a           :  2; // Bits  1:0
		uint32_t ch_b           :  2; // Bits  3:2
		uint32_t ch_c           :  2; // Bits  5:4
		uint32_t stacked_mode   :  1; // Bits  6:6
		uint32_t stkd_mode_bits :  3; // Bits  9:7
		uint32_t lpddr_mode     :  1; // Bits 10:10
		uint32_t                : 21; // Bits 31:11
	};
	uint32_t raw;
};

union mad_dimm_reg {
	struct __packed {
		uint32_t dimm_a_size     :  8;  // Bits  7:0
		uint32_t dimm_b_size     :  8;  // Bits 15:8
		uint32_t dimm_a_sel      :  1;  // Bits 16:16
		uint32_t dimm_a_ranks    :  1;  // Bits 17:17
		uint32_t dimm_b_ranks    :  1;  // Bits 18:18
		uint32_t dimm_a_width    :  1;  // Bits 19:19
		uint32_t dimm_b_width    :  1;  // Bits 20:20
		uint32_t rank_interleave :  1;  // Bits 21:21
		uint32_t enh_interleave  :  1;  // Bits 22:22
		uint32_t                 :  1;  // Bits 23:23
		uint32_t ecc_mode        :  2;  // Bits 25:24
		uint32_t hori_mode       :  1;  // Bits 26:26
		uint32_t hori_address    :  3;  // Bits 29:27
		uint32_t                 :  2;  // Bits 31:30
	};
	uint32_t raw;
};

union mad_zr_reg {
	struct __packed {
		uint32_t             : 16; // Bits 15:0
		uint32_t ch_b_double :  8; // Bits 23:16
		uint32_t ch_b_single :  8; // Bits 31:24
	};
	uint32_t raw;
};

/* Same definition for P_COMP, M_COMP, D_COMP */
union pcu_comp_reg {
	struct __packed {
		uint32_t comp_disable  :  1; // Bits  0:0
		uint32_t comp_interval :  4; // Bits  4:1
		uint32_t               :  3; // Bits  7:5
		uint32_t comp_force    :  1; // Bits  8:8
		uint32_t               : 23; // Bits 31:9
	};
	uint32_t raw;
};

#endif
