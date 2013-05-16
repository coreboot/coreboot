/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
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

/* Samsung DP (Display port) register interface driver. */

#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include "timer.h"
#include "clk.h"
#include "cpu.h"
#include "periph.h"
#include "dp.h"
#include "fimd.h"
#include "dp-core.h"

void s5p_dp_reset(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	writel(RESET_DP_TX, &base->dp_tx_sw_reset);

	/* Stop Video */
	clrbits_le32(&base->video_ctl_1, VIDEO_EN);
	clrbits_le32(&base->video_ctl_1, HDCP_VIDEO_MUTE);

	reg = MASTER_VID_FUNC_EN_N | SLAVE_VID_FUNC_EN_N |
		AUD_FIFO_FUNC_EN_N | AUD_FUNC_EN_N |
		HDCP_FUNC_EN_N | SW_FUNC_EN_N;
	writel(reg, &base->func_en_1);

	reg = SSC_FUNC_EN_N | AUX_FUNC_EN_N |
		SERDES_FIFO_FUNC_EN_N |
		LS_CLK_DOMAIN_FUNC_EN_N;
	writel(reg, &base->func_en_2);

	udelay(20);

	reg = LANE3_MAP_LOGIC_LANE_3 | LANE2_MAP_LOGIC_LANE_2 |
		LANE1_MAP_LOGIC_LANE_1 | LANE0_MAP_LOGIC_LANE_0;

	writel(reg, &base->lane_map);

	writel(0x0, &base->sys_ctl_1);
	writel(0x40, &base->sys_ctl_2);
	writel(0x0, &base->sys_ctl_3);
	writel(0x0, &base->sys_ctl_4);

	writel(0x0, &base->pkt_send_ctl);
	writel(0x0, &base->dp_hdcp_ctl);

	writel(0x5e, &base->dp_hpd_deglitch_l);
	writel(0x1a, &base->dp_hpd_deglitch_h);

	writel(0x10, &base->dp_debug_ctl);

	writel(0x0, &base->dp_phy_test);

	writel(0x0, &base->dp_video_fifo_thrd);
	writel(0x20, &base->dp_audio_margin);

	writel(0x4, &base->m_vid_gen_filter_th);
	writel(0x2, &base->m_aud_gen_filter_th);

	writel(0x00000101, &base->soc_general_ctl);

	/* Set Analog Parameters */
	writel(0x10, &base->analog_ctl_1);
	writel(0x0C, &base->analog_ctl_2);
	writel(0x85, &base->analog_ctl_3);
	writel(0x66, &base->pll_filter_ctl_1);
	writel(0x0, &base->tx_amp_tuning_ctl);

	/* Set interrupt pin assertion polarity as high */
	writel(INT_POL0 | INT_POL1, &base->int_ctl);

	/* Clear pending regisers */
	writel(0xff, &base->common_int_sta_1);
	writel(0x4f, &base->common_int_sta_2);
	writel(0xe0, &base->common_int_sta_3);
	writel(0xe7, &base->common_int_sta_4);
	writel(0x63, &base->dp_int_sta);

	/* 0:mask,1: unmask */
	writel(0x00, &base->common_int_mask_1);
	writel(0x00, &base->common_int_mask_2);
	writel(0x00, &base->common_int_mask_3);
	writel(0x00, &base->common_int_mask_4);
	writel(0x00, &base->int_sta_mask);
}

unsigned int s5p_dp_get_pll_lock_status(struct s5p_dp_device *dp)
{
	u32 reg;

	reg = readl(&dp->base->dp_debug_ctl);
	if (reg & PLL_LOCK)
		return PLL_LOCKED;
	else
		return PLL_UNLOCKED;
}

int s5p_dp_init_analog_func(struct s5p_dp_device *dp)
{
	u32 reg;
	u32 start;
	struct exynos5_dp *base = dp->base;

	writel(0x00, &base->dp_phy_pd);

	reg = PLL_LOCK_CHG;
	writel(reg, &base->common_int_sta_1);

	clrbits_le32(&base->dp_debug_ctl, (F_PLL_LOCK | PLL_LOCK_CTRL));

	/* Power up PLL */
	if (s5p_dp_get_pll_lock_status(dp) == PLL_UNLOCKED) {

		clrbits_le32(&base->dp_pll_ctl, DP_PLL_PD);

		start = get_timer(0);
		while (s5p_dp_get_pll_lock_status(dp) == PLL_UNLOCKED) {
			if (get_timer(start) > PLL_LOCK_TIMEOUT) {
				printk(BIOS_ERR, "%s: PLL is not locked\n",
						__func__);
				return -1;
			}
		}
	}

	/* Enable Serdes FIFO function and Link symbol clock domain module */
	clrbits_le32(&base->func_en_2, (SERDES_FIFO_FUNC_EN_N |
				LS_CLK_DOMAIN_FUNC_EN_N | AUX_FUNC_EN_N));
	return 0;
}

void s5p_dp_init_aux(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	/* Clear inerrupts related to AUX channel */
	reg = RPLY_RECEIV | AUX_ERR;
	writel(reg, &base->dp_int_sta);

	/* Disable AUX channel module */
	setbits_le32(&base->func_en_2, AUX_FUNC_EN_N);

	/* Disable AUX transaction H/W retry */
	reg = (3 & AUX_BIT_PERIOD_MASK) << AUX_BIT_PERIOD_SHIFT;
	reg |= (0 & AUX_HW_RETRY_COUNT_MASK) << AUX_HW_RETRY_COUNT_SHIFT;
	reg |= (AUX_HW_RETRY_INTERVAL_600_US << AUX_HW_RETRY_INTERVAL_SHIFT);
	writel(reg, &base->aux_hw_retry_ctl) ;

	/* Receive AUX Channel DEFER commands equal to DEFFER_COUNT*64 */
	reg = DEFER_CTRL_EN;
	reg |= (1 & DEFER_COUNT_MASK) << DEFER_COUNT_SHIFT;
	writel(reg, &base->aux_ch_defer_dtl);

	/* Enable AUX channel module */
	clrbits_le32(&base->func_en_2, AUX_FUNC_EN_N);
}

int s5p_dp_start_aux_transaction(struct s5p_dp_device *dp)
{
	int reg;
	struct exynos5_dp *base = dp->base;

	/* Enable AUX CH operation */
	setbits_le32(&base->aux_ch_ctl_2, AUX_EN);

	/* Is AUX CH command reply received? */
	reg = readl(&base->dp_int_sta);
	while (!(reg & RPLY_RECEIV))
		reg = readl(&base->dp_int_sta);

	/* Clear interrupt source for AUX CH command reply */
	writel(RPLY_RECEIV, &base->dp_int_sta);

	/* Clear interrupt source for AUX CH access error */
	reg = readl(&base->dp_int_sta);
	if (reg & AUX_ERR) {
		printk(BIOS_ERR, "%s: AUX_ERR encountered, dp_int_sta: "
				"0x%02x\n", __func__, reg);
		writel(AUX_ERR, &base->dp_int_sta);
		return -1;
	}

	/* Check AUX CH error access status */
	reg = readl(&base->dp_int_sta);
	if ((reg & AUX_STATUS_MASK) != 0) {
		printk(BIOS_ERR, "AUX CH error happens: %d\n\n",
			reg & AUX_STATUS_MASK);
		return -1;
	}

	return 0;
}

int s5p_dp_write_byte_to_dpcd(struct s5p_dp_device *dp,
				unsigned int reg_addr,
				unsigned char data)
{
	u32 reg;
	int i;
	int retval;
	struct exynos5_dp *base = dp->base;

	for (i = 0; i < MAX_AUX_RETRY_COUNT; i++) {
		/* Clear AUX CH data buffer */
		writel(BUF_CLR, &base->buf_data_ctl);

		/* Select DPCD device address */
		reg = reg_addr >> AUX_ADDR_7_0_SHIFT;
		reg &= AUX_ADDR_7_0_MASK;
		writel(reg, &base->aux_addr_7_0);
		reg = reg_addr >> AUX_ADDR_15_8_SHIFT;
		reg &= AUX_ADDR_15_8_MASK;
		writel(reg, &base->aux_addr_15_8);
		reg = reg_addr >> AUX_ADDR_19_16_SHIFT;
		reg &= AUX_ADDR_19_16_MASK;
		writel(reg, &base->aux_addr_19_16);

		/* Write data buffer */
		reg = (unsigned int)data;
		writel(reg, &base->buf_data_0);

		/*
		 * Set DisplayPort transaction and write 1 byte
		 * If bit 3 is 1, DisplayPort transaction.
		 * If Bit 3 is 0, I2C transaction.
		 */
		reg = AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_WRITE;
		writel(reg, &base->aux_ch_ctl_1);

		/* Start AUX transaction */
		retval = s5p_dp_start_aux_transaction(dp);
		if (retval == 0)
			break;
		else
			printk(BIOS_DEBUG, "Aux Transaction fail!\n");
	}

	return retval;
}

int s5p_dp_read_byte_from_dpcd(struct s5p_dp_device *dp,
				unsigned int reg_addr,
				unsigned char *data)
{
	u32 reg;
	int i;
	int retval;
	struct exynos5_dp *base = dp->base;

	for (i = 0; i < MAX_AUX_RETRY_COUNT; i++) {
		/* Clear AUX CH data buffer */
		writel(BUF_CLR, &base->buf_data_ctl);

		/* Select DPCD device address */
		reg = reg_addr >> AUX_ADDR_7_0_SHIFT;
		reg &= AUX_ADDR_7_0_MASK;
		writel(reg, &base->aux_addr_7_0);
		reg = reg_addr >> AUX_ADDR_15_8_SHIFT;
		reg &= AUX_ADDR_15_8_MASK;
		writel(reg, &base->aux_addr_15_8);
		reg = reg_addr >> AUX_ADDR_19_16_SHIFT;
		reg &= AUX_ADDR_19_16_MASK;
		writel(reg, &base->aux_addr_19_16);

		/*
		 * Set DisplayPort transaction and read 1 byte
		 * If bit 3 is 1, DisplayPort transaction.
		 * If Bit 3 is 0, I2C transaction.
		 */
		reg = AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_READ;
		writel(reg, &base->aux_ch_ctl_1);

		/* Start AUX transaction */
		retval = s5p_dp_start_aux_transaction(dp);
		if (retval == 0)
			break;
		else
			printk(BIOS_DEBUG, "Aux Transaction fail!\n");
	}

	/* Read data buffer */
	if (!retval) {
		reg = readl(&base->buf_data_0);
		*data = (unsigned char)(reg & 0xff);
	}

	return retval;
}

void s5p_dp_init_video(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = VSYNC_DET | VID_FORMAT_CHG | VID_CLK_CHG;
	writel(reg, &base->common_int_sta_1);

	reg = 0x0;
	writel(reg, &base->sys_ctl_1);

	reg = (4 & CHA_CRI_MASK) << CHA_CRI_SHIFT;
	reg |= CHA_CTRL;
	writel(reg, &base->sys_ctl_2);

	reg = 0x0;
	writel(reg, &base->sys_ctl_3);
}

void s5p_dp_set_video_color_format(struct s5p_dp_device *dp,
					unsigned int color_depth,
					unsigned int color_space,
					unsigned int dynamic_range,
					unsigned int coeff)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	/* Configure the input color depth, color space, dynamic range */
	reg = (dynamic_range << IN_D_RANGE_SHIFT) |
		(color_depth << IN_BPC_SHIFT) |
		(color_space << IN_COLOR_F_SHIFT);
	writel(reg, &base->video_ctl_2);

	/* Set Input Color YCbCr Coefficients to ITU601 or ITU709 */
	reg = readl(&base->video_ctl_3);
	reg &= ~IN_YC_COEFFI_MASK;
	if (coeff)
		reg |= IN_YC_COEFFI_ITU709;
	else
		reg |= IN_YC_COEFFI_ITU601;
	writel(reg, &base->video_ctl_3);
}

int s5p_dp_is_slave_video_stream_clock_on(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = readl(&base->sys_ctl_1);
	writel(reg, &base->sys_ctl_1);

	reg = readl(&base->sys_ctl_1);

	if (!(reg & DET_STA))
		return -1;

	reg = readl(&base->sys_ctl_2);
	writel(reg, &base->sys_ctl_2);

	reg = readl(&base->sys_ctl_2);

	if (reg & CHA_STA) {
		printk(BIOS_DEBUG, "Input stream clk is changing\n");
		return -1;
	}

	return 0;
}

void s5p_dp_set_video_cr_mn(struct s5p_dp_device *dp,
		enum clock_recovery_m_value_type type,
		unsigned int m_value,
		unsigned int n_value)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	if (type == REGISTER_M) {
		setbits_le32(&base->sys_ctl_4, FIX_M_VID);

		reg = m_value >> M_VID_0_VALUE_SHIFT;
		writel(reg, &base->m_vid_0);

		reg = (m_value >> M_VID_1_VALUE_SHIFT);
		writel(reg, &base->m_vid_1);

		reg = (m_value >> M_VID_2_VALUE_SHIFT);
		writel(reg, &base->m_vid_2);

		reg = n_value >> N_VID_0_VALUE_SHIFT;
		writel(reg, &base->n_vid_0);

		reg = (n_value >> N_VID_1_VALUE_SHIFT);
		writel(reg, &base->n_vid_1);

		reg = (n_value >> N_VID_2_VALUE_SHIFT);
		writel(reg, &base->n_vid_2);
	} else  {
		clrbits_le32(&base->sys_ctl_4, FIX_M_VID);

		writel(0x00, &base->n_vid_0);
		writel(0x80, &base->n_vid_1);
		writel(0x00, &base->n_vid_2);
	}
}

void s5p_dp_enable_video_master(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = readl(&base->soc_general_ctl);
	reg &= ~VIDEO_MODE_MASK;
	reg |= VIDEO_MODE_SLAVE_MODE;
	writel(reg, &base->soc_general_ctl);
}

int s5p_dp_is_video_stream_on(struct s5p_dp_device *dp)
{
	u32 reg, i = 0;
	u32 start;
	struct exynos5_dp *base = dp->base;

	/* Wait for 4 VSYNC_DET interrupts */
	start = get_timer(0);
	do {
		reg = readl(&base->common_int_sta_1);
		if (reg & VSYNC_DET) {
			i++;
			writel(reg | VSYNC_DET, &base->common_int_sta_1);
		}
		if (i == 4)
			break;
	} while (get_timer(start) <= STREAM_ON_TIMEOUT);

	if (i != 4) {
		printk(BIOS_DEBUG, "s5p_dp_is_video_stream_on timeout\n");
		return -1;
	}

	return 0;
}

void s5p_dp_config_video_slave_mode(struct s5p_dp_device *dp,
			struct video_info *video_info)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = readl(&base->func_en_1);
	reg &= ~(MASTER_VID_FUNC_EN_N|SLAVE_VID_FUNC_EN_N);
	reg |= MASTER_VID_FUNC_EN_N;
	writel(reg, &base->func_en_1);

	reg = readl(&base->video_ctl_10);
	reg &= ~INTERACE_SCAN_CFG;
	reg |= (video_info->interlaced << 2);
	writel(reg, &base->video_ctl_10);

	reg = readl(&base->video_ctl_10);
	reg &= ~VSYNC_POLARITY_CFG;
	reg |= (video_info->v_sync_polarity << 1);
	writel(reg, &base->video_ctl_10);

	reg = readl(&base->video_ctl_10);
	reg &= ~HSYNC_POLARITY_CFG;
	reg |= (video_info->h_sync_polarity << 0);
	writel(reg, &base->video_ctl_10);

	reg = AUDIO_MODE_SPDIF_MODE | VIDEO_MODE_SLAVE_MODE;
	writel(reg, &base->soc_general_ctl);
}

void s5p_dp_wait_hw_link_training_done(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = readl(&base->dp_hw_link_training);
	while (reg & HW_TRAINING_EN)
		reg = readl(&base->dp_hw_link_training);
}
