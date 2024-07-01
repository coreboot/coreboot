/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <timer.h>
#include <soc/usb/qmp_usb_phy.h>
#include <soc/addressmap.h>


/* Only for QMP V4 PHY - QSERDES COM registers */
struct usb3_phy_qserdes_com_reg_layout {
	u8 _reserved1[16];
	u32 com_ssc_en_center;
	u32 com_ssc_adj_per1;
	u32 com_ssc_adj_per2;
	u32 com_ssc_per1;
	u32 com_ssc_per2;
	u32 com_ssc_step_size1_mode0;
	u32 com_ssc_step_size2_mode0;
	u32 com_ssc_step_size3_mode0;
	u32 com_ssc_step_size1_mode1;
	u32 com_ssc_step_size2_mode1;
	u32 com_ssc_step_size3_mode1;
	u8 _reserved2[8];
	u32 com_bias_en_clkbuflr_en;
	u32 com_sys_clk_enable1;
	u32 com_sys_clk_ctrl;
	u32 com_sysclk_buf_enable;
	u32 com_pll_en;
	u32 com_pll_ivco;
	u8 _reserved3[4];
	u32 com_cmn_iptrim;
	u8 _reserved4[16];
	u32 com_cp_ctrl_mode0;
	u32 com_cp_ctrl_mode1;
	u32 com_pll_rctrl_mode0;
	u32 com_pll_rctrl_mode1;
	u32 com_pll_cctrl_mode0;
	u32 com_pll_cctrl_mode1;
	u8 _reserved6[8];
	u32 com_sysclk_en_sel;
	u8 _reserved7[8];
	u32 com_resetsm_ctrl2;
	u32 com_lock_cmp_en;
	u32 com_lock_cmp_cfg;
	u32 com_lock_cmp1_mode0;
	u32 com_lock_cmp2_mode0;
	u32 com_lock_cmp1_mode1;
	u32 com_lock_cmp2_mode1;
	u32 com_dec_start_mode0;
	u8 _reserved8[4];
	u32 com_dec_start_mode1;
	u8 _reserved9[4];
	u32 com_div_frac_start1_mode0;
	u32 com_div_frac_start2_mode0;
	u32 com_div_frac_start3_mode0;
	u32 com_div_frac_start1_mode1;
	u32 com_div_frac_start2_mode1;
	u32 com_div_frac_start3_mode1;
	u8 _reserved10[8];
	u32 com_integloop_gain0_mode0;
	u32 com_integloop_gain1_mode0;
	u8 _reserved11[24];
	u32 com_vco_tune_map;
	u32 com_vco_tune1_mode0;
	u32 com_vco_tune2_mode0;
	u32 com_vco_tune1_mode1;
	u32 com_vco_tune2_mode1;
	u8 _reserved12[52];
	u32 com_clk_select;
	u32 com_hsclk_sel;
	u8 _reserved13[12];
	u32 com_coreclk_div_mode0;
	u32 com_coreclk_div_mode1;
	u8 _reserved14[4];
	u32 com_core_clk_en;
	u32 com_c_ready_status;
	u32 com_cmn_config;
	u32 com_cmn_rate_override;
	u32 com_svs_mode_clk_sel;
	u8 _reserved15[36];
	u32 com_bin_vcocal_cmp_code1_mode0;
	u32 com_bin_vcocal_cmp_code2_mode0;
	u32 com_bin_vcocal_cmp_code1_mode1;
	u32 com_bin_vcocal_cmp_code2_mode1;
	u32 com_bin_vcocal_hsclk_sel;
};

check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_en_center, 0x010);
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_adj_per1, 0x014);
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_adj_per2, 0x018);
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_per1, 0x01c);
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_per2, 0x020);
check_member(usb3_phy_qserdes_com_reg_layout, com_bias_en_clkbuflr_en, 0x044);
check_member(usb3_phy_qserdes_com_reg_layout, com_pll_ivco, 0x058);
check_member(usb3_phy_qserdes_com_reg_layout, com_cp_ctrl_mode0, 0x074);
check_member(usb3_phy_qserdes_com_reg_layout, com_sysclk_en_sel, 0x094);
check_member(usb3_phy_qserdes_com_reg_layout, com_resetsm_ctrl2, 0x0a0);
check_member(usb3_phy_qserdes_com_reg_layout, com_dec_start_mode0, 0x0bc);
check_member(usb3_phy_qserdes_com_reg_layout, com_div_frac_start1_mode0, 0x0cc);
check_member(usb3_phy_qserdes_com_reg_layout, com_integloop_gain0_mode0, 0x0ec);
check_member(usb3_phy_qserdes_com_reg_layout, com_vco_tune_map, 0x010c);
check_member(usb3_phy_qserdes_com_reg_layout, com_clk_select, 0x154);
check_member(usb3_phy_qserdes_com_reg_layout, com_coreclk_div_mode0, 0x168);
check_member(usb3_phy_qserdes_com_reg_layout, com_core_clk_en, 0x174);
check_member(usb3_phy_qserdes_com_reg_layout, com_svs_mode_clk_sel, 0x184);
check_member(usb3_phy_qserdes_com_reg_layout, com_bin_vcocal_hsclk_sel, 0x1bc);

/* Only for QMP V4 PHY - TX registers */
struct usb3_phy_qserdes_tx_reg_layout {
	u8 _reserved1[52];
	u32 tx_res_code_lane_tx;
	u32 tx_res_code_lane_rx;
	u32 tx_res_code_lane_offset_tx;
	u32 tx_res_code_lane_offset_rx;
	u8 _reserved2[64];
	u32 tx_lane_mode_1;
	u8 _reserved3[20];
	u32 tx_rcv_detect_lvl_2;
	u8 _reserved4[100];
	u32 tx_pi_qec_ctrl;
};
check_member(usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_tx, 0x03c);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_rx, 0x040);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_1, 0x084);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_rcv_detect_lvl_2, 0x09c);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_pi_qec_ctrl, 0x104);

/* Only for QMP V4 PHY - RX registers */
struct usb3_phy_qserdes_rx_reg_layout {
	u8 _reserved1[20];
	u32 rx_ucdr_so_gain;
	u8 _reserved2[24];
	u32 rx_ucdr_fastlock_fo_gain;
	u32 rx_ucdr_so_saturation_and_enable;
	u8 _reserved3[4];
	u32 rx_ucdr_fastlock_count_low;
	u32 rx_ucdr_fastlock_count_high;
	u32 rx_ucdr_pi_controls;
	u8 _reserved4[4];
	u32 rx_ucdr_sb2_thresh1;
	u32 rx_ucdr_sb2_thresh2;
	u32 rx_ucdr_sb2_gain1;
	u32 rx_ucdr_sb2_gain2;
	u8 _reserved12[4];
	u32 rx_aux_data_tcoarse_tfine;
	u8 _reserved5[112];
	u32 rx_vga_cal_cntrl1;
	u32 rx_vga_cal_cntrl2;
	u32 rx_gm_cal;
	u8 _reserved6[12];
	u32 rx_rx_equ_adaptor_cntrl2;
	u32 rx_rx_equ_adaptor_cntrl3;
	u32 rx_rx_equ_adaptor_cntrl4;
	u32 rx_rx_idac_tsettle_low;
	u32 rx_rx_idac_tsettle_high;
	u8 _reserved7[16];
	u32 rx_rx_eq_offset_adaptor_cntrl1;
	u8 _reserved8[8];
	u32 rx_sigdet_cntrl;
	u8 _reserved9[4];
	u32 rx_sigdet_deglitch_cntrl;
	u8 _reserved10[72];
	u32 rx_rx_mode_00_low;
	u32 rx_rx_mode_00_high;
	u32 rx_rx_mode_00_high2;
	u32 rx_rx_mode_00_high3;
	u32 rx_rx_mode_00_high4;
	u32 rx_rx_mode_01_low;
	u32 rx_rx_mode_01_high;
	u32 rx_rx_mode_01_high2;
	u32 rx_rx_mode_01_high3;
	u32 rx_rx_mode_01_high4;
	u8 _reserved11[28];
	u32 rx_dfe_en_timer;
	u32 rx_dfe_ctle_post_cal_offset;
	u32 rx_dcc_ctrl1;
	u8 _reserved13[4];
	u32 rx_vth_code;
};

check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_so_gain, 0x014);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fastlock_fo_gain, 0x030);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_cntrl1, 0x0d4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl2, 0x0ec);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl3, 0x0f0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl4, 0x0f4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_cntrl, 0x11c);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_dcc_ctrl1, 0x1bc);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_vth_code, 0x1c4);

/* Only for QMP V4 PHY - PCS registers */
struct usb3_phy_pcs_reg_layout {
	u32 pcs_sw_reset;
	u8 _reserved0[16];
	u32 pcs_ready_status;
	u8 _reserved1[40];
	u32 pcs_power_down_control;
	u32 pcs_start_control;
	u8 _reserved2[124];
	u32 pcs_lock_detect_config1;
	u32 pcs_lock_detect_config2;
	u32 pcs_lock_detect_config3;
	u8 _reserved3[8];
	u32 pcs_lock_detect_config6;
	u32 pcs_refgen_req_config1;
	u8 _reserved4[168];
	u32 pcs_rx_sigdet_lvl;
	u8 _reserved5[36];
	u32 pcs_cdr_reset_time;
	u8 _reserved6[12];
	u32 pcs_align_detect_config1;
	u32 pcs_align_detect_config2;
	u8 _reserved7[8];
	u32 pcs_pcs_tx_rx_config;
	u8 _reserved8[8];
	u32 pcs_eq_config1;
	u8 _reserved9[12];
	u32 pcs_eq_config5;
	u8 _reserved10[296];
	u32 pcs_usb3_lfps_det_high_count_val;
	u8 _reserved11[28];
	u32 pcs_usb3_rxeqtraining_dfe_time_s2;
};

check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config1, 0x0c4);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config2, 0x0c8);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config6, 0x0d8);
check_member(usb3_phy_pcs_reg_layout, pcs_pcs_tx_rx_config, 0x1d0);
check_member(usb3_phy_pcs_reg_layout, pcs_eq_config5, 0x1ec);
check_member(usb3_phy_pcs_reg_layout, pcs_usb3_lfps_det_high_count_val, 0x318);
check_member(usb3_phy_pcs_reg_layout, pcs_usb3_rxeqtraining_dfe_time_s2, 0x338);

static struct usb3_phy_qserdes_com_reg_layout *const qserdes_com_reg_layout =
	(void *)QMP_PHY_QSERDES_COM_REG_BASE;
static struct usb3_phy_qserdes_tx_reg_layout *const qserdes_tx_reg_layout =
	(void *)QMP_PHY_QSERDES_TX_REG_BASE;
static struct usb3_phy_qserdes_rx_reg_layout *const qserdes_rx_reg_layout =
	(void *)QMP_PHY_QSERDES_RX_REG_BASE;
static struct usb3_phy_pcs_reg_layout *const pcs_reg_layout =
	(void *)QMP_PHY_PCS_REG_BASE;
static const struct qmp_phy_init_tbl qmp_v4_usb3_serdes_tbl[] = {
	{&qserdes_com_reg_layout->com_ssc_en_center, 0x01},
	{&qserdes_com_reg_layout->com_ssc_per1, 0x31},
	{&qserdes_com_reg_layout->com_ssc_per2, 0x01},
	{&qserdes_com_reg_layout->com_ssc_step_size1_mode0, 0xde},
	{&qserdes_com_reg_layout->com_ssc_step_size2_mode0, 0x07},
	{&qserdes_com_reg_layout->com_ssc_step_size1_mode1, 0xde},
	{&qserdes_com_reg_layout->com_ssc_step_size2_mode1, 0x07},
	{&qserdes_com_reg_layout->com_sysclk_buf_enable, 0x0a},
	{&qserdes_com_reg_layout->com_cmn_iptrim, 0x20},
	{&qserdes_com_reg_layout->com_cp_ctrl_mode0, 0x06},
	{&qserdes_com_reg_layout->com_cp_ctrl_mode1, 0x06},
	{&qserdes_com_reg_layout->com_pll_rctrl_mode0, 0x16},
	{&qserdes_com_reg_layout->com_pll_rctrl_mode1, 0x16},
	{&qserdes_com_reg_layout->com_pll_cctrl_mode0, 0x36},
	{&qserdes_com_reg_layout->com_pll_cctrl_mode1, 0x36},
	{&qserdes_com_reg_layout->com_sysclk_en_sel, 0x1a},
	{&qserdes_com_reg_layout->com_lock_cmp_en, 0x04},
	{&qserdes_com_reg_layout->com_lock_cmp1_mode0, 0x14},
	{&qserdes_com_reg_layout->com_lock_cmp2_mode0, 0x34},
	{&qserdes_com_reg_layout->com_lock_cmp1_mode1, 0x34},
	{&qserdes_com_reg_layout->com_lock_cmp2_mode1, 0x82},
	{&qserdes_com_reg_layout->com_dec_start_mode0, 0x82},
	{&qserdes_com_reg_layout->com_dec_start_mode1, 0x82},
	{&qserdes_com_reg_layout->com_div_frac_start1_mode0, 0xab},
	{&qserdes_com_reg_layout->com_div_frac_start2_mode0, 0xea},
	{&qserdes_com_reg_layout->com_div_frac_start3_mode0, 0x02},
	{&qserdes_com_reg_layout->com_div_frac_start1_mode1, 0xab},
	{&qserdes_com_reg_layout->com_div_frac_start2_mode1, 0xea},
	{&qserdes_com_reg_layout->com_div_frac_start3_mode1, 0x02},
	{&qserdes_com_reg_layout->com_vco_tune_map, 0x02},
	{&qserdes_com_reg_layout->com_vco_tune1_mode0, 0x24},
	{&qserdes_com_reg_layout->com_vco_tune1_mode1, 0x24},
	{&qserdes_com_reg_layout->com_vco_tune2_mode1, 0x02},
	{&qserdes_com_reg_layout->com_hsclk_sel, 0x01},
	{&qserdes_com_reg_layout->com_coreclk_div_mode1, 0x08},
	{&qserdes_com_reg_layout->com_bin_vcocal_cmp_code1_mode0, 0xca},
	{&qserdes_com_reg_layout->com_bin_vcocal_cmp_code2_mode0, 0x1e},
	{&qserdes_com_reg_layout->com_bin_vcocal_cmp_code1_mode1, 0xca},
	{&qserdes_com_reg_layout->com_bin_vcocal_cmp_code2_mode1, 0x1e},
	{&qserdes_com_reg_layout->com_bin_vcocal_hsclk_sel, 0x11},
};

static const struct qmp_phy_init_tbl qmp_v4_usb3_tx_tbl[] = {
	{&qserdes_tx_reg_layout->tx_res_code_lane_tx, 0x60},
	{&qserdes_tx_reg_layout->tx_res_code_lane_rx, 0x60},
	{&qserdes_tx_reg_layout->tx_res_code_lane_offset_tx, 0x11},
	{&qserdes_tx_reg_layout->tx_res_code_lane_offset_rx, 0x02},
	{&qserdes_tx_reg_layout->tx_lane_mode_1, 0xd5},
	{&qserdes_tx_reg_layout->tx_rcv_detect_lvl_2, 0x12},
	{&qserdes_tx_reg_layout->tx_pi_qec_ctrl, 0x40},
};

static const struct qmp_phy_init_tbl qmp_v4_usb3_rx_tbl[] = {
	{&qserdes_rx_reg_layout->rx_ucdr_so_gain, 0x06},
	{&qserdes_rx_reg_layout->rx_ucdr_fastlock_fo_gain, 0x2f},
	{&qserdes_rx_reg_layout->rx_ucdr_so_saturation_and_enable, 0x7f},
	{&qserdes_rx_reg_layout->rx_ucdr_fastlock_count_low, 0xff},
	{&qserdes_rx_reg_layout->rx_ucdr_fastlock_count_high, 0x0f},
	{&qserdes_rx_reg_layout->rx_ucdr_pi_controls, 0x99},
	{&qserdes_rx_reg_layout->rx_ucdr_sb2_thresh1, 0x04},
	{&qserdes_rx_reg_layout->rx_ucdr_sb2_thresh2, 0x08},
	{&qserdes_rx_reg_layout->rx_ucdr_sb2_gain1, 0x05},
	{&qserdes_rx_reg_layout->rx_ucdr_sb2_gain2, 0x05},
	{&qserdes_rx_reg_layout->rx_vga_cal_cntrl1, 0x54},
	{&qserdes_rx_reg_layout->rx_vga_cal_cntrl2, 0x0c},
	{&qserdes_rx_reg_layout->rx_rx_equ_adaptor_cntrl2, 0x0f},
	{&qserdes_rx_reg_layout->rx_rx_equ_adaptor_cntrl3, 0x4a},
	{&qserdes_rx_reg_layout->rx_rx_equ_adaptor_cntrl4, 0x0a},
	{&qserdes_rx_reg_layout->rx_rx_idac_tsettle_low, 0xc0},
	{&qserdes_rx_reg_layout->rx_rx_idac_tsettle_high, 0x00},
	{&qserdes_rx_reg_layout->rx_rx_eq_offset_adaptor_cntrl1, 0x77},
	{&qserdes_rx_reg_layout->rx_sigdet_cntrl, 0x04},
	{&qserdes_rx_reg_layout->rx_sigdet_deglitch_cntrl, 0x0e},
	{&qserdes_rx_reg_layout->rx_rx_mode_00_low, 0xff},
	{&qserdes_rx_reg_layout->rx_rx_mode_00_low, 0x7f},
	{&qserdes_rx_reg_layout->rx_rx_mode_00_high, 0x7f},
	{&qserdes_rx_reg_layout->rx_rx_mode_00_high, 0xff},
	{&qserdes_rx_reg_layout->rx_rx_mode_00_high2, 0x7f},
	{&qserdes_rx_reg_layout->rx_rx_mode_00_high3, 0x7f},
	{&qserdes_rx_reg_layout->rx_rx_mode_00_high4, 0x97},
	{&qserdes_rx_reg_layout->rx_rx_mode_01_low, 0xdc},
	{&qserdes_rx_reg_layout->rx_rx_mode_01_high, 0xdc},
	{&qserdes_rx_reg_layout->rx_rx_mode_01_high2, 0x5c},
	{&qserdes_rx_reg_layout->rx_rx_mode_01_high3, 0x7b},
	{&qserdes_rx_reg_layout->rx_rx_mode_01_high4, 0xb4},
	{&qserdes_rx_reg_layout->rx_dfe_en_timer, 0x04},
	{&qserdes_rx_reg_layout->rx_dfe_ctle_post_cal_offset, 0x38},
	{&qserdes_rx_reg_layout->rx_aux_data_tcoarse_tfine, 0xa0},
	{&qserdes_rx_reg_layout->rx_dcc_ctrl1, 0x0c},
	{&qserdes_rx_reg_layout->rx_gm_cal, 0x1f},
	{&qserdes_rx_reg_layout->rx_vth_code, 0x10},
};

static const struct qmp_phy_init_tbl qmp_v4_usb3_pcs_tbl[] = {
	{&pcs_reg_layout->pcs_lock_detect_config1, 0xd0},
	{&pcs_reg_layout->pcs_lock_detect_config2, 0x07},
	{&pcs_reg_layout->pcs_lock_detect_config3, 0x20},
	{&pcs_reg_layout->pcs_lock_detect_config6, 0x13},
	{&pcs_reg_layout->pcs_refgen_req_config1, 0x21},
	{&pcs_reg_layout->pcs_rx_sigdet_lvl, 0xa9},
	{&pcs_reg_layout->pcs_cdr_reset_time, 0x0a},
	{&pcs_reg_layout->pcs_align_detect_config1, 0x88},
	{&pcs_reg_layout->pcs_align_detect_config2, 0x13},
	{&pcs_reg_layout->pcs_pcs_tx_rx_config, 0x0c},
	{&pcs_reg_layout->pcs_eq_config1, 0x4b},
	{&pcs_reg_layout->pcs_eq_config5, 0x10},
	{&pcs_reg_layout->pcs_usb3_lfps_det_high_count_val, 0xf8},
	{&pcs_reg_layout->pcs_usb3_rxeqtraining_dfe_time_s2, 0x07},
};

struct ss_usb_phy_reg qmp_v4_usb_phy = {
	.serdes_tbl =		qmp_v4_usb3_serdes_tbl,
	.serdes_tbl_num	=	ARRAY_SIZE(qmp_v4_usb3_serdes_tbl),
	.tx_tbl =		qmp_v4_usb3_tx_tbl,
	.tx_tbl_num =		ARRAY_SIZE(qmp_v4_usb3_tx_tbl),
	.rx_tbl =		qmp_v4_usb3_rx_tbl,
	.rx_tbl_num =		ARRAY_SIZE(qmp_v4_usb3_rx_tbl),
	.pcs_tbl =		qmp_v4_usb3_pcs_tbl,
	.pcs_tbl_num =		ARRAY_SIZE(qmp_v4_usb3_pcs_tbl),
	.qmp_pcs_reg =		(void *)QMP_PHY_PCS_REG_BASE,
};

static void qcom_qmp_phy_configure(const struct qmp_phy_init_tbl tbl[],
				int num)
{
	int i;
	const struct qmp_phy_init_tbl *t = tbl;

	if (!t)
		return;

	for (i = 0; i < num; i++, t++)
		write32(t->address, t->val);
}

void ss_qmp_phy_init(void)
{
	struct ss_usb_phy_reg *ss_phy_reg;

	ss_phy_reg = &qmp_v4_usb_phy;

	/* power up USB3 PHY */
	write32(&ss_phy_reg->qmp_pcs_reg->pcs_power_down_control, 0x01);

	 /* Serdes configuration */
	qcom_qmp_phy_configure(ss_phy_reg->serdes_tbl,
		ss_phy_reg->serdes_tbl_num);
	/* Tx, Rx, and PCS configurations */
	qcom_qmp_phy_configure(ss_phy_reg->tx_tbl, ss_phy_reg->tx_tbl_num);
	qcom_qmp_phy_configure(ss_phy_reg->rx_tbl, ss_phy_reg->rx_tbl_num);
	qcom_qmp_phy_configure(ss_phy_reg->pcs_tbl, ss_phy_reg->pcs_tbl_num);

	/* perform software reset of PCS/Serdes */
	write32(&ss_phy_reg->qmp_pcs_reg->pcs_sw_reset, 0x00);
	/* start PCS/Serdes to operation mode */
	write32(&ss_phy_reg->qmp_pcs_reg->pcs_start_control, 0x03);

	/*
	 * Wait for PHY initialization to be done
	 * PCS_STATUS: wait for 1ms for PHY STATUS;
	 * SW can continuously check for PHYSTATUS = 1.b0.
	 */
	long lock_us = wait_us(10000,
			!(read32(&ss_phy_reg->qmp_pcs_reg->pcs_ready_status) &
			USB3_PCS_PHYSTATUS));
	if (!lock_us)
		printk(BIOS_ERR, "QMP PHY PLL LOCK fails:\n");
	else
		printk(BIOS_DEBUG, "QMP PHY initialized and locked in %ldus\n",
				lock_us);
}
