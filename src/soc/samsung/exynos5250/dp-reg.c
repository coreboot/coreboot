/* SPDX-License-Identifier: GPL-2.0-only */

/* Samsung DP (Display port) register interface driver. */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/dp.h>
#include <soc/dp-core.h>
#include <soc/fimd.h>
#include <soc/periph.h>
#include <timer.h>

void s5p_dp_reset(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	write32(&base->dp_tx_sw_reset, RESET_DP_TX);

	/* Stop Video */
	clrbits32(&base->video_ctl_1, VIDEO_EN);
	clrbits32(&base->video_ctl_1, HDCP_VIDEO_MUTE);

	reg = MASTER_VID_FUNC_EN_N | SLAVE_VID_FUNC_EN_N |
		AUD_FIFO_FUNC_EN_N | AUD_FUNC_EN_N |
		HDCP_FUNC_EN_N | SW_FUNC_EN_N;
	write32(&base->func_en_1, reg);

	reg = SSC_FUNC_EN_N | AUX_FUNC_EN_N |
		SERDES_FIFO_FUNC_EN_N |
		LS_CLK_DOMAIN_FUNC_EN_N;
	write32(&base->func_en_2, reg);

	udelay(20);

	reg = LANE3_MAP_LOGIC_LANE_3 | LANE2_MAP_LOGIC_LANE_2 |
		LANE1_MAP_LOGIC_LANE_1 | LANE0_MAP_LOGIC_LANE_0;

	write32(&base->lane_map, reg);

	write32(&base->sys_ctl_1, 0x0);
	write32(&base->sys_ctl_2, 0x40);
	write32(&base->sys_ctl_3, 0x0);
	write32(&base->sys_ctl_4, 0x0);

	write32(&base->pkt_send_ctl, 0x0);
	write32(&base->dp_hdcp_ctl, 0x0);

	write32(&base->dp_hpd_deglitch_l, 0x5e);
	write32(&base->dp_hpd_deglitch_h, 0x1a);

	write32(&base->dp_debug_ctl, 0x10);

	write32(&base->dp_phy_test, 0x0);

	write32(&base->dp_video_fifo_thrd, 0x0);
	write32(&base->dp_audio_margin, 0x20);

	write32(&base->m_vid_gen_filter_th, 0x4);
	write32(&base->m_aud_gen_filter_th, 0x2);

	write32(&base->soc_general_ctl, 0x00000101);

	/* Set Analog Parameters */
	write32(&base->analog_ctl_1, 0x10);
	write32(&base->analog_ctl_2, 0x0C);
	write32(&base->analog_ctl_3, 0x85);
	write32(&base->pll_filter_ctl_1, 0x66);
	write32(&base->tx_amp_tuning_ctl, 0x0);

	/* Set interrupt pin assertion polarity as high */
	write32(&base->int_ctl, INT_POL0 | INT_POL1);

	/* Clear pending registers */
	write32(&base->common_int_sta_1, 0xff);
	write32(&base->common_int_sta_2, 0x4f);
	write32(&base->common_int_sta_3, 0xe0);
	write32(&base->common_int_sta_4, 0xe7);
	write32(&base->dp_int_sta, 0x63);

	/* 0:mask,1: unmask */
	write32(&base->common_int_mask_1, 0x00);
	write32(&base->common_int_mask_2, 0x00);
	write32(&base->common_int_mask_3, 0x00);
	write32(&base->common_int_mask_4, 0x00);
	write32(&base->int_sta_mask, 0x00);
}

unsigned int s5p_dp_get_pll_lock_status(struct s5p_dp_device *dp)
{
	u32 reg;

	reg = read32(&dp->base->dp_debug_ctl);
	if (reg & PLL_LOCK)
		return PLL_LOCKED;
	else
		return PLL_UNLOCKED;
}

int s5p_dp_init_analog_func(struct s5p_dp_device *dp)
{
	u32 reg;
	struct stopwatch sw;
	struct exynos5_dp *base = dp->base;

	write32(&base->dp_phy_pd, 0x00);

	reg = PLL_LOCK_CHG;
	write32(&base->common_int_sta_1, reg);

	clrbits32(&base->dp_debug_ctl, (F_PLL_LOCK | PLL_LOCK_CTRL));

	/* Power up PLL */
	if (s5p_dp_get_pll_lock_status(dp) == PLL_UNLOCKED) {

		clrbits32(&base->dp_pll_ctl, DP_PLL_PD);

		stopwatch_init_msecs_expire(&sw, PLL_LOCK_TIMEOUT);

		while (s5p_dp_get_pll_lock_status(dp) == PLL_UNLOCKED) {
			if (stopwatch_expired(&sw)) {
				printk(BIOS_ERR, "%s: PLL is not locked\n",
						__func__);
				return -1;
			}
		}
	}

	/* Enable Serdes FIFO function and Link symbol clock domain module */
	clrbits32(&base->func_en_2, (SERDES_FIFO_FUNC_EN_N |
				LS_CLK_DOMAIN_FUNC_EN_N | AUX_FUNC_EN_N));
	return 0;
}

void s5p_dp_init_aux(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	/* Clear interrupts related to AUX channel */
	reg = RPLY_RECEIV | AUX_ERR;
	write32(&base->dp_int_sta, reg);

	/* Disable AUX channel module */
	setbits32(&base->func_en_2, AUX_FUNC_EN_N);

	/* Disable AUX transaction H/W retry */
	reg = (3 & AUX_BIT_PERIOD_MASK) << AUX_BIT_PERIOD_SHIFT;
	reg |= (0 & AUX_HW_RETRY_COUNT_MASK) << AUX_HW_RETRY_COUNT_SHIFT;
	reg |= (AUX_HW_RETRY_INTERVAL_600_US << AUX_HW_RETRY_INTERVAL_SHIFT);
	write32(&base->aux_hw_retry_ctl, reg);

	/* Receive AUX Channel DEFER commands equal to DEFFER_COUNT*64 */
	reg = DEFER_CTRL_EN;
	reg |= (1 & DEFER_COUNT_MASK) << DEFER_COUNT_SHIFT;
	write32(&base->aux_ch_defer_dtl, reg);

	/* Enable AUX channel module */
	clrbits32(&base->func_en_2, AUX_FUNC_EN_N);
}

int s5p_dp_start_aux_transaction(struct s5p_dp_device *dp)
{
	int reg;
	struct exynos5_dp *base = dp->base;

	/* Enable AUX CH operation */
	setbits32(&base->aux_ch_ctl_2, AUX_EN);

	/* Is AUX CH command reply received? */
	reg = read32(&base->dp_int_sta);
	while (!(reg & RPLY_RECEIV))
		reg = read32(&base->dp_int_sta);

	/* Clear interrupt source for AUX CH command reply */
	write32(&base->dp_int_sta, RPLY_RECEIV);

	/* Clear interrupt source for AUX CH access error */
	reg = read32(&base->dp_int_sta);
	if (reg & AUX_ERR) {
		printk(BIOS_ERR, "%s: AUX_ERR encountered, dp_int_sta: "
				"0x%02x\n", __func__, reg);
		write32(&base->dp_int_sta, AUX_ERR);
		return -1;
	}

	/* Check AUX CH error access status */
	reg = read32(&base->dp_int_sta);
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
		write32(&base->buf_data_ctl, BUF_CLR);

		/* Select DPCD device address */
		reg = reg_addr >> AUX_ADDR_7_0_SHIFT;
		reg &= AUX_ADDR_7_0_MASK;
		write32(&base->aux_addr_7_0, reg);
		reg = reg_addr >> AUX_ADDR_15_8_SHIFT;
		reg &= AUX_ADDR_15_8_MASK;
		write32(&base->aux_addr_15_8, reg);
		reg = reg_addr >> AUX_ADDR_19_16_SHIFT;
		reg &= AUX_ADDR_19_16_MASK;
		write32(&base->aux_addr_19_16, reg);

		/* Write data buffer */
		reg = (unsigned int)data;
		write32(&base->buf_data_0, reg);

		/*
		 * Set DisplayPort transaction and write 1 byte
		 * If bit 3 is 1, DisplayPort transaction.
		 * If Bit 3 is 0, I2C transaction.
		 */
		reg = AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_WRITE;
		write32(&base->aux_ch_ctl_1, reg);

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
		write32(&base->buf_data_ctl, BUF_CLR);

		/* Select DPCD device address */
		reg = reg_addr >> AUX_ADDR_7_0_SHIFT;
		reg &= AUX_ADDR_7_0_MASK;
		write32(&base->aux_addr_7_0, reg);
		reg = reg_addr >> AUX_ADDR_15_8_SHIFT;
		reg &= AUX_ADDR_15_8_MASK;
		write32(&base->aux_addr_15_8, reg);
		reg = reg_addr >> AUX_ADDR_19_16_SHIFT;
		reg &= AUX_ADDR_19_16_MASK;
		write32(&base->aux_addr_19_16, reg);

		/*
		 * Set DisplayPort transaction and read 1 byte
		 * If bit 3 is 1, DisplayPort transaction.
		 * If Bit 3 is 0, I2C transaction.
		 */
		reg = AUX_TX_COMM_DP_TRANSACTION | AUX_TX_COMM_READ;
		write32(&base->aux_ch_ctl_1, reg);

		/* Start AUX transaction */
		retval = s5p_dp_start_aux_transaction(dp);
		if (retval == 0)
			break;
		else
			printk(BIOS_DEBUG, "Aux Transaction fail!\n");
	}

	/* Read data buffer */
	if (!retval) {
		reg = read32(&base->buf_data_0);
		*data = (unsigned char)(reg & 0xff);
	}

	return retval;
}

void s5p_dp_init_video(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = VSYNC_DET | VID_FORMAT_CHG | VID_CLK_CHG;
	write32(&base->common_int_sta_1, reg);

	reg = 0x0;
	write32(&base->sys_ctl_1, reg);

	reg = (4 & CHA_CRI_MASK) << CHA_CRI_SHIFT;
	reg |= CHA_CTRL;
	write32(&base->sys_ctl_2, reg);

	reg = 0x0;
	write32(&base->sys_ctl_3, reg);
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
	write32(&base->video_ctl_2, reg);

	/* Set Input Color YCbCr Coefficients to ITU601 or ITU709 */
	reg = read32(&base->video_ctl_3);
	reg &= ~IN_YC_COEFFI_MASK;
	if (coeff)
		reg |= IN_YC_COEFFI_ITU709;
	else
		reg |= IN_YC_COEFFI_ITU601;
	write32(&base->video_ctl_3, reg);
}

int s5p_dp_is_slave_video_stream_clock_on(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = read32(&base->sys_ctl_1);
	write32(&base->sys_ctl_1, reg);

	reg = read32(&base->sys_ctl_1);

	if (!(reg & DET_STA))
		return -1;

	reg = read32(&base->sys_ctl_2);
	write32(&base->sys_ctl_2, reg);

	reg = read32(&base->sys_ctl_2);

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
		setbits32(&base->sys_ctl_4, FIX_M_VID);

		reg = m_value >> M_VID_0_VALUE_SHIFT;
		write32(&base->m_vid_0, reg);

		reg = (m_value >> M_VID_1_VALUE_SHIFT);
		write32(&base->m_vid_1, reg);

		reg = (m_value >> M_VID_2_VALUE_SHIFT);
		write32(&base->m_vid_2, reg);

		reg = n_value >> N_VID_0_VALUE_SHIFT;
		write32(&base->n_vid_0, reg);

		reg = (n_value >> N_VID_1_VALUE_SHIFT);
		write32(&base->n_vid_1, reg);

		reg = (n_value >> N_VID_2_VALUE_SHIFT);
		write32(&base->n_vid_2, reg);
	} else  {
		clrbits32(&base->sys_ctl_4, FIX_M_VID);

		write32(&base->n_vid_0, 0x00);
		write32(&base->n_vid_1, 0x80);
		write32(&base->n_vid_2, 0x00);
	}
}

void s5p_dp_enable_video_master(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = read32(&base->soc_general_ctl);
	reg &= ~VIDEO_MODE_MASK;
	reg |= VIDEO_MODE_SLAVE_MODE;
	write32(&base->soc_general_ctl, reg);
}

int s5p_dp_is_video_stream_on(struct s5p_dp_device *dp)
{
	u32 reg, i = 0;
	struct stopwatch sw;
	struct exynos5_dp *base = dp->base;

	/* Wait for 4 VSYNC_DET interrupts */
	stopwatch_init_msecs_expire(&sw, STREAM_ON_TIMEOUT);

	do {
		reg = read32(&base->common_int_sta_1);
		if (reg & VSYNC_DET) {
			i++;
			write32(&base->common_int_sta_1, reg | VSYNC_DET);
		}
		if (i == 4)
			break;
	} while (!stopwatch_expired(&sw));

	if (i != 4) {
		printk(BIOS_DEBUG, "%s timeout\n", __func__);
		return -1;
	}

	return 0;
}

void s5p_dp_config_video_slave_mode(struct s5p_dp_device *dp,
			struct video_info *video_info)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = read32(&base->func_en_1);
	reg &= ~(MASTER_VID_FUNC_EN_N|SLAVE_VID_FUNC_EN_N);
	reg |= MASTER_VID_FUNC_EN_N;
	write32(&base->func_en_1, reg);

	reg = read32(&base->video_ctl_10);
	reg &= ~INTERACE_SCAN_CFG;
	reg |= (video_info->interlaced << 2);
	write32(&base->video_ctl_10, reg);

	reg = read32(&base->video_ctl_10);
	reg &= ~VSYNC_POLARITY_CFG;
	reg |= (video_info->v_sync_polarity << 1);
	write32(&base->video_ctl_10, reg);

	reg = read32(&base->video_ctl_10);
	reg &= ~HSYNC_POLARITY_CFG;
	reg |= (video_info->h_sync_polarity << 0);
	write32(&base->video_ctl_10, reg);

	reg = AUDIO_MODE_SPDIF_MODE | VIDEO_MODE_SLAVE_MODE;
	write32(&base->soc_general_ctl, reg);
}

void s5p_dp_wait_hw_link_training_done(struct s5p_dp_device *dp)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = read32(&base->dp_hw_link_training);
	while (reg & HW_TRAINING_EN)
		reg = read32(&base->dp_hw_link_training);
}
