/*
 * (C) Copyright 2012 Samsung Electronics
 * Register map for Exynos5 DP
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __EXYNOS5_DP_H__
#define __EXYNOS5_DP_H__

/* DSIM register map */
struct exynos5_dp {
	unsigned char	res1[0x10];
	unsigned int	dp_tx_version;
	unsigned int	dp_tx_sw_reset;
	unsigned int	func_en_1;
	unsigned int	func_en_2;
	unsigned int	video_ctl_1;
	unsigned int	video_ctl_2;
	unsigned int	video_ctl_3;
	unsigned int	video_ctl_4;
	unsigned int	clr_blue_cb;
	unsigned int	clr_green_y;
	unsigned int	clr_red_cr;
	unsigned int	video_ctl_8;
	unsigned char	res2[0x4];
	unsigned int	video_ctl_10;
	unsigned int	total_line_l;
	unsigned int	total_line_h;
	unsigned int	active_line_l;
	unsigned int	active_line_h;
	unsigned int	v_f_porch;
	unsigned int	vsync;
	unsigned int	v_b_porch;
	unsigned int	total_pixel_l;
	unsigned int	total_pixel_h;
	unsigned int	active_pixel_l;
	unsigned int	active_pixel_h;
	unsigned int	h_f_porch_l;
	unsigned int	h_f_porch_h;
	unsigned int	hsync_l;
	unsigned int	hysnc_h;
	unsigned int	h_b_porch_l;
	unsigned int	h_b_porch_h;
	unsigned int	vid_status;
	unsigned int	total_line_sta_l;
	unsigned int	total_line_sta_h;
	unsigned int	active_line_sta_l;
	unsigned int	active_line_sta_h;
	unsigned int	v_f_porch_sta;
	unsigned int	vsync_sta;
	unsigned int	v_b_porch_sta;
	unsigned int	total_pixel_sta_l;
	unsigned int	total_pixel_sta_h;
	unsigned int	active_pixel_sta_l;
	unsigned int	active_pixel_sta_h;
	unsigned int	h_f_porch_sta_l;
	unsigned int	h_f_porch_sta_h;
	unsigned int	hsync_sta_l;
	unsigned int	hsync_sta_h;
	unsigned int	h_b_porch_sta_l;
	unsigned int	h_b_porch__sta_h;
	unsigned char	res3[0x288];
	unsigned int	lane_map;
	unsigned char	res4[0x10];
	unsigned int	analog_ctl_1;
	unsigned int	analog_ctl_2;
	unsigned int	analog_ctl_3;
	unsigned int	pll_filter_ctl_1;
	unsigned int	tx_amp_tuning_ctl;
	unsigned char	res5[0xc];
	unsigned int	aux_hw_retry_ctl;
	unsigned char	res6[0x2c];
	unsigned int	int_state;
	unsigned int	common_int_sta_1;
	unsigned int	common_int_sta_2;
	unsigned int	common_int_sta_3;
	unsigned int	common_int_sta_4;
	unsigned char	res7[0x8];
	unsigned int	dp_int_sta;
	unsigned int	common_int_mask_1;
	unsigned int	common_int_mask_2;
	unsigned int	common_int_mask_3;
	unsigned int	common_int_mask_4;
	unsigned char	res8[0x08];
	unsigned int	int_sta_mask;
	unsigned int	int_ctl;
	unsigned char	res9[0x200];
	unsigned int	sys_ctl_1;
	unsigned int	sys_ctl_2;
	unsigned int	sys_ctl_3;
	unsigned int	sys_ctl_4;
	unsigned int	dp_vid_ctl;
	unsigned char	res10[0x2c];
	unsigned int	pkt_send_ctl;
	unsigned char	res11[0x4];
	unsigned int	dp_hdcp_ctl;
	unsigned char	res12[0x34];
	unsigned int	link_bw_set;
	unsigned int	lane_count_set;
	unsigned int	dp_training_ptn_set;
	unsigned int	ln0_link_trn_ctl;
	unsigned int	ln1_link_trn_ctl;
	unsigned int	ln2_link_trn_ctl;
	unsigned int	ln3_link_trn_ctl;
	unsigned int	dp_dn_spread;
	unsigned int	dp_hw_link_training;
	unsigned char	res13[0x1c];
	unsigned int	dp_debug_ctl;
	unsigned int	dp_hpd_deglitch_l;
	unsigned int	dp_hpd_deglitch_h;
	unsigned char	res14[0x14];
	unsigned int	dp_link_debug_ctl;
	unsigned char	res15[0x1c];
	unsigned int	m_vid_0;
	unsigned int	m_vid_1;
	unsigned int	m_vid_2;
	unsigned int	n_vid_0;
	unsigned int	n_vid_1;
	unsigned int	n_vid_2;
	unsigned int	m_vid_mon;
	unsigned int	dp_pll_ctl;
	unsigned int	dp_phy_pd;
	unsigned int	dp_phy_test;
	unsigned char	res16[0x8];
	unsigned int	dp_video_fifo_thrd;
	unsigned char	res17[0x8];
	unsigned int	dp_audio_margin;
	unsigned int	dp_dn_spread_ctl_1;
	unsigned int	dp_dn_spread_ctl_2;
	unsigned char	res18[0x18];
	unsigned int	dp_m_cal_ctl;
	unsigned int	m_vid_gen_filter_th;
	unsigned char	res19[0x14];
	unsigned int	m_aud_gen_filter_th;
	unsigned int	aux_ch_sta;
	unsigned int	aux_err_num;
	unsigned int	aux_ch_defer_dtl;
	unsigned int	aux_rx_comm;
	unsigned int	buf_data_ctl;
	unsigned int	aux_ch_ctl_1;
	unsigned int	aux_addr_7_0;
	unsigned int	aux_addr_15_8;
	unsigned int	aux_addr_19_16;
	unsigned int	aux_ch_ctl_2;
	unsigned char   res20[0x18];
	unsigned int	buf_data_0;
	unsigned char	res21[0x3c];
	unsigned int	soc_general_ctl;
};
/* DP_TX_SW_RESET */
#define RESET_DP_TX				(1 << 0)

/* DP_FUNC_EN_1 */
#define MASTER_VID_FUNC_EN_N			(1 << 7)
#define SLAVE_VID_FUNC_EN_N			(1 << 5)
#define AUD_FIFO_FUNC_EN_N			(1 << 4)
#define AUD_FUNC_EN_N				(1 << 3)
#define HDCP_FUNC_EN_N				(1 << 2)
#define CRC_FUNC_EN_N				(1 << 1)
#define SW_FUNC_EN_N				(1 << 0)

/* DP_FUNC_EN_2 */
#define SSC_FUNC_EN_N				(1 << 7)
#define AUX_FUNC_EN_N				(1 << 2)
#define SERDES_FIFO_FUNC_EN_N			(1 << 1)
#define LS_CLK_DOMAIN_FUNC_EN_N			(1 << 0)

/* DP_VIDEO_CTL_1 */
#define VIDEO_EN				(1 << 7)
#define HDCP_VIDEO_MUTE				(1 << 6)

/* DP_VIDEO_CTL_1 */
#define IN_D_RANGE_MASK				(1 << 7)
#define IN_D_RANGE_SHIFT			(7)
#define IN_D_RANGE_CEA				(1 << 7)
#define IN_D_RANGE_VESA				(0 << 7)
#define IN_BPC_MASK				(7 << 4)
#define IN_BPC_SHIFT				(4)
#define IN_BPC_12_BITS				(3 << 4)
#define IN_BPC_10_BITS				(2 << 4)
#define IN_BPC_8_BITS				(1 << 4)
#define IN_BPC_6_BITS				(0 << 4)
#define IN_COLOR_F_MASK				(3 << 0)
#define IN_COLOR_F_SHIFT			(0)
#define IN_COLOR_F_YCBCR444			(2 << 0)
#define IN_COLOR_F_YCBCR422			(1 << 0)
#define IN_COLOR_F_RGB				(0 << 0)

/* DP_VIDEO_CTL_3 */
#define IN_YC_COEFFI_MASK			(1 << 7)
#define IN_YC_COEFFI_SHIFT			(7)
#define IN_YC_COEFFI_ITU709			(1 << 7)
#define IN_YC_COEFFI_ITU601			(0 << 7)
#define VID_CHK_UPDATE_TYPE_MASK		(1 << 4)
#define VID_CHK_UPDATE_TYPE_SHIFT		(4)
#define VID_CHK_UPDATE_TYPE_1			(1 << 4)
#define VID_CHK_UPDATE_TYPE_0			(0 << 4)

/* DP_VIDEO_CTL_10 */
#define FORMAT_SEL				(1 << 4)
#define INTERACE_SCAN_CFG			(1 << 2)
#define VSYNC_POLARITY_CFG			(1 << 1)
#define HSYNC_POLARITY_CFG			(1 << 0)

/* DP_LANE_MAP */
#define LANE3_MAP_LOGIC_LANE_0			(0 << 6)
#define LANE3_MAP_LOGIC_LANE_1			(1 << 6)
#define LANE3_MAP_LOGIC_LANE_2			(2 << 6)
#define LANE3_MAP_LOGIC_LANE_3			(3 << 6)
#define LANE2_MAP_LOGIC_LANE_0			(0 << 4)
#define LANE2_MAP_LOGIC_LANE_1			(1 << 4)
#define LANE2_MAP_LOGIC_LANE_2			(2 << 4)
#define LANE2_MAP_LOGIC_LANE_3			(3 << 4)
#define LANE1_MAP_LOGIC_LANE_0			(0 << 2)
#define LANE1_MAP_LOGIC_LANE_1			(1 << 2)
#define LANE1_MAP_LOGIC_LANE_2			(2 << 2)
#define LANE1_MAP_LOGIC_LANE_3			(3 << 2)
#define LANE0_MAP_LOGIC_LANE_0			(0 << 0)
#define LANE0_MAP_LOGIC_LANE_1			(1 << 0)
#define LANE0_MAP_LOGIC_LANE_2			(2 << 0)
#define LANE0_MAP_LOGIC_LANE_3			(3 << 0)

/* DP_AUX_HW_RETRY_CTL */
#define AUX_BIT_PERIOD_SHIFT			8
#define AUX_BIT_PERIOD_MASK			7

#define AUX_HW_RETRY_INTERVAL_SHIFT		3
#define AUX_HW_RETRY_INTERVAL_600_US		0
#define AUX_HW_RETRY_INTERVAL_800_US		1
#define AUX_HW_RETRY_INTERVAL_1000_US		2
#define AUX_HW_RETRY_INTERVAL_1800_US		3
#define AUX_HW_RETRY_COUNT_SHIFT		0
#define AUX_HW_RETRY_COUNT_MASK			7

/* DP_COMMON_INT_STA_1 */
#define VSYNC_DET				(1 << 7)
#define PLL_LOCK_CHG				(1 << 6)
#define SPDIF_ERR				(1 << 5)
#define SPDIF_UNSTBL				(1 << 4)
#define VID_FORMAT_CHG				(1 << 3)
#define AUD_CLK_CHG				(1 << 2)
#define VID_CLK_CHG				(1 << 1)
#define SW_INT					(1 << 0)

/* DP_COMMON_INT_STA_2 */
#define ENC_EN_CHG				(1 << 6)
#define HW_BKSV_RDY				(1 << 3)
#define HW_SHA_DONE				(1 << 2)
#define HW_AUTH_STATE_CHG			(1 << 1)
#define HW_AUTH_DONE				(1 << 0)

/* DP_COMMON_INT_STA_3 */
#define AFIFO_UNDER				(1 << 7)
#define AFIFO_OVER				(1 << 6)
#define R0_CHK_FLAG				(1 << 5)

/* DP_COMMON_INT_STA_4 */
#define PSR_ACTIVE				(1 << 7)
#define PSR_INACTIVE				(1 << 6)
#define SPDIF_BI_PHASE_ERR			(1 << 5)
#define HOTPLUG_CHG				(1 << 2)
#define HPD_LOST				(1 << 1)
#define PLUG					(1 << 0)

/* DP_INT_STA */
#define INT_HPD					(1 << 6)
#define HW_TRAINING_FINISH			(1 << 5)
#define RPLY_RECEIV				(1 << 1)
#define AUX_ERR					(1 << 0)

/* DP_INT_CTL */
#define INT_POL0				(1 << 0)
#define INT_POL1				(1 << 1)
#define SOFT_INT_CTRL				(1 << 2)

/* DP_SYS_CTL_1 */
#define DET_STA					(1 << 2)
#define FORCE_DET				(1 << 1)
#define DET_CTRL				(1 << 0)

/* DP_SYS_CTL_2 */
#define CHA_CRI_SHIFT				4
#define CHA_CRI_MASK				0xf
#define CHA_STA					(1 << 2)
#define FORCE_CHA				(1 << 1)
#define CHA_CTRL				(1 << 0)

/* DP_SYS_CTL_3 */
#define HPD_STATUS				(1 << 6)
#define F_HPD					(1 << 5)
#define HPD_CTRL				(1 << 4)
#define HDCP_RDY				(1 << 3)
#define STRM_VALID				(1 << 2)
#define F_VALID					(1 << 1)
#define VALID_CTRL				(1 << 0)

/* DP_SYS_CTL_4 */
#define FIX_M_AUD				(1 << 4)
#define ENHANCED				(1 << 3)
#define FIX_M_VID				(1 << 2)
#define M_VID_UPDATE_CTRL			(3 << 0)

/* DP_TRAINING_PTN_SET */
#define SCRAMBLER_TYPE				(1 << 9)
#define HW_LINK_TRAINING_PATTERN		(1 << 8)
#define SCRAMBLING_DISABLE			(1 << 5)
#define SCRAMBLING_ENABLE			(0 << 5)
#define LINK_QUAL_PATTERN_SET_MASK		(3 << 2)
#define LINK_QUAL_PATTERN_SET_PRBS7		(3 << 2)
#define LINK_QUAL_PATTERN_SET_D10_2		(1 << 2)
#define LINK_QUAL_PATTERN_SET_DISABLE		(0 << 2)
#define SW_TRAINING_PATTERN_SET_MASK		(3 << 0)
#define SW_TRAINING_PATTERN_SET_PTN2		(2 << 0)
#define SW_TRAINING_PATTERN_SET_PTN1		(1 << 0)
#define SW_TRAINING_PATTERN_SET_NORMAL		(0 << 0)

/* DP_LN0_LINK_TRAINING_CTL */
#define PRE_EMPHASIS_SET_SHIFT			(3)

/* DP_DEBUG_CTL */
#define PLL_LOCK				(1 << 4)
#define F_PLL_LOCK				(1 << 3)
#define PLL_LOCK_CTRL				(1 << 2)
#define PN_INV					(1 << 0)

/* DP_M_VID */
#define M_VID_0_VALUE_SHIFT			0
#define M_VID_1_VALUE_SHIFT			8
#define M_VID_2_VALUE_SHIFT			16

/* DP_M_VID */
#define N_VID_0_VALUE_SHIFT			0
#define N_VID_1_VALUE_SHIFT			8
#define N_VID_2_VALUE_SHIFT			16

/* DP_PLL_CTL */
#define DP_PLL_PD				(1 << 7)
#define DP_PLL_RESET				(1 << 6)
#define DP_PLL_LOOP_BIT_DEFAULT			(1 << 4)
#define DP_PLL_REF_BIT_1_1250V			(5 << 0)
#define DP_PLL_REF_BIT_1_2500V			(7 << 0)

/* DP_PHY_PD */
#define DP_PHY_PD				(1 << 5)
#define AUX_PD					(1 << 4)
#define CH3_PD					(1 << 3)
#define CH2_PD					(1 << 2)
#define CH1_PD					(1 << 1)
#define CH0_PD					(1 << 0)

/* DP_PHY_TEST */
#define MACRO_RST				(1 << 5)
#define CH1_TEST				(1 << 1)
#define CH0_TEST				(1 << 0)

/* DP_AUX_CH_STA */
#define AUX_BUSY				(1 << 4)
#define AUX_STATUS_MASK				(0xf << 0)

/* DP_AUX_CH_DEFER_CTL */
#define DEFER_CTRL_EN				(1 << 7)
#define DEFER_COUNT_SHIFT			0
#define DEFER_COUNT_MASK			0x7f

/* DP_AUX_RX_COMM */
#define AUX_RX_COMM_I2C_DEFER			(2 << 2)
#define AUX_RX_COMM_AUX_DEFER			(2 << 0)

/* DP_BUFFER_DATA_CTL */
#define BUF_CLR					(1 << 7)

/* Maximum number of tries for Aux Transaction */
#define MAX_AUX_RETRY_COUNT			10

/* DP_AUX_CH_CTL_1 */
#define AUX_LENGTH_SHIFT			4
#define AUX_LENGTH_MASK				0xf

#define AUX_TX_COMM_MASK			(0xf << 0)
#define AUX_TX_COMM_DP_TRANSACTION		(1 << 3)
#define AUX_TX_COMM_I2C_TRANSACTION		(0 << 3)
#define AUX_TX_COMM_MOT				(1 << 2)
#define AUX_TX_COMM_WRITE			(0 << 0)
#define AUX_TX_COMM_READ			(1 << 0)

/* DP_AUX_ADDR_7_0 */
#define AUX_ADDR_7_0_SHIFT			0
#define AUX_ADDR_7_0_MASK			0xff

/* DP_AUX_ADDR_15_8 */
#define AUX_ADDR_15_8_SHIFT			8
#define AUX_ADDR_15_8_MASK			0xff

/* DP_AUX_ADDR_19_16 */
#define AUX_ADDR_19_16_SHIFT			16
#define AUX_ADDR_19_16_MASK			0x0f

/* DP_AUX_CH_CTL_2 */
#define ADDR_ONLY				(1 << 1)
#define AUX_EN					(1 << 0)

/* DP_SOC_GENERAL_CTL */
#define AUDIO_MODE_SPDIF_MODE			(1 << 8)
#define AUDIO_MODE_MASTER_MODE			(0 << 8)
#define MASTER_VIDEO_INTERLACE_EN		(1 << 4)
#define VIDEO_MASTER_CLK_SEL			(1 << 2)
#define VIDEO_MASTER_MODE_EN			(1 << 1)
#define VIDEO_MODE_MASK				(1 << 0)
#define VIDEO_MODE_SLAVE_MODE			(1 << 0)
#define VIDEO_MODE_MASTER_MODE			(0 << 0)

#define HW_TRAINING_ERROR_CODE                 (7<<4)
#define HW_TRAINING_EN                         (1<<0)

/* I2C EDID Chip ID, Slave Address */
#define I2C_EDID_DEVICE_ADDR			0x50
#define I2C_E_EDID_DEVICE_ADDR			0x30

#define EDID_BLOCK_LENGTH			0x80
#define EDID_HEADER_PATTERN			0x00
#define EDID_EXTENSION_FLAG			0x7e
#define EDID_CHECKSUM				0x7f

/* Definition for DPCD Register */
#define DPCD_ADDR_DPCD_REV			0x0000
#define DPCD_ADDR_MAX_LINK_RATE			0x0001
#define DPCD_ADDR_MAX_LANE_COUNT		0x0002
#define DPCD_ADDR_LINK_BW_SET			0x0100
#define DPCD_ADDR_LANE_COUNT_SET		0x0101
#define DPCD_ADDR_TRAINING_PATTERN_SET		0x0102
#define DPCD_ADDR_TRAINING_LANE0_SET		0x0103
#define DPCD_ADDR_LANE0_1_STATUS		0x0202
#define DPCD_ADDR_LANE_ALIGN__STATUS_UPDATED	0x0204
#define DPCD_ADDR_ADJUST_REQUEST_LANE0_1	0x0206
#define DPCD_ADDR_ADJUST_REQUEST_LANE2_3	0x0207
#define DPCD_ADDR_TEST_REQUEST			0x0218
#define DPCD_ADDR_TEST_RESPONSE			0x0260
#define DPCD_ADDR_TEST_EDID_CHECKSUM		0x0261
#define DPCD_ADDR_SINK_POWER_STATE		0x0600

/* DPCD_ADDR_MAX_LANE_COUNT */
#define DPCD_MAX_LANE_COUNT_MASK		0x1f

/* DPCD_ADDR_LANE_COUNT_SET */
#define DPCD_ENHANCED_FRAME_EN			(1 << 7)
#define DPCD_LANE_COUNT_SET_MASK		0x1f

/* DPCD_ADDR_TRAINING_PATTERN_SET */
#define DPCD_SCRAMBLING_DISABLED		(1 << 5)
#define DPCD_SCRAMBLING_ENABLED			(0 << 5)
#define DPCD_TRAINING_PATTERN_2			(2 << 0)
#define DPCD_TRAINING_PATTERN_1			(1 << 0)
#define DPCD_TRAINING_PATTERN_DISABLED		(0 << 0)

/* DPCD_ADDR_LANE0_1_STATUS */
#define DPCD_LANE_SYMBOL_LOCKED			(1 << 2)
#define DPCD_LANE_CHANNEL_EQ_DONE		(1 << 1)
#define DPCD_LANE_CR_DONE			(1 << 0)
#define DPCD_CHANNEL_EQ_BITS			(DPCD_LANE_CR_DONE | \
						 DPCD_LANE_CHANNEL_EQ_DONE | \
						 DPCD_LANE_SYMBOL_LOCKED)

/* DPCD_ADDR_LANE_ALIGN__STATUS_UPDATED */
#define DPCD_LINK_STATUS_UPDATED		(1 << 7)
#define DPCD_DOWNSTREAM_PORT_STATUS_CHANGED	(1 << 6)
#define DPCD_INTERLANE_ALIGN_DONE		(1 << 0)

/* DPCD_ADDR_TEST_REQUEST */
#define DPCD_TEST_EDID_READ			(1 << 2)

/* DPCD_ADDR_TEST_RESPONSE */
#define DPCD_TEST_EDID_CHECKSUM_WRITE		(1 << 2)

/* DPCD_ADDR_SINK_POWER_STATE */
#define DPCD_SET_POWER_STATE_D0			(1 << 0)
#define DPCD_SET_POWER_STATE_D4			(2 << 0)

/* Allow DP Gating clock and set FIMD source to 267 Mhz for DP */
void clock_init_dp_clock(void);

/**
 * Perform the next stage of the LCD init if it is time to do so.
 *
 * LCD init can be time-consuming because of the number of delays we need
 * while waiting for the backlight power supply, etc. This function can
 * be called at various times during U-Boot operation to advance the
 * initialization of the LCD to the next stage if sufficient time has
 * passed since the last stage. It keeps track of what stage it is up to
 * and the time that it is permitted to move to the next stage.
 *
 * The final call should have can_block=1 to complete the init.
 *
 * @param blob		fdt blob containing LCD information
 * @param can_block	1 to wait until all init is complete, and then return
 *			0 to return immediately, potentially doing nothing if it
 *			is not yet time for the next init.
 */
int exynos_lcd_check_next_stage(const void *blob, int can_block);
#endif
