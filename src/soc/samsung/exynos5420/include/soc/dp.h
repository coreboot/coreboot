/* SPDX-License-Identifier: GPL-2.0-only */

/* Register map for Exynos5 DP */

#ifndef CPU_SAMSUNG_EXYNOS5420_DP_H
#define CPU_SAMSUNG_EXYNOS5420_DP_H

#include <soc/cpu.h>

/* DSIM register map */
struct exynos_dp {
	u8	res1[0x10];
	u32	tx_version;
	u32	tx_sw_reset;
	u32	func_en1;
	u32	func_en2;
	u32	video_ctl1;
	u32	video_ctl2;
	u32	video_ctl3;
	u32	video_ctl4;
	u32	color_blue_cb;
	u32	color_green_y;
	u32	color_red_cr;
	u32	video_ctl8;
	u8	res2[0x4];
	u32	video_ctl10;
	u32	total_ln_cfg_l;
	u32	total_ln_cfg_h;
	u32	active_ln_cfg_l;
	u32	active_ln_cfg_h;
	u32	vfp_cfg;
	u32	vsw_cfg;
	u32	vbp_cfg;
	u32	total_pix_cfg_l;
	u32	total_pix_cfg_h;
	u32	active_pix_cfg_l;
	u32	active_pix_cfg_h;
	u32	hfp_cfg_l;
	u32	hfp_cfg_h;
	u32	hsw_cfg_l;
	u32	hsw_cfg_h;
	u32	hbp_cfg_l;
	u32	hbp_cfg_h;
	u32	video_status;
	u32	total_ln_sta_l;
	u32	total_ln_sta_h;
	u32	active_ln_sta_l;
	u32	active_ln_sta_h;

	u32	vfp_sta;
	u32	vsw_sta;
	u32	vbp_sta;

	u32	total_pix_sta_l;
	u32	total_pix_sta_h;
	u32	active_pix_sta_l;
	u32	active_pix_sta_h;

	u32	hfp_sta_l;
	u32	hfp_sta_h;
	u32	hsw_sta_l;
	u32	hsw_sta_h;
	u32	hbp_sta_l;
	u32	hbp_sta_h;

	u8	res3[0x288];

	u32	lane_map;
	u8	res4[0x10];
	u32	analog_ctl1;
	u32	analog_ctl2;
	u32	analog_ctl3;

	u32	pll_filter_ctl1;
	u32	amp_tuning_ctl;
	u8	res5[0xc];

	u32	aux_hw_retry_ctl;
	u8	res6[0x2c];
	u32	int_state;
	u32	common_int_sta1;
	u32	common_int_sta2;
	u32	common_int_sta3;
	u32	common_int_sta4;
	u8	res7[0x8];

	u32	int_sta;
	u8	res8[0x1c];
	u32	int_ctl;
	u8	res9[0x200];
	u32	sys_ctl1;
	u32	sys_ctl2;
	u32	sys_ctl3;
	u32	sys_ctl4;
	u32	vid_ctl;
	u8	res10[0x2c];
	u32	pkt_send_ctl;
	u8	res[0x4];
	u32	hdcp_ctl;
	u8	res11[0x34];
	u32	link_bw_set;

	u32	lane_count_set;
	u32	training_ptn_set;
	u32	ln0_link_training_ctl;
	u32	ln1_link_training_ctl;
	u32	ln2_link_training_ctl;
	u32	ln3_link_training_ctl;
	u32	dn_spread_ctl;
	u32	hw_link_training_ctl;
	u8	res12[0x1c];

	u32	debug_ctl;
	u32	hpd_deglitch_l;
	u32	hpd_deglitch_h;

	u8	res13[0x14];
	u32	link_debug_ctl;

	u8	res14[0x1c];

	u32	m_vid0;
	u32	m_vid1;
	u32	m_vid2;
	u32	n_vid0;
	u32	n_vid1;
	u32	n_vid2;
	u32	m_vid_mon;
	u32	pll_ctl;
	u32	phy_pd;
	u32	phy_test;
	u8	res15[0x8];

	u32	video_fifo_thrd;
	u8	res16[0x8];
	u32	audio_margin;

	u32	dn_spread_ctl1;
	u32	dn_spread_ctl2;
	u8	res17[0x18];
	u32	m_cal_ctl;
	u32	m_vid_gen_filter_th;
	u8	res18[0x10];
	u32	m_aud_gen_filter_th;
	u8	res50[0x4];

	u32	aux_ch_sta;
	u32	aux_err_num;
	u32	aux_ch_defer_ctl;
	u32	aux_rx_comm;
	u32	buffer_data_ctl;

	u32	aux_ch_ctl1;
	u32	aux_addr_7_0;
	u32	aux_addr_15_8;
	u32	aux_addr_19_16;
	u32	aux_ch_ctl2;
	u8	res19[0x18];
	u32	buf_data0;
	u8	res20[0x3c];

	u32	soc_general_ctl;
	u8	res21[0x8c];
	u32	crc_con;
	u32	crc_result;
	u8	res22[0x8];

	u32	common_int_mask1;
	u32	common_int_mask2;
	u32	common_int_mask3;
	u32	common_int_mask4;
	u32	int_sta_mask1;
	u32	int_sta_mask2;
	u32	int_sta_mask3;
	u32	int_sta_mask4;
	u32	int_sta_mask;
	u32	crc_result2;
	u32	scrambler_reset_cnt;

	u32	pn_inv;
	u32	psr_config;
	u32	psr_command0;
	u32	psr_command1;
	u32	psr_crc_mon0;
	u32	psr_crc_mon1;

	u8	res24[0x30];
	u32	phy_bist_ctrl;
	u8	res25[0xc];
	u32	phy_ctrl;
	u8	res26[0x1c];
	u32	test_pattern_gen_en;
	u32	test_pattern_gen_ctrl;
};
check_member(exynos_dp, phy_ctrl, 0x924);

static struct exynos_dp * const exynos_dp0 = (void *)EXYNOS5_DP0_BASE;
static struct exynos_dp * const exynos_dp1 = (void *)EXYNOS5_DP1_BASE;

/* For DP VIDEO CTL 1 */
#define VIDEO_EN_MASK				(0x01 << 7)
#define VIDEO_MUTE_MASK				(0x01 << 6)

/* For DP VIDEO CTL 4 */
#define VIDEO_BIST_MASK				(0x1 << 3)

/* EXYNOS_DP_ANALOG_CTL_1 */
#define SEL_BG_NEW_BANDGAP			(0x0 << 6)
#define SEL_BG_INTERNAL_RESISTOR		(0x1 << 6)
#define TX_TERMINAL_CTRL_73_OHM			(0x0 << 4)
#define TX_TERMINAL_CTRL_61_OHM			(0x1 << 4)
#define TX_TERMINAL_CTRL_50_OHM			(0x2 << 4)
#define TX_TERMINAL_CTRL_45_OHM			(0x3 << 4)
#define SWING_A_30PER_G_INCREASE		(0x1 << 3)
#define SWING_A_30PER_G_NORMAL			(0x0 << 3)

/* EXYNOS_DP_ANALOG_CTL_2 */
#define CPREG_BLEED				(0x1 << 4)
#define SEL_24M					(0x1 << 3)
#define TX_DVDD_BIT_1_0000V			(0x3 << 0)
#define TX_DVDD_BIT_1_0625V			(0x4 << 0)
#define TX_DVDD_BIT_1_1250V			(0x5 << 0)

/* EXYNOS_DP_ANALOG_CTL_3 */
#define DRIVE_DVDD_BIT_1_0000V			(0x3 << 5)
#define DRIVE_DVDD_BIT_1_0625V			(0x4 << 5)
#define DRIVE_DVDD_BIT_1_1250V			(0x5 << 5)
#define SEL_CURRENT_DEFAULT			(0x0 << 3)
#define VCO_BIT_000_MICRO			(0x0 << 0)
#define VCO_BIT_200_MICRO			(0x1 << 0)
#define VCO_BIT_300_MICRO			(0x2 << 0)
#define VCO_BIT_400_MICRO			(0x3 << 0)
#define VCO_BIT_500_MICRO			(0x4 << 0)
#define VCO_BIT_600_MICRO			(0x5 << 0)
#define VCO_BIT_700_MICRO			(0x6 << 0)
#define VCO_BIT_900_MICRO			(0x7 << 0)

/* EXYNOS_DP_PLL_FILTER_CTL_1 */
#define PD_RING_OSC				(0x1 << 6)
#define AUX_TERMINAL_CTRL_52_OHM		(0x3 << 4)
#define AUX_TERMINAL_CTRL_69_OHM		(0x2 << 4)
#define AUX_TERMINAL_CTRL_102_OHM		(0x1 << 4)
#define AUX_TERMINAL_CTRL_200_OHM		(0x0 << 4)
#define TX_CUR1_1X				(0x0 << 2)
#define TX_CUR1_2X				(0x1 << 2)
#define TX_CUR1_3X				(0x2 << 2)
#define TX_CUR_1_MA				(0x0 << 0)
#define TX_CUR_2_MA			        (0x1 << 0)
#define TX_CUR_3_MA				(0x2 << 0)
#define TX_CUR_4_MA				(0x3 << 0)

/* EXYNOS_DP_PLL_FILTER_CTL_2 */
#define CH3_AMP_0_MV				(0x3 << 12)
#define CH2_AMP_0_MV				(0x3 << 8)
#define CH1_AMP_0_MV				(0x3 << 4)
#define CH0_AMP_0_MV				(0x3 << 0)

/* EXYNOS_DP_PLL_CTL */
#define DP_PLL_PD			        (0x1 << 7)
#define DP_PLL_RESET				(0x1 << 6)
#define DP_PLL_LOOP_BIT_DEFAULT		        (0x1 << 4)
#define DP_PLL_REF_BIT_1_1250V			(0x5 << 0)
#define DP_PLL_REF_BIT_1_2500V		        (0x7 << 0)

/* EXYNOS_DP_INT_CTL */
#define SOFT_INT_CTRL				(0x1 << 2)
#define INT_POL					(0x1 << 0)

/* DP TX SW RESET */
#define RESET_DP_TX				(0x01 << 0)

/* DP FUNC_EN_1 */
#define MASTER_VID_FUNC_EN_N			(0x1 << 7)
#define SLAVE_VID_FUNC_EN_N			(0x1 << 5)
#define AUD_FIFO_FUNC_EN_N			(0x1 << 4)
#define AUD_FUNC_EN_N				(0x1 << 3)
#define HDCP_FUNC_EN_N				(0x1 << 2)
#define CRC_FUNC_EN_N				(0x1 << 1)
#define SW_FUNC_EN_N				(0x1 << 0)

/* DP FUNC_EN_2 */
#define SSC_FUNC_EN_N			        (0x1 << 7)
#define AUX_FUNC_EN_N				(0x1 << 2)
#define SERDES_FIFO_FUNC_EN_N			(0x1 << 1)
#define LS_CLK_DOMAIN_FUNC_EN_N		        (0x1 << 0)

/* EXYNOS_DP_PHY_PD */
#define PHY_PD					(0x1 << 5)
#define AUX_PD					(0x1 << 4)
#define CH3_PD					(0x1 << 3)
#define CH2_PD					(0x1 << 2)
#define CH1_PD					(0x1 << 1)
#define CH0_PD					(0x1 << 0)

/* EXYNOS_DP_COMMON_INT_STA_1 */
#define VSYNC_DET				(0x1 << 7)
#define PLL_LOCK_CHG				(0x1 << 6)
#define SPDIF_ERR				(0x1 << 5)
#define SPDIF_UNSTBL				(0x1 << 4)
#define VID_FORMAT_CHG				(0x1 << 3)
#define AUD_CLK_CHG				(0x1 << 2)
#define VID_CLK_CHG				(0x1 << 1)
#define SW_INT					(0x1 << 0)

/* EXYNOS_DP_DEBUG_CTL */
#define PLL_LOCK				(0x1 << 4)
#define F_PLL_LOCK				(0x1 << 3)
#define PLL_LOCK_CTRL				(0x1 << 2)

/* EXYNOS_DP_FUNC_EN_2 */
#define SSC_FUNC_EN_N				(0x1 << 7)
#define AUX_FUNC_EN_N				(0x1 << 2)
#define SERDES_FIFO_FUNC_EN_N			(0x1 << 1)
#define LS_CLK_DOMAIN_FUNC_EN_N			(0x1 << 0)

/* EXYNOS_DP_COMMON_INT_STA_4 */
#define PSR_ACTIVE				(0x1 << 7)
#define PSR_INACTIVE				(0x1 << 6)
#define SPDIF_BI_PHASE_ERR			(0x1 << 5)
#define HOTPLUG_CHG				(0x1 << 2)
#define HPD_LOST				(0x1 << 1)
#define PLUG					(0x1 << 0)

/* EXYNOS_DP_INT_STA */
#define INT_HPD					(0x1 << 6)
#define HW_TRAINING_FINISH			(0x1 << 5)
#define RPLY_RECEIV				(0x1 << 1)
#define AUX_ERR					(0x1 << 0)

/* EXYNOS_DP_SYS_CTL_3 */
#define HPD_STATUS				(0x1 << 6)
#define F_HPD					(0x1 << 5)
#define HPD_CTRL				(0x1 << 4)
#define HDCP_RDY				(0x1 << 3)
#define STRM_VALID				(0x1 << 2)
#define F_VALID					(0x1 << 1)
#define VALID_CTRL				(0x1 << 0)

/* EXYNOS_DP_AUX_HW_RETRY_CTL */
#define AUX_BIT_PERIOD_EXPECTED_DELAY(x)	(((x) & 0x7) << 8)
#define AUX_HW_RETRY_INTERVAL_MASK		(0x3 << 3)
#define AUX_HW_RETRY_INTERVAL_600_MICROSECONDS	(0x0 << 3)
#define AUX_HW_RETRY_INTERVAL_800_MICROSECONDS	(0x1 << 3)
#define AUX_HW_RETRY_INTERVAL_1000_MICROSECONDS	(0x2 << 3)
#define AUX_HW_RETRY_INTERVAL_1800_MICROSECONDS	(0x3 << 3)
#define AUX_HW_RETRY_COUNT_SEL(x)		(((x) & 0x7) << 0)

/* EXYNOS_DP_AUX_CH_DEFER_CTL */
#define DEFER_CTRL_EN				(0x1 << 7)
#define DEFER_COUNT(x)				(((x) & 0x7f) << 0)

#define COMMON_INT_MASK_1			(0)
#define COMMON_INT_MASK_2			(0)
#define COMMON_INT_MASK_3			(0)
#define COMMON_INT_MASK_4			(0)
#define INT_STA_MASK				(0)

/* EXYNOS_DP_BUFFER_DATA_CTL */
#define BUF_CLR					(0x1 << 7)
#define BUF_DATA_COUNT(x)			(((x) & 0x1f) << 0)

/* EXYNOS_DP_AUX_ADDR_7_0 */
#define AUX_ADDR_7_0(x)				(((x) >> 0) & 0xff)

/* EXYNOS_DP_AUX_ADDR_15_8 */
#define AUX_ADDR_15_8(x)			(((x) >> 8) & 0xff)

/* EXYNOS_DP_AUX_ADDR_19_16 */
#define AUX_ADDR_19_16(x)			(((x) >> 16) & 0x0f)

/* EXYNOS_DP_AUX_CH_CTL_1 */
#define AUX_LENGTH(x)				(((x - 1) & 0xf) << 4)
#define AUX_TX_COMM_MASK			(0xf << 0)
#define AUX_TX_COMM_DP_TRANSACTION		(0x1 << 3)
#define AUX_TX_COMM_I2C_TRANSACTION		(0x0 << 3)
#define AUX_TX_COMM_MOT				(0x1 << 2)
#define AUX_TX_COMM_WRITE			(0x0 << 0)
#define AUX_TX_COMM_READ			(0x1 << 0)

/* EXYNOS_DP_AUX_CH_CTL_2 */
#define ADDR_ONLY				(0x1 << 1)
#define AUX_EN					(0x1 << 0)

/* EXYNOS_DP_AUX_CH_STA */
#define AUX_BUSY				(0x1 << 4)
#define AUX_STATUS_MASK				(0xf << 0)

/* EXYNOS_DP_AUX_RX_COMM */
#define AUX_RX_COMM_I2C_DEFER			(0x2 << 2)
#define AUX_RX_COMM_AUX_DEFER			(0x2 << 0)

/* EXYNOS_DP_PHY_TEST */
#define MACRO_RST				(0x1 << 5)
#define CH1_TEST				(0x1 << 1)
#define CH0_TEST				(0x1 << 0)

/* EXYNOS_DP_TRAINING_PTN_SET */
#define SCRAMBLER_TYPE				(0x1 << 9)
#define HW_LINK_TRAINING_PATTERN		(0x1 << 8)
#define SCRAMBLING_DISABLE			(0x1 << 5)
#define SCRAMBLING_ENABLE			(0x0 << 5)
#define LINK_QUAL_PATTERN_SET_MASK		(0x3 << 2)
#define LINK_QUAL_PATTERN_SET_PRBS7		(0x3 << 2)
#define LINK_QUAL_PATTERN_SET_D10_2		(0x1 << 2)
#define LINK_QUAL_PATTERN_SET_DISABLE		(0x0 << 2)
#define SW_TRAINING_PATTERN_SET_MASK		(0x3 << 0)
#define SW_TRAINING_PATTERN_SET_PTN2		(0x2 << 0)
#define SW_TRAINING_PATTERN_SET_PTN1		(0x1 << 0)
#define SW_TRAINING_PATTERN_SET_NORMAL		(0x0 << 0)

/* EXYNOS_DP_TOTAL_LINE_CFG */
#define TOTAL_LINE_CFG_L(x)			((x) & 0xff)
#define TOTAL_LINE_CFG_H(x)			((((x) >> 8)) & 0xff)
#define ACTIVE_LINE_CFG_L(x)			((x) & 0xff)
#define ACTIVE_LINE_CFG_H(x)			(((x) >> 8) & 0xff)
#define TOTAL_PIXEL_CFG_L(x)			((x) & 0xff)
#define TOTAL_PIXEL_CFG_H(x)			((((x) >> 8)) & 0xff)
#define ACTIVE_PIXEL_CFG_L(x)			((x) & 0xff)
#define ACTIVE_PIXEL_CFG_H(x)			((((x) >> 8)) & 0xff)

#define H_F_PORCH_CFG_L(x)			((x) & 0xff)
#define H_F_PORCH_CFG_H(x)			((((x) >> 8)) & 0xff)
#define H_SYNC_PORCH_CFG_L(x)			((x) & 0xff)
#define H_SYNC_PORCH_CFG_H(x)			((((x) >> 8)) & 0xff)
#define H_B_PORCH_CFG_L(x)			((x) & 0xff)
#define H_B_PORCH_CFG_H(x)			((((x) >> 8)) & 0xff)

/* EXYNOS_DP_LN0_LINK_TRAINING_CTL */
#define MAX_PRE_EMPHASIS_REACH_0		(0x1 << 5)
#define PRE_EMPHASIS_SET_0_SET(x)		(((x) & 0x3) << 3)
#define PRE_EMPHASIS_SET_0_GET(x)		(((x) >> 3) & 0x3)
#define PRE_EMPHASIS_SET_0_MASK			(0x3 << 3)
#define PRE_EMPHASIS_SET_0_SHIFT		(3)
#define PRE_EMPHASIS_SET_0_LEVEL_3		(0x3 << 3)
#define PRE_EMPHASIS_SET_0_LEVEL_2		(0x2 << 3)
#define PRE_EMPHASIS_SET_0_LEVEL_1		(0x1 << 3)
#define PRE_EMPHASIS_SET_0_LEVEL_0		(0x0 << 3)
#define MAX_DRIVE_CURRENT_REACH_0		(0x1 << 2)
#define DRIVE_CURRENT_SET_0_MASK		(0x3 << 0)
#define DRIVE_CURRENT_SET_0_SET(x)		(((x) & 0x3) << 0)
#define DRIVE_CURRENT_SET_0_GET(x)		(((x) >> 0) & 0x3)
#define DRIVE_CURRENT_SET_0_LEVEL_3		(0x3 << 0)
#define DRIVE_CURRENT_SET_0_LEVEL_2		(0x2 << 0)
#define DRIVE_CURRENT_SET_0_LEVEL_1		(0x1 << 0)
#define DRIVE_CURRENT_SET_0_LEVEL_0		(0x0 << 0)

/* EXYNOS_DP_LN1_LINK_TRAINING_CTL */
#define MAX_PRE_EMPHASIS_REACH_1		(0x1 << 5)
#define PRE_EMPHASIS_SET_1_SET(x)		(((x) & 0x3) << 3)
#define PRE_EMPHASIS_SET_1_GET(x)		(((x) >> 3) & 0x3)
#define PRE_EMPHASIS_SET_1_MASK			(0x3 << 3)
#define PRE_EMPHASIS_SET_1_SHIFT		(3)
#define PRE_EMPHASIS_SET_1_LEVEL_3		(0x3 << 3)
#define PRE_EMPHASIS_SET_1_LEVEL_2		(0x2 << 3)
#define PRE_EMPHASIS_SET_1_LEVEL_1		(0x1 << 3)
#define PRE_EMPHASIS_SET_1_LEVEL_0		(0x0 << 3)
#define MAX_DRIVE_CURRENT_REACH_1		(0x1 << 2)
#define DRIVE_CURRENT_SET_1_MASK		(0x3 << 0)
#define DRIVE_CURRENT_SET_1_SET(x)		(((x) & 0x3) << 0)
#define DRIVE_CURRENT_SET_1_GET(x)		(((x) >> 0) & 0x3)
#define DRIVE_CURRENT_SET_1_LEVEL_3		(0x3 << 0)
#define DRIVE_CURRENT_SET_1_LEVEL_2		(0x2 << 0)
#define DRIVE_CURRENT_SET_1_LEVEL_1		(0x1 << 0)
#define DRIVE_CURRENT_SET_1_LEVEL_0		(0x0 << 0)

/* EXYNOS_DP_LN2_LINK_TRAINING_CTL */
#define MAX_PRE_EMPHASIS_REACH_2		(0x1 << 5)
#define PRE_EMPHASIS_SET_2_SET(x)		(((x) & 0x3) << 3)
#define PRE_EMPHASIS_SET_2_GET(x)		(((x) >> 3) & 0x3)
#define PRE_EMPHASIS_SET_2_MASK			(0x3 << 3)
#define PRE_EMPHASIS_SET_2_SHIFT		(3)
#define PRE_EMPHASIS_SET_2_LEVEL_3		(0x3 << 3)
#define PRE_EMPHASIS_SET_2_LEVEL_2		(0x2 << 3)
#define PRE_EMPHASIS_SET_2_LEVEL_1		(0x1 << 3)
#define PRE_EMPHASIS_SET_2_LEVEL_0		(0x0 << 3)
#define MAX_DRIVE_CURRENT_REACH_2		(0x1 << 2)
#define DRIVE_CURRENT_SET_2_MASK		(0x3 << 0)
#define DRIVE_CURRENT_SET_2_SET(x)		(((x) & 0x3) << 0)
#define DRIVE_CURRENT_SET_2_GET(x)		(((x) >> 0) & 0x3)
#define DRIVE_CURRENT_SET_2_LEVEL_3		(0x3 << 0)
#define DRIVE_CURRENT_SET_2_LEVEL_2		(0x2 << 0)
#define DRIVE_CURRENT_SET_2_LEVEL_1		(0x1 << 0)
#define DRIVE_CURRENT_SET_2_LEVEL_0		(0x0 << 0)

/* EXYNOS_DP_LN3_LINK_TRAINING_CTL */
#define MAX_PRE_EMPHASIS_REACH_3		(0x1 << 5)
#define PRE_EMPHASIS_SET_3_SET(x)		(((x) & 0x3) << 3)
#define PRE_EMPHASIS_SET_3_GET(x)		(((x) >> 3) & 0x3)
#define PRE_EMPHASIS_SET_3_MASK			(0x3 << 3)
#define PRE_EMPHASIS_SET_3_SHIFT		(3)
#define PRE_EMPHASIS_SET_3_LEVEL_3		(0x3 << 3)
#define PRE_EMPHASIS_SET_3_LEVEL_2		(0x2 << 3)
#define PRE_EMPHASIS_SET_3_LEVEL_1		(0x1 << 3)
#define PRE_EMPHASIS_SET_3_LEVEL_0		(0x0 << 3)
#define MAX_DRIVE_CURRENT_REACH_3		(0x1 << 2)
#define DRIVE_CURRENT_SET_3_MASK		(0x3 << 0)
#define DRIVE_CURRENT_SET_3_SET(x)		(((x) & 0x3) << 0)
#define DRIVE_CURRENT_SET_3_GET(x)		(((x) >> 0) & 0x3)
#define DRIVE_CURRENT_SET_3_LEVEL_3		(0x3 << 0)
#define DRIVE_CURRENT_SET_3_LEVEL_2		(0x2 << 0)
#define DRIVE_CURRENT_SET_3_LEVEL_1		(0x1 << 0)
#define DRIVE_CURRENT_SET_3_LEVEL_0		(0x0 << 0)

/* EXYNOS_DP_VIDEO_CTL_10 */
#define FORMAT_SEL				(0x1 << 4)
#define INTERACE_SCAN_CFG			(0x1 << 2)
#define INTERACE_SCAN_CFG_SHIFT			(2)
#define VSYNC_POLARITY_CFG			(0x1 << 1)
#define V_S_POLARITY_CFG_SHIFT			(1)
#define HSYNC_POLARITY_CFG			(0x1 << 0)
#define H_S_POLARITY_CFG_SHIFT			(0)

/* EXYNOS_DP_SOC_GENERAL_CTL */
#define AUDIO_MODE_SPDIF_MODE			(0x1 << 8)
#define AUDIO_MODE_MASTER_MODE			(0x0 << 8)
#define MASTER_VIDEO_INTERLACE_EN		(0x1 << 4)
#define VIDEO_MASTER_CLK_SEL			(0x1 << 2)
#define VIDEO_MASTER_MODE_EN			(0x1 << 1)
#define VIDEO_MODE_MASK				(0x1 << 0)
#define VIDEO_MODE_SLAVE_MODE			(0x1 << 0)
#define VIDEO_MODE_MASTER_MODE			(0x0 << 0)

/* EXYNOS_DP_VIDEO_CTL_1 */
#define VIDEO_EN				(0x1 << 7)
#define HDCP_VIDEO_MUTE				(0x1 << 6)

/* EXYNOS_DP_VIDEO_CTL_2 */
#define IN_D_RANGE_MASK				(0x1 << 7)
#define IN_D_RANGE_SHIFT			(7)
#define IN_D_RANGE_CEA				(0x1 << 7)
#define IN_D_RANGE_VESA				(0x0 << 7)
#define IN_BPC_MASK				(0x7 << 4)
#define IN_BPC_SHIFT				(4)
#define IN_BPC_12_BITS				(0x3 << 4)
#define IN_BPC_10_BITS				(0x2 << 4)
#define IN_BPC_8_BITS				(0x1 << 4)
#define IN_BPC_6_BITS				(0x0 << 4)
#define IN_COLOR_F_MASK				(0x3 << 0)
#define IN_COLOR_F_SHIFT			(0)
#define IN_COLOR_F_YCBCR444			(0x2 << 0)
#define IN_COLOR_F_YCBCR422			(0x1 << 0)
#define IN_COLOR_F_RGB				(0x0 << 0)

/* EXYNOS_DP_VIDEO_CTL_3 */
#define IN_YC_COEFFI_MASK			(0x1 << 7)
#define IN_YC_COEFFI_SHIFT			(7)
#define IN_YC_COEFFI_ITU709			(0x1 << 7)
#define IN_YC_COEFFI_ITU601			(0x0 << 7)
#define VID_CHK_UPDATE_TYPE_MASK		(0x1 << 4)
#define VID_CHK_UPDATE_TYPE_SHIFT		(4)
#define VID_CHK_UPDATE_TYPE_1			(0x1 << 4)
#define VID_CHK_UPDATE_TYPE_0			(0x0 << 4)

/* EXYNOS_DP_TEST_PATTERN_GEN_EN */
#define TEST_PATTERN_GEN_EN			(0x1 << 0)
#define TEST_PATTERN_GEN_DIS			(0x0 << 0)

/* EXYNOS_DP_TEST_PATTERN_GEN_CTRL */
#define TEST_PATTERN_MODE_COLOR_SQUARE		(0x3 << 0)
#define TEST_PATTERN_MODE_BALCK_WHITE_V_LINES	(0x2 << 0)
#define TEST_PATTERN_MODE_COLOR_RAMP		(0x1 << 0)

/* EXYNOS_DP_VIDEO_CTL_4 */
#define BIST_EN					(0x1 << 3)
#define BIST_WIDTH_MASK				(0x1 << 2)
#define BIST_WIDTH_BAR_32_PIXEL			(0x0 << 2)
#define BIST_WIDTH_BAR_64_PIXEL			(0x1 << 2)
#define BIST_TYPE_MASK				(0x3 << 0)
#define BIST_TYPE_COLOR_BAR			(0x0 << 0)
#define BIST_TYPE_WHITE_GRAY_BLACK_BAR		(0x1 << 0)
#define BIST_TYPE_MOBILE_WHITE_BAR		(0x2 << 0)

/* EXYNOS_DP_SYS_CTL_1 */
#define DET_STA					(0x1 << 2)
#define FORCE_DET				(0x1 << 1)
#define DET_CTRL				(0x1 << 0)

/* EXYNOS_DP_SYS_CTL_2 */
#define CHA_CRI(x)				(((x) & 0xf) << 4)
#define CHA_STA					(0x1 << 2)
#define FORCE_CHA				(0x1 << 1)
#define CHA_CTRL				(0x1 << 0)

/* EXYNOS_DP_SYS_CTL_3 */
#define HPD_STATUS				(0x1 << 6)
#define F_HPD					(0x1 << 5)
#define HPD_CTRL				(0x1 << 4)
#define HDCP_RDY				(0x1 << 3)
#define STRM_VALID				(0x1 << 2)
#define F_VALID					(0x1 << 1)
#define VALID_CTRL				(0x1 << 0)

/* EXYNOS_DP_SYS_CTL_4 */
#define FIX_M_AUD				(0x1 << 4)
#define ENHANCED				(0x1 << 3)
#define FIX_M_VID				(0x1 << 2)
#define M_VID_UPDATE_CTRL			(0x3 << 0)

/* EXYNOS_M_VID_X */
#define M_VID0_CFG(x)				((x) & 0xff)
#define M_VID1_CFG(x)				(((x) >> 8) & 0xff)
#define M_VID2_CFG(x)				(((x) >> 16) & 0xff)

/* EXYNOS_M_VID_X */
#define N_VID0_CFG(x)				((x) & 0xff)
#define N_VID1_CFG(x)				(((x) >> 8) & 0xff)
#define N_VID2_CFG(x)				(((x) >> 16) & 0xff)

/* DPCD_TRAINING_PATTERN_SET */
#define DPCD_SCRAMBLING_DISABLED		(0x1 << 5)
#define DPCD_SCRAMBLING_ENABLED			(0x0 << 5)
#define DPCD_TRAINING_PATTERN_2			(0x2 << 0)
#define DPCD_TRAINING_PATTERN_1			(0x1 << 0)
#define DPCD_TRAINING_PATTERN_DISABLED		(0x0 << 0)

/* Definition for DPCD Register */
#define DPCD_DPCD_REV				(0x0000)
#define DPCD_MAX_LINK_RATE			(0x0001)
#define DPCD_MAX_LANE_COUNT			(0x0002)
#define DPCD_LINK_BW_SET			(0x0100)
#define DPCD_LANE_COUNT_SET			(0x0101)
#define DPCD_TRAINING_PATTERN_SET		(0x0102)
#define DPCD_TRAINING_LANE0_SET			(0x0103)
#define DPCD_LANE0_1_STATUS			(0x0202)
#define DPCD_LN_ALIGN_UPDATED			(0x0204)
#define DPCD_ADJUST_REQUEST_LANE0_1		(0x0206)
#define DPCD_ADJUST_REQUEST_LANE2_3		(0x0207)
#define DPCD_TEST_REQUEST			(0x0218)
#define DPCD_TEST_RESPONSE			(0x0260)
#define DPCD_TEST_EDID_CHECKSUM			(0x0261)
#define DPCD_SINK_POWER_STATE			(0x0600)

/* DPCD_TEST_REQUEST */
#define DPCD_TEST_EDID_READ			(0x1 << 2)

/* DPCD_TEST_RESPONSE */
#define DPCD_TEST_EDID_CHECKSUM_WRITE		(0x1 << 2)

/* DPCD_SINK_POWER_STATE */
#define DPCD_SET_POWER_STATE_D0			(0x1 << 0)
#define DPCD_SET_POWER_STATE_D4			(0x2 << 0)

/* I2C EDID Chip ID, Slave Address */
#define I2C_EDID_DEVICE_ADDR			(0x50)
#define I2C_E_EDID_DEVICE_ADDR			(0x30)
#define EDID_BLOCK_LENGTH			(0x80)
#define EDID_HEADER_PATTERN			(0x00)
#define EDID_EXTENSION_FLAG			(0x7e)
#define EDID_CHECKSUM				(0x7f)

/* DPCD_LANE0_1_STATUS */
#define DPCD_LANE1_SYMBOL_LOCKED		(0x1 << 6)
#define DPCD_LANE1_CHANNEL_EQ_DONE		(0x1 << 5)
#define DPCD_LANE1_CR_DONE			(0x1 << 4)
#define DPCD_LANE0_SYMBOL_LOCKED		(0x1 << 2)
#define DPCD_LANE0_CHANNEL_EQ_DONE		(0x1 << 1)
#define DPCD_LANE0_CR_DONE			(0x1 << 0)

/* DPCD_ADJUST_REQUEST_LANE0_1 */
#define DPCD_PRE_EMPHASIS_LANE1_MASK		(0x3 << 6)
#define DPCD_PRE_EMPHASIS_LANE1(x)		(((x) >> 6) & 0x3)
#define DPCD_PRE_EMPHASIS_LANE1_LEVEL_3		(0x3 << 6)
#define DPCD_PRE_EMPHASIS_LANE1_LEVEL_2		(0x2 << 6)
#define DPCD_PRE_EMPHASIS_LANE1_LEVEL_1		(0x1 << 6)
#define DPCD_PRE_EMPHASIS_LANE1_LEVEL_0		(0x0 << 6)
#define DPCD_VOLTAGE_SWING_LANE1_MASK		(0x3 << 4)
#define DPCD_VOLTAGE_SWING_LANE1(x)		(((x) >> 4) & 0x3)
#define DPCD_VOLTAGE_SWING_LANE1_LEVEL_3	(0x3 << 4)
#define DPCD_VOLTAGE_SWING_LANE1_LEVEL_2	(0x2 << 4)
#define DPCD_VOLTAGE_SWING_LANE1_LEVEL_1	(0x1 << 4)
#define DPCD_VOLTAGE_SWING_LANE1_LEVEL_0	(0x0 << 4)
#define DPCD_PRE_EMPHASIS_LANE0_MASK		(0x3 << 2)
#define DPCD_PRE_EMPHASIS_LANE0(x)		(((x) >> 2) & 0x3)
#define DPCD_PRE_EMPHASIS_LANE0_LEVEL_3		(0x3 << 2)
#define DPCD_PRE_EMPHASIS_LANE0_LEVEL_2		(0x2 << 2)
#define DPCD_PRE_EMPHASIS_LANE0_LEVEL_1		(0x1 << 2)
#define DPCD_PRE_EMPHASIS_LANE0_LEVEL_0		(0x0 << 2)
#define DPCD_VOLTAGE_SWING_LANE0_MASK		(0x3 << 0)
#define DPCD_VOLTAGE_SWING_LANE0(x)		(((x) >> 0) & 0x3)
#define DPCD_VOLTAGE_SWING_LANE0_LEVEL_3	(0x3 << 0)
#define DPCD_VOLTAGE_SWING_LANE0_LEVEL_2	(0x2 << 0)
#define DPCD_VOLTAGE_SWING_LANE0_LEVEL_1	(0x1 << 0)
#define DPCD_VOLTAGE_SWING_LANE0_LEVEL_0	(0x0 << 0)

/* DPCD_ADJUST_REQUEST_LANE2_3 */
#define DPCD_PRE_EMPHASIS_LANE2_MASK		(0x3 << 6)
#define DPCD_PRE_EMPHASIS_LANE2(x)		(((x) >> 6) & 0x3)
#define DPCD_PRE_EMPHASIS_LANE2_LEVEL_3		(0x3 << 6)
#define DPCD_PRE_EMPHASIS_LANE2_LEVEL_2		(0x2 << 6)
#define DPCD_PRE_EMPHASIS_LANE2_LEVEL_1		(0x1 << 6)
#define DPCD_PRE_EMPHASIS_LANE2_LEVEL_0		(0x0 << 6)
#define DPCD_VOLTAGE_SWING_LANE2_MASK		(0x3 << 4)
#define DPCD_VOLTAGE_SWING_LANE2(x)		(((x) >> 4) & 0x3)
#define DPCD_VOLTAGE_SWING_LANE2_LEVEL_3	(0x3 << 4)
#define DPCD_VOLTAGE_SWING_LANE2_LEVEL_2	(0x2 << 4)
#define DPCD_VOLTAGE_SWING_LANE2_LEVEL_1	(0x1 << 4)
#define DPCD_VOLTAGE_SWING_LANE2_LEVEL_0	(0x0 << 4)
#define DPCD_PRE_EMPHASIS_LANE3_MASK		(0x3 << 2)
#define DPCD_PRE_EMPHASIS_LANE3(x)		(((x) >> 2) & 0x3)
#define DPCD_PRE_EMPHASIS_LANE3_LEVEL_3		(0x3 << 2)
#define DPCD_PRE_EMPHASIS_LANE3_LEVEL_2		(0x2 << 2)
#define DPCD_PRE_EMPHASIS_LANE3_LEVEL_1		(0x1 << 2)
#define DPCD_PRE_EMPHASIS_LANE3_LEVEL_0		(0x0 << 2)
#define DPCD_VOLTAGE_SWING_LANE3_MASK		(0x3 << 0)
#define DPCD_VOLTAGE_SWING_LANE3(x)		(((x) >> 0) & 0x3)
#define DPCD_VOLTAGE_SWING_LANE3_LEVEL_3	(0x3 << 0)
#define DPCD_VOLTAGE_SWING_LANE3_LEVEL_2	(0x2 << 0)
#define DPCD_VOLTAGE_SWING_LANE3_LEVEL_1	(0x1 << 0)
#define DPCD_VOLTAGE_SWING_LANE3_LEVEL_0	(0x0 << 0)

/* DPCD_LANE_COUNT_SET */
#define DPCD_ENHANCED_FRAME_EN			(0x1 << 7)
#define DPCD_LN_COUNT_SET(x)			((x) & 0x1f)

/* DPCD_LANE_ALIGN__STATUS_UPDATED */
#define DPCD_LINK_STATUS_UPDATED		(0x1 << 7)
#define DPCD_DOWNSTREAM_PORT_STATUS_CHANGED	(0x1 << 6)
#define DPCD_INTERLANE_ALIGN_DONE		(0x1 << 0)

/* DPCD_TRAINING_LANE0_SET */
#define DPCD_PRE_EMPHASIS_SET_PATTERN_2_LEVEL_3		(0x3 << 3)
#define DPCD_PRE_EMPHASIS_SET_PATTERN_2_LEVEL_2		(0x2 << 3)
#define DPCD_PRE_EMPHASIS_SET_PATTERN_2_LEVEL_1		(0x1 << 3)
#define DPCD_PRE_EMPHASIS_SET_PATTERN_2_LEVEL_0		(0x0 << 3)
#define DPCD_VOLTAGE_SWING_SET_PATTERN_1_LEVEL_3	(0x3 << 0)
#define DPCD_VOLTAGE_SWING_SET_PATTERN_1_LEVEL_2	(0x2 << 0)
#define DPCD_VOLTAGE_SWING_SET_PATTERN_1_LEVEL_1	(0x1 << 0)
#define DPCD_VOLTAGE_SWING_SET_PATTERN_1_LEVEL_0	(0x0 << 0)

#define DPCD_REQ_ADJ_SWING			(0x00)
#define DPCD_REQ_ADJ_EMPHASIS			(0x01)

#define DP_LANE_STAT_CR_DONE			(0x01 << 0)
#define DP_LANE_STAT_CE_DONE			(0x01 << 1)
#define DP_LANE_STAT_SYM_LOCK			(0x01 << 2)

struct exynos_fb {
	u32 vidcon0;
	u32 vidcon1;
	u32 vidcon2;
	u32 vidcon3;
	u32 vidtcon0;
	u32 vidtcon1;
	u32 vidtcon2;
	u32 vidtcon3;
	u32 wincon0;
	u32 wincon1;
	u32 wincon2;
	u32 wincon3;
	u32 wincon4;

	u32 winshmap;
	u32 res1;

	u32 winchmap2;
	u32 vidosd0a;
	u32 vidosd0b;
	u32 vidosd0c;
	u32 res2;

	u32 vidosd1a;
	u32 vidosd1b;
	u32 vidosd1c;
	u32 vidosd1d;

	u32 vidosd2a;
	u32 vidosd2b;
	u32 vidosd2c;
	u32 vidosd2d;

	u32 vidosd3a;
	u32 vidosd3b;
	u32 vidosd3c;
	u32 res3;

	u32 vidosd4a;
	u32 vidosd4b;
	u32 vidosd4c;
	u32 res4[5];

	u32 vidw00add0b0;
	u32 vidw00add0b1;
	u32 vidw01add0b0;
	u32 vidw01add0b1;

	u32 vidw02add0b0;
	u32 vidw02add0b1;
	u32 vidw03add0b0;
	u32 vidw03add0b1;
	u32 vidw04add0b0;
	u32 vidw04add0b1;
	u32 res5[2];

	u32 vidw00add1b0;
	u32 vidw00add1b1;
	u32 vidw01add1b0;
	u32 vidw01add1b1;

	u32 vidw02add1b0;
	u32 vidw02add1b1;
	u32 vidw03add1b0;
	u32 vidw03add1b1;

	u32 vidw04add1b0;
	u32 vidw04add1b1;
	u32 res7[2];

	u32 vidw00add2;
	u32 vidw01add2;
	u32 vidw02add2;
	u32 vidw03add2;
	u32 vidw04add2;
	u32 res8[7];

	u32 vidintcon0;
	u32 vidintcon1;
	u32 res9[1];

	u32 w1keycon0;
	u32 w1keycon1;
	u32 w2keycon0;
	u32 w2keycon1;
	u32 w3keycon0;
	u32 w3keycon1;
	u32 w4keycon0;
	u32 w4keycon1;

	u32 w1keyalpha;
	u32 w2keyalpha;
	u32 w3keyalpha;
	u32 w4keyalpha;

	u32 dithmode;
	u32 res10[2];

	u32 win0map;
	u32 win1map;
	u32 win2map;
	u32 win3map;
	u32 win4map;
	u32 res11[1];

	u32 wpalcon_h;
	u32 wpalcon_l;

	u32 trigcon;
	u32 res12[2];

	u32 i80ifcona0;
	u32 i80ifcona1;
	u32 i80ifconb0;
	u32 i80ifconb1;

	u32 colorgaincon;
	u32 res13[2];

	u32 ldi_cmdcon0;
	u32 ldi_cmdcon1;
	u32 res14[1];

	/* To be updated */

	u8 res15[156];
	u32 dualrgb;
	u8 res16[16];
	u32 dp_mie_clkcon;
};
/* TODO: can't decipher this, someone add a check_member() please */

/* LCD IF register offset */
#define EXYNOS5_LCD_IF_BASE_OFFSET			0x20000

/*
 *  Register offsets
*/
#define EXYNOS_WINCON(x)				(x)
#define EXYNOS_VIDOSD(x)				(x * 4)
#define EXYNOS_BUFFER_OFFSET(x)				(x * 2)
#define EXYNOS_BUFFER_SIZE(x)				(x)

/*
 * Bit Definitions
*/

/* VIDCON0 */
#define EXYNOS_VIDCON0_DSI_DISABLE			(0 << 30)
#define EXYNOS_VIDCON0_DSI_ENABLE			(1 << 30)
#define EXYNOS_VIDCON0_SCAN_PROGRESSIVE			(0 << 29)
#define EXYNOS_VIDCON0_SCAN_INTERLACE			(1 << 29)
#define EXYNOS_VIDCON0_SCAN_MASK			(1 << 29)
#define EXYNOS_VIDCON0_VIDOUT_RGB			(0 << 26)
#define EXYNOS_VIDCON0_VIDOUT_ITU			(1 << 26)
#define EXYNOS_VIDCON0_VIDOUT_I80LDI0			(2 << 26)
#define EXYNOS_VIDCON0_VIDOUT_I80LDI1			(3 << 26)
#define EXYNOS_VIDCON0_VIDOUT_WB_RGB			(4 << 26)
#define EXYNOS_VIDCON0_VIDOUT_WB_I80LDI0		(6 << 26)
#define EXYNOS_VIDCON0_VIDOUT_WB_I80LDI1		(7 << 26)
#define EXYNOS_VIDCON0_VIDOUT_MASK			(7 << 26)
#define EXYNOS_VIDCON0_PNRMODE_RGB_P			(0 << 17)
#define EXYNOS_VIDCON0_PNRMODE_BGR_P			(1 << 17)
#define EXYNOS_VIDCON0_PNRMODE_RGB_S			(2 << 17)
#define EXYNOS_VIDCON0_PNRMODE_BGR_S			(3 << 17)
#define EXYNOS_VIDCON0_PNRMODE_MASK			(3 << 17)
#define EXYNOS_VIDCON0_PNRMODE_SHIFT			(17)
#define EXYNOS_VIDCON0_CLKVALUP_ALWAYS			(0 << 16)
#define EXYNOS_VIDCON0_CLKVALUP_START_FRAME		(1 << 16)
#define EXYNOS_VIDCON0_CLKVALUP_MASK			(1 << 16)
#define EXYNOS_VIDCON0_CLKVAL_F(x)			(((x) & 0xff) << 6)
#define EXYNOS_VIDCON0_VCLKEN_NORMAL			(0 << 5)
#define EXYNOS_VIDCON0_VCLKEN_FREERUN			(1 << 5)
#define EXYNOS_VIDCON0_VCLKEN_MASK			(1 << 5)
#define EXYNOS_VIDCON0_CLKDIR_DIRECTED			(0 << 4)
#define EXYNOS_VIDCON0_CLKDIR_DIVIDED			(1 << 4)
#define EXYNOS_VIDCON0_CLKDIR_MASK			(1 << 4)
#define EXYNOS_VIDCON0_CLKSEL_HCLK			(0 << 2)
#define EXYNOS_VIDCON0_CLKSEL_SCLK			(1 << 2)
#define EXYNOS_VIDCON0_CLKSEL_MASK			(1 << 2)
#define EXYNOS_VIDCON0_ENVID_ENABLE			(1 << 1)
#define EXYNOS_VIDCON0_ENVID_DISABLE			(0 << 1)
#define EXYNOS_VIDCON0_ENVID_F_ENABLE			(1 << 0)
#define EXYNOS_VIDCON0_ENVID_F_DISABLE			(0 << 0)

/* VIDCON1 */
#define EXYNOS_VIDCON1_IVCLK_FALLING_EDGE		(0 << 7)
#define EXYNOS_VIDCON1_IVCLK_RISING_EDGE		(1 << 7)
#define EXYNOS_VIDCON1_IHSYNC_NORMAL			(0 << 6)
#define EXYNOS_VIDCON1_IHSYNC_INVERT			(1 << 6)
#define EXYNOS_VIDCON1_IVSYNC_NORMAL			(0 << 5)
#define EXYNOS_VIDCON1_IVSYNC_INVERT			(1 << 5)
#define EXYNOS_VIDCON1_IVDEN_NORMAL			(0 << 4)
#define EXYNOS_VIDCON1_IVDEN_INVERT			(1 << 4)

/* VIDCON2 */
#define EXYNOS_VIDCON2_EN601_DISABLE			(0 << 23)
#define EXYNOS_VIDCON2_EN601_ENABLE			(1 << 23)
#define EXYNOS_VIDCON2_EN601_MASK			(1 << 23)
#define EXYNOS_VIDCON2_WB_DISABLE			(0 << 15)
#define EXYNOS_VIDCON2_WB_ENABLE			(1 << 15)
#define EXYNOS_VIDCON2_WB_MASK				(1 << 15)
#define EXYNOS_VIDCON2_TVFORMATSEL_HW			(0 << 14)
#define EXYNOS_VIDCON2_TVFORMATSEL_SW			(1 << 14)
#define EXYNOS_VIDCON2_TVFORMATSEL_MASK			(1 << 14)
#define EXYNOS_VIDCON2_TVFORMATSEL_YUV422		(1 << 12)
#define EXYNOS_VIDCON2_TVFORMATSEL_YUV444		(2 << 12)
#define EXYNOS_VIDCON2_TVFORMATSEL_YUV_MASK		(3 << 12)
#define EXYNOS_VIDCON2_ORGYUV_YCBCR			(0 << 8)
#define EXYNOS_VIDCON2_ORGYUV_CBCRY			(1 << 8)
#define EXYNOS_VIDCON2_ORGYUV_MASK			(1 << 8)
#define EXYNOS_VIDCON2_YUVORD_CBCR			(0 << 7)
#define EXYNOS_VIDCON2_YUVORD_CRCB			(1 << 7)
#define EXYNOS_VIDCON2_YUVORD_MASK			(1 << 7)

/* PRTCON */
#define EXYNOS_PRTCON_UPDATABLE				(0 << 11)
#define EXYNOS_PRTCON_PROTECT				(1 << 11)

/* VIDTCON0 */
#define EXYNOS_VIDTCON0_VBPDE(x)			(((x) & 0xff) << 24)
#define EXYNOS_VIDTCON0_VBPD(x)				(((x) & 0xff) << 16)
#define EXYNOS_VIDTCON0_VFPD(x)				(((x) & 0xff) << 8)
#define EXYNOS_VIDTCON0_VSPW(x)				(((x) & 0xff) << 0)

/* VIDTCON1 */
#define EXYNOS_VIDTCON1_VFPDE(x)			(((x) & 0xff) << 24)
#define EXYNOS_VIDTCON1_HBPD(x)				(((x) & 0xff) << 16)
#define EXYNOS_VIDTCON1_HFPD(x)				(((x) & 0xff) << 8)
#define EXYNOS_VIDTCON1_HSPW(x)				(((x) & 0xff) << 0)

/* VIDTCON2 */
#define EXYNOS_VIDTCON2_LINEVAL(x)			(((x) & 0x7ff) << 11)
#define EXYNOS_VIDTCON2_HOZVAL(x)			(((x) & 0x7ff) << 0)
#define EXYNOS_VIDTCON2_LINEVAL_E(x)			((((x) & 0x800) >> 11) << 23)
#define EXYNOS_VIDTCON2_HOZVAL_E(x)			((((x) & 0x800) >> 11) << 22)

/* Window 0~4 Control - WINCONx */
#define EXYNOS_WINCON_DATAPATH_DMA			(0 << 22)
#define EXYNOS_WINCON_DATAPATH_LOCAL			(1 << 22)
#define EXYNOS_WINCON_DATAPATH_MASK			(1 << 22)
#define EXYNOS_WINCON_BUFSEL_0				(0 << 20)
#define EXYNOS_WINCON_BUFSEL_1				(1 << 20)
#define EXYNOS_WINCON_BUFSEL_MASK			(1 << 20)
#define EXYNOS_WINCON_BUFSEL_SHIFT			(20)
#define EXYNOS_WINCON_BUFAUTO_DISABLE			(0 << 19)
#define EXYNOS_WINCON_BUFAUTO_ENABLE			(1 << 19)
#define EXYNOS_WINCON_BUFAUTO_MASK			(1 << 19)
#define EXYNOS_WINCON_BITSWP_DISABLE			(0 << 18)
#define EXYNOS_WINCON_BITSWP_ENABLE			(1 << 18)
#define EXYNOS_WINCON_BITSWP_SHIFT			(18)
#define EXYNOS_WINCON_BYTESWP_DISABLE			(0 << 17)
#define EXYNOS_WINCON_BYTESWP_ENABLE			(1 << 17)
#define EXYNOS_WINCON_BYTESWP_SHIFT			(17)
#define EXYNOS_WINCON_HAWSWP_DISABLE			(0 << 16)
#define EXYNOS_WINCON_HAWSWP_ENABLE			(1 << 16)
#define EXYNOS_WINCON_HAWSWP_SHIFT			(16)
#define EXYNOS_WINCON_WSWP_DISABLE			(0 << 15)
#define EXYNOS_WINCON_WSWP_ENABLE			(1 << 15)
#define EXYNOS_WINCON_WSWP_SHIFT			(15)
#define EXYNOS_WINCON_INRGB_RGB				(0 << 13)
#define EXYNOS_WINCON_INRGB_YUV				(1 << 13)
#define EXYNOS_WINCON_INRGB_MASK			(1 << 13)
#define EXYNOS_WINCON_BURSTLEN_16WORD			(0 << 9)
#define EXYNOS_WINCON_BURSTLEN_8WORD			(1 << 9)
#define EXYNOS_WINCON_BURSTLEN_4WORD			(2 << 9)
#define EXYNOS_WINCON_BURSTLEN_MASK			(3 << 9)
#define EXYNOS_WINCON_ALPHA_MULTI_DISABLE		(0 << 7)
#define EXYNOS_WINCON_ALPHA_MULTI_ENABLE		(1 << 7)
#define EXYNOS_WINCON_BLD_PLANE				(0 << 6)
#define EXYNOS_WINCON_BLD_PIXEL				(1 << 6)
#define EXYNOS_WINCON_BLD_MASK				(1 << 6)
#define EXYNOS_WINCON_BPPMODE_1BPP			(0 << 2)
#define EXYNOS_WINCON_BPPMODE_2BPP			(1 << 2)
#define EXYNOS_WINCON_BPPMODE_4BPP			(2 << 2)
#define EXYNOS_WINCON_BPPMODE_8BPP_PAL			(3 << 2)
#define EXYNOS_WINCON_BPPMODE_8BPP			(4 << 2)
#define EXYNOS_WINCON_BPPMODE_16BPP_565			(5 << 2)
#define EXYNOS_WINCON_BPPMODE_16BPP_A555		(6 << 2)
#define EXYNOS_WINCON_BPPMODE_18BPP_666			(8 << 2)
#define EXYNOS_WINCON_BPPMODE_18BPP_A665		(9 << 2)
#define EXYNOS_WINCON_BPPMODE_24BPP_888			(0xb << 2)
#define EXYNOS_WINCON_BPPMODE_24BPP_A887		(0xc << 2)
#define EXYNOS_WINCON_BPPMODE_32BPP			(0xd << 2)
#define EXYNOS_WINCON_BPPMODE_16BPP_A444		(0xe << 2)
#define EXYNOS_WINCON_BPPMODE_15BPP_555			(0xf << 2)
#define EXYNOS_WINCON_BPPMODE_MASK			(0xf << 2)
#define EXYNOS_WINCON_BPPMODE_SHIFT			(2)
#define EXYNOS_WINCON_ALPHA0_SEL			(0 << 1)
#define EXYNOS_WINCON_ALPHA1_SEL			(1 << 1)
#define EXYNOS_WINCON_ALPHA_SEL_MASK			(1 << 1)
#define EXYNOS_WINCON_ENWIN_DISABLE			(0 << 0)
#define EXYNOS_WINCON_ENWIN_ENABLE			(1 << 0)

/* WINCON1 special */
#define EXYNOS_WINCON1_VP_DISABLE			(0 << 24)
#define EXYNOS_WINCON1_VP_ENABLE			(1 << 24)
#define EXYNOS_WINCON1_LOCALSEL_FIMC1			(0 << 23)
#define EXYNOS_WINCON1_LOCALSEL_VP			(1 << 23)
#define EXYNOS_WINCON1_LOCALSEL_MASK			(1 << 23)

/* WINSHMAP */
#define EXYNOS_WINSHMAP_PROTECT(x)			(((x) & 0x1f) << 10)
#define EXYNOS_WINSHMAP_CH_ENABLE(x)			(1 << (x))
#define EXYNOS_WINSHMAP_CH_DISABLE(x)			(1 << (x))
#define EXYNOS_WINSHMAP_LOCAL_ENABLE(x)			(0x20 << (x))
#define EXYNOS_WINSHMAP_LOCAL_DISABLE(x)		(0x20 << (x))

/* VIDOSDxA, VIDOSDxB */
#define EXYNOS_VIDOSD_LEFT_X(x)				(((x) & 0x7ff) << 11)
#define EXYNOS_VIDOSD_TOP_Y(x)				(((x) & 0x7ff) << 0)
#define EXYNOS_VIDOSD_RIGHT_X(x)			(((x) & 0x7ff) << 11)
#define EXYNOS_VIDOSD_BOTTOM_Y(x)			(((x) & 0x7ff) << 0)
#define EXYNOS_VIDOSD_RIGHT_X_E(x)			(((x) & 0x1) << 23)
#define EXYNOS_VIDOSD_BOTTOM_Y_E(x)			(((x) & 0x1) << 22)

/* VIDOSD0C, VIDOSDxD */
#define EXYNOS_VIDOSD_SIZE(x)				(((x) & 0xffffff) << 0)

/* VIDOSDxC (1~4) */
#define EXYNOS_VIDOSD_ALPHA0_R(x)			(((x) & 0xf) << 20)
#define EXYNOS_VIDOSD_ALPHA0_G(x)			(((x) & 0xf) << 16)
#define EXYNOS_VIDOSD_ALPHA0_B(x)			(((x) & 0xf) << 12)
#define EXYNOS_VIDOSD_ALPHA1_R(x)			(((x) & 0xf) << 8)
#define EXYNOS_VIDOSD_ALPHA1_G(x)			(((x) & 0xf) << 4)
#define EXYNOS_VIDOSD_ALPHA1_B(x)			(((x) & 0xf) << 0)
#define EXYNOS_VIDOSD_ALPHA0_SHIFT			(12)
#define EXYNOS_VIDOSD_ALPHA1_SHIFT			(0)

/* Start Address */
#define EXYNOS_VIDADDR_START_VBANK(x)			(((x) & 0xff) << 24)
#define EXYNOS_VIDADDR_START_VBASEU(x)			(((x) & 0xffffff) << 0)

/* End Address */
#define EXYNOS_VIDADDR_END_VBASEL(x)			(((x) & 0xffffff) << 0)

/* Buffer Size */
#define EXYNOS_VIDADDR_OFFSIZE(x)			(((x) & 0x1fff) << 13)
#define EXYNOS_VIDADDR_PAGEWIDTH(x)			(((x) & 0x1fff) << 0)
#define EXYNOS_VIDADDR_OFFSIZE_E(x)			((((x) & 0x2000) >> 13) << 27)
#define EXYNOS_VIDADDR_PAGEWIDTH_E(x)			((((x) & 0x2000) >> 13) << 26)

/* WIN Color Map */
#define EXYNOS_WINMAP_COLOR(x)				((x) & 0xffffff)

/* VIDINTCON0 */
#define EXYNOS_VIDINTCON0_SYSMAINCON_DISABLE		(0 << 19)
#define EXYNOS_VIDINTCON0_SYSMAINCON_ENABLE		(1 << 19)
#define EXYNOS_VIDINTCON0_SYSSUBCON_DISABLE		(0 << 18)
#define EXYNOS_VIDINTCON0_SYSSUBCON_ENABLE		(1 << 18)
#define EXYNOS_VIDINTCON0_SYSIFDONE_DISABLE		(0 << 17)
#define EXYNOS_VIDINTCON0_SYSIFDONE_ENABLE		(1 << 17)
#define EXYNOS_VIDINTCON0_FRAMESEL0_BACK		(0 << 15)
#define EXYNOS_VIDINTCON0_FRAMESEL0_VSYNC		(1 << 15)
#define EXYNOS_VIDINTCON0_FRAMESEL0_ACTIVE		(2 << 15)
#define EXYNOS_VIDINTCON0_FRAMESEL0_FRONT		(3 << 15)
#define EXYNOS_VIDINTCON0_FRAMESEL0_MASK		(3 << 15)
#define EXYNOS_VIDINTCON0_FRAMESEL1_NONE		(0 << 13)
#define EXYNOS_VIDINTCON0_FRAMESEL1_BACK		(1 << 13)
#define EXYNOS_VIDINTCON0_FRAMESEL1_VSYNC		(2 << 13)
#define EXYNOS_VIDINTCON0_FRAMESEL1_FRONT		(3 << 13)
#define EXYNOS_VIDINTCON0_INTFRMEN_DISABLE		(0 << 12)
#define EXYNOS_VIDINTCON0_INTFRMEN_ENABLE		(1 << 12)
#define EXYNOS_VIDINTCON0_FIFOSEL_WIN4			(1 << 11)
#define EXYNOS_VIDINTCON0_FIFOSEL_WIN3			(1 << 10)
#define EXYNOS_VIDINTCON0_FIFOSEL_WIN2			(1 << 9)
#define EXYNOS_VIDINTCON0_FIFOSEL_WIN1			(1 << 6)
#define EXYNOS_VIDINTCON0_FIFOSEL_WIN0			(1 << 5)
#define EXYNOS_VIDINTCON0_FIFOSEL_ALL			(0x73 << 5)
#define EXYNOS_VIDINTCON0_FIFOSEL_MASK			(0x73 << 5)
#define EXYNOS_VIDINTCON0_FIFOLEVEL_25			(0 << 2)
#define EXYNOS_VIDINTCON0_FIFOLEVEL_50			(1 << 2)
#define EXYNOS_VIDINTCON0_FIFOLEVEL_75			(2 << 2)
#define EXYNOS_VIDINTCON0_FIFOLEVEL_EMPTY		(3 << 2)
#define EXYNOS_VIDINTCON0_FIFOLEVEL_FULL		(4 << 2)
#define EXYNOS_VIDINTCON0_FIFOLEVEL_MASK		(7 << 2)
#define EXYNOS_VIDINTCON0_INTFIFO_DISABLE		(0 << 1)
#define EXYNOS_VIDINTCON0_INTFIFO_ENABLE		(1 << 1)
#define EXYNOS_VIDINTCON0_INT_DISABLE			(0 << 0)
#define EXYNOS_VIDINTCON0_INT_ENABLE			(1 << 0)
#define EXYNOS_VIDINTCON0_INT_MASK			(1 << 0)

/* VIDINTCON1 */
#define EXYNOS_VIDINTCON1_INTVPPEND			(1 << 5)
#define EXYNOS_VIDINTCON1_INTI80PEND			(1 << 2)
#define EXYNOS_VIDINTCON1_INTFRMPEND			(1 << 1)
#define EXYNOS_VIDINTCON1_INTFIFOPEND			(1 << 0)

/* WINMAP */
#define EXYNOS_WINMAP_ENABLE				(1 << 24)

/* WxKEYCON0 (1~4) */
#define EXYNOS_KEYCON0_KEYBLEN_DISABLE			(0 << 26)
#define EXYNOS_KEYCON0_KEYBLEN_ENABLE			(1 << 26)
#define EXYNOS_KEYCON0_KEY_DISABLE			(0 << 25)
#define EXYNOS_KEYCON0_KEY_ENABLE			(1 << 25)
#define EXYNOS_KEYCON0_DIRCON_MATCH_FG			(0 << 24)
#define EXYNOS_KEYCON0_DIRCON_MATCH_BG			(1 << 24)
#define EXYNOS_KEYCON0_COMPKEY(x)			(((x) & 0xffffff) << 0)

/* WxKEYCON1 (1~4) */
#define EXYNOS_KEYCON1_COLVAL(x)			(((x) & 0xffffff) << 0)

/* DUALRGB */
#define EXYNOS_DUALRGB_BYPASS_SINGLE			(0x00 << 0)
#define EXYNOS_DUALRGB_BYPASS_DUAL			(0x01 << 0)
#define EXYNOS_DUALRGB_MIE_DUAL				(0x10 << 0)
#define EXYNOS_DUALRGB_MIE_SINGLE			(0x11 << 0)
#define EXYNOS_DUALRGB_LINESPLIT			(0x0 << 2)
#define EXYNOS_DUALRGB_FRAMESPLIT			(0x1 << 2)
#define EXYNOS_DUALRGB_SUB_CNT(x)			((x & 0xfff) << 4)
#define EXYNOS_DUALRGB_VDEN_EN_DISABLE			(0x0 << 16)
#define EXYNOS_DUALRGB_VDEN_EN_ENABLE			(0x1 << 16)
#define EXYNOS_DUALRGB_MAIN_CNT(x)			((x & 0xfff) << 18)

/* I80IFCONA0 and I80IFCONA1 */
#define EXYNOS_LCD_CS_SETUP(x)				(((x) & 0xf) << 16)
#define EXYNOS_LCD_WR_SETUP(x)				(((x) & 0xf) << 12)
#define EXYNOS_LCD_WR_ACT(x)				(((x) & 0xf) << 8)
#define EXYNOS_LCD_WR_HOLD(x)				(((x) & 0xf) << 4)
#define EXYNOS_RSPOL_LOW				(0 << 2)
#define EXYNOS_RSPOL_HIGH				(1 << 2)
#define EXYNOS_I80IFEN_DISABLE				(0 << 0)
#define EXYNOS_I80IFEN_ENABLE				(1 << 0)

/* TRIGCON */
#define EXYNOS_I80SOFT_TRIG_EN				(1 << 0)
#define EXYNOS_I80START_TRIG				(1 << 1)
#define EXYNOS_I80STATUS_TRIG_DONE			(1 << 2)

/* DP_MIE_CLKCON */
#define EXYNOS_DP_MIE_DISABLE				(0 << 0)
#define EXYNOS_DP_CLK_ENABLE				(1 << 1)
#define EXYNOS_MIE_CLK_ENABLE				(3 << 0)

#define DP_TIMEOUT_LOOP_COUNT		1000
#define MAX_CR_LOOP			5
#define MAX_EQ_LOOP			4

#define EXYNOS_DP_SUCCESS		0

enum {
	DP_DISABLE,
	DP_ENABLE,
};

struct edp_disp_info {
	char *name;
	unsigned int h_total;
	unsigned int h_res;
	unsigned int h_sync_width;
	unsigned int h_back_porch;
	unsigned int h_front_porch;
	unsigned int v_total;
	unsigned int v_res;
	unsigned int v_sync_width;
	unsigned int v_back_porch;
	unsigned int v_front_porch;
	unsigned int v_sync_rate;
};

struct edp_link_train_info {
	unsigned int lt_status;
	unsigned int ep_loop;
	unsigned int cr_loop[4];
};

struct edp_video_info {
	unsigned int master_mode;
	unsigned int bist_mode;
	unsigned int bist_pattern;
	unsigned int h_sync_polarity;
	unsigned int v_sync_polarity;
	unsigned int interlaced;
	unsigned int color_space;
	unsigned int dynamic_range;
	unsigned int ycbcr_coeff;
	unsigned int color_depth;
};

struct edp_device_info {
	struct edp_disp_info disp_info;
	struct edp_link_train_info lt_info;
	struct edp_video_info video_info;

	/* below info get from panel during training */
	u8 lane_bw;
	u8 lane_cnt;
	u8 dpcd_rev;
	/* support enhanced frame cap */
	u8 dpcd_efc;
	u8 *raw_edid;
};

enum analog_power_block {
	AUX_BLOCK,
	CH0_BLOCK,
	CH1_BLOCK,
	CH2_BLOCK,
	CH3_BLOCK,
	ANALOG_TOTAL,
	POWER_ALL
};

enum pll_status {
	PLL_UNLOCKED = 0,
	PLL_LOCKED
};

enum {
	COLOR_RGB,
	COLOR_YCBCR422,
	COLOR_YCBCR444
};

enum {
	VESA,
	CEA
};

enum {
	COLOR_YCBCR601,
	COLOR_YCBCR709
};

enum {
	COLOR_6,
	COLOR_8,
	COLOR_10,
	COLOR_12
};

enum {
	DP_LANE_BW_1_62 = 0x06,
	DP_LANE_BW_2_70 = 0x0a,
};

enum {
	DP_LANE_CNT_1 = 1,
	DP_LANE_CNT_2 = 2,
	DP_LANE_CNT_4 = 4,
};

enum {
	DP_DPCD_REV_10 = 0x10,
	DP_DPCD_REV_11 = 0x11,
};

enum {
	DP_LT_NONE,
	DP_LT_START,
	DP_LT_CR,
	DP_LT_ET,
	DP_LT_FINISHED,
	DP_LT_FAIL,
};

enum  {
	PRE_EMPHASIS_LEVEL_0,
	PRE_EMPHASIS_LEVEL_1,
	PRE_EMPHASIS_LEVEL_2,
	PRE_EMPHASIS_LEVEL_3,
};

enum {
	PRBS7,
	D10_2,
	TRAINING_PTN1,
	TRAINING_PTN2,
	DP_NONE
};

enum {
	VOLTAGE_LEVEL_0,
	VOLTAGE_LEVEL_1,
	VOLTAGE_LEVEL_2,
	VOLTAGE_LEVEL_3,
};

enum pattern_type {
	NO_PATTERN,
	COLOR_RAMP,
	BALCK_WHITE_V_LINES,
	COLOR_SQUARE,
	INVALID_PATTERN,
	COLORBAR_32,
	COLORBAR_64,
	WHITE_GRAY_BALCKBAR_32,
	WHITE_GRAY_BALCKBAR_64,
	MOBILE_WHITEBAR_32,
	MOBILE_WHITEBAR_64
};

enum {
	CALCULATED_M,
	REGISTER_M
};

enum {
	VIDEO_TIMING_FROM_CAPTURE,
	VIDEO_TIMING_FROM_REGISTER
};

struct exynos_dp_platform_data {
	struct edp_device_info *edp_dev_info;
};

int exynos_init_dp(struct edp_device_info *edp_info);

void exynos_set_dp_platform_data(struct exynos_dp_platform_data *pd);

void exynos_dp_disable_video_bist(void);
void exynos_dp_enable_video_mute(unsigned int enable);
void exynos_dp_reset(void);
void exynos_dp_enable_sw_func(unsigned int enable);
unsigned int exynos_dp_set_analog_power_down(unsigned int block, u32 enable);
unsigned int exynos_dp_get_pll_lock_status(void);
int exynos_dp_init_analog_func(void);
void exynos_dp_init_hpd(void);
void exynos_dp_init_aux(void);
void exynos_dp_config_interrupt(void);
unsigned int exynos_dp_get_plug_in_status(void);
unsigned int exynos_dp_detect_hpd(void);
unsigned int exynos_dp_start_aux_transaction(void);
unsigned int exynos_dp_write_byte_to_dpcd(u32 reg_addr,
				u8 data);
unsigned int exynos_dp_read_byte_from_dpcd(u32 reg_addr,
		u8 *data);
unsigned int exynos_dp_write_bytes_to_dpcd(u32 reg_addr,
		unsigned int count,
		u8 data[]);
u32 exynos_dp_read_bytes_from_dpcd( unsigned int reg_addr,
		unsigned int count,
		u8 data[]);
int exynos_dp_select_i2c_device( u32 device_addr,
		u32 reg_addr);
int exynos_dp_read_byte_from_i2c(u32 device_addr,
		u32 reg_addr, unsigned int *data);
int exynos_dp_read_bytes_from_i2c(u32 device_addr,
		u32 reg_addr, unsigned int count,
		u8 edid[]);
void exynos_dp_reset_macro(void);
void exynos_dp_set_link_bandwidth(u8 bwtype);
u8 exynos_dp_get_link_bandwidth(void);
void exynos_dp_set_lane_count(u8 count);
unsigned int exynos_dp_get_lane_count(void);
u8 exynos_dp_get_lanex_pre_emphasis(u8 lanecnt);
void exynos_dp_set_lane_pre_emphasis(unsigned int level,
		u8 lanecnt);
void exynos_dp_set_lanex_pre_emphasis(u8 request_val,
		u8 lanecnt);
void exynos_dp_set_training_pattern(unsigned int pattern);
void exynos_dp_enable_enhanced_mode(u8 enable);
void exynos_dp_enable_scrambling(unsigned int enable);
int exynos_dp_init_video(void);
void exynos_dp_config_video_slave_mode(struct edp_video_info *video_info);
void exynos_dp_set_video_color_format(struct edp_video_info *video_info);
int exynos_dp_config_video_bist(struct edp_device_info *edp_info);
unsigned int exynos_dp_is_slave_video_stream_clock_on(void);
void exynos_dp_set_video_cr_mn(unsigned int type, unsigned int m_value,
		unsigned int n_value);
void exynos_dp_set_video_timing_mode(unsigned int type);
void exynos_dp_enable_video_master(unsigned int enable);
void exynos_dp_start_video(void);
unsigned int exynos_dp_is_video_stream_on(void);
void exynos_dp_set_base_addr(void);
void dp_phy_control(unsigned int enable);

#endif
