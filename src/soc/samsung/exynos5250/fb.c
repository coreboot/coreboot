/* SPDX-License-Identifier: GPL-2.0-only */

/* LCD driver for Exynos */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/dp.h>
#include <soc/dp-core.h>
#include <soc/fimd.h>
#include <soc/i2c.h>
#include <soc/power.h>
#include <soc/sysreg.h>
#include <timer.h>

/*
 * Here is the rough outline of how we bring up the display:
 *  1. Upon power-on Sink generates a hot plug detection pulse thru HPD
 *  2. Source determines video mode by reading DPCD receiver capability field
 *     (DPCD 00000h to 0000Dh) including eDP CP capability register (DPCD
 *     0000Dh).
 *  3. Sink replies DPCD receiver capability field.
 *  4. Source starts EDID read thru I2C-over-AUX.
 *  5. Sink replies EDID thru I2C-over-AUX.
 *  6. Source determines link configuration, such as MAX_LINK_RATE and
 *     MAX_LANE_COUNT. Source also determines which type of eDP Authentication
 *     method to use and writes DPCD link configuration field (DPCD 00100h to
 *     0010Ah) including eDP configuration set (DPCD 0010Ah).
 *  7. Source starts link training. Sink does clock recovery and equalization.
 *  8. Source reads DPCD link status field (DPCD 00200h to 0020Bh).
 *  9. Sink replies DPCD link status field. If main link is not stable, Source
 *     repeats Step 7.
 * 10. Source sends MSA (Main Stream Attribute) data. Sink extracts video
 *     parameters and recovers stream clock.
 * 11. Source sends video data.
 */

/* To help debug any init errors here, define a list of possible errors */
enum {
	ERR_PLL_NOT_UNLOCKED = 2,
	ERR_VIDEO_CLOCK_BAD,
	ERR_VIDEO_STREAM_BAD,
	ERR_DPCD_READ_ERROR1,		/* 5 */

	ERR_DPCD_WRITE_ERROR1,
	ERR_DPCD_READ_ERROR2,
	ERR_DPCD_WRITE_ERROR2,
	ERR_INVALID_LANE,
	ERR_PLL_NOT_LOCKED,		/* 10 */

	ERR_PRE_EMPHASIS_LEVELS,
	ERR_LINK_RATE_ABNORMAL,
	ERR_MAX_LANE_COUNT_ABNORMAL,
	ERR_LINK_TRAINING_FAILURE,
	ERR_MISSING_DP_BASE,		/* 15 */

	ERR_NO_FDT_NODE,
};
/* ok, this is stupid, but we're going to leave the variables in here until we
 * know it works. One cleanup task at a time.
 */
enum stage_t {
	STAGE_START = 0,
	STAGE_LCD_VDD,
	STAGE_BRIDGE_SETUP,
	STAGE_BRIDGE_INIT,
	STAGE_BRIDGE_RESET,
	STAGE_HOTPLUG,
	STAGE_DP_CONTROLLER,
	STAGE_BACKLIGHT_VDD,
	STAGE_BACKLIGHT_PWM,
	STAGE_BACKLIGHT_EN,
	STAGE_DONE,
};

int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;

void *lcd_console_address;	/* Start of console buffer */

short console_col;
short console_row;

/* Bypass FIMD of DISP1_BLK */
static void fimd_bypass(void)
{
	setbits32(&exynos_sysreg->disp1blk_cfg, FIMDBYPASS_DISP1);
	exynos_sysreg->disp1blk_cfg &= ~FIMDBYPASS_DISP1;
}

/*
 * Initialize display controller.
 *
 * @param lcdbase	pointer to the base address of framebuffer.
 * @param pd		pointer to the main panel_data structure
 */
void fb_init(unsigned long int fb_size, void *lcdbase,
	     struct exynos5_fimd_panel *pd)
{
	unsigned int val;

	fb_size = ALIGN(fb_size, 4096);

	write32(&exynos_disp_ctrl->vidcon1, pd->ivclk | pd->fixvclk);
	val = ENVID_ON | ENVID_F_ON | (pd->clkval_f << CLKVAL_F_OFFSET);
	write32(&exynos_fimd->vidcon0, val);

	val = (pd->vsync << VSYNC_PULSE_WIDTH_OFFSET) |
		(pd->lower_margin << V_FRONT_PORCH_OFFSET) |
		(pd->upper_margin << V_BACK_PORCH_OFFSET);
	write32(&exynos_disp_ctrl->vidtcon0, val);

	val = (pd->hsync << HSYNC_PULSE_WIDTH_OFFSET) |
		(pd->right_margin << H_FRONT_PORCH_OFFSET) |
		(pd->left_margin << H_BACK_PORCH_OFFSET);
	write32(&exynos_disp_ctrl->vidtcon1, val);

	val = ((pd->xres - 1) << HOZVAL_OFFSET) |
		((pd->yres - 1) << LINEVAL_OFFSET);
	write32(&exynos_disp_ctrl->vidtcon2, val);

	write32(&exynos_fimd->vidw00add0b0, (unsigned int)lcdbase);
	write32(&exynos_fimd->vidw00add1b0, (unsigned int)lcdbase + fb_size);

	write32(&exynos_fimd->vidw00add2, pd->xres * 2);

	val = ((pd->xres - 1) << OSD_RIGHTBOTX_F_OFFSET);
	val |= ((pd->yres - 1) << OSD_RIGHTBOTY_F_OFFSET);
	write32(&exynos_fimd->vidosd0b, val);
	write32(&exynos_fimd->vidosd0c, pd->xres * pd->yres);

	setbits32(&exynos_fimd->shadowcon, CHANNEL0_EN);

	val = BPPMODE_F_RGB_16BIT_565 << BPPMODE_F_OFFSET;
	val |= ENWIN_F_ENABLE | HALF_WORD_SWAP_EN;
	write32(&exynos_fimd->wincon0, val);

	/* DPCLKCON_ENABLE */
	write32(&exynos_fimd->dpclkcon, 1 << 1);
}

/*
 * Configure DP in slave mode and wait for video stream.
 *
 * param dp		pointer to main s5p-dp structure
 * param video_info	pointer to main video_info structure.
 * return		status
 */
static int s5p_dp_config_video(struct s5p_dp_device *dp,
			       struct video_info *video_info)
{
	int timeout = 0;
	struct exynos5_dp *base = dp->base;
	struct stopwatch sw;
	s5p_dp_config_video_slave_mode(dp, video_info);

	s5p_dp_set_video_color_format(dp, video_info->color_depth,
				      video_info->color_space,
				      video_info->dynamic_range,
				      video_info->ycbcr_coeff);

	if (s5p_dp_get_pll_lock_status(dp) == PLL_UNLOCKED) {
		printk(BIOS_DEBUG, "PLL is not locked yet.\n");
		return -ERR_PLL_NOT_UNLOCKED;
	}

	stopwatch_init_msecs_expire(&sw, STREAM_ON_TIMEOUT);
	do {
		if (s5p_dp_is_slave_video_stream_clock_on(dp) == 0) {
			timeout++;
			break;
		}
	} while (!stopwatch_expired(&sw));

	if (!timeout) {
		printk(BIOS_ERR, "Video Clock Not ok after %lldus.\n",
				stopwatch_duration_usecs(&sw));
		return -ERR_VIDEO_CLOCK_BAD;
	}

	/* Set to use the register calculated M/N video */
	s5p_dp_set_video_cr_mn(dp, CALCULATED_M, 0, 0);

	clrbits32(&base->video_ctl_10, FORMAT_SEL);

	/* Disable video mute */
	clrbits32(&base->video_ctl_1, HDCP_VIDEO_MUTE);

	/* Configure video slave mode */
	s5p_dp_enable_video_master(dp);

	/* Enable video */
	setbits32(&base->video_ctl_1, VIDEO_EN);
	timeout = s5p_dp_is_video_stream_on(dp);

	if (timeout) {
		printk(BIOS_DEBUG, "Video Stream Not on\n");
		return -ERR_VIDEO_STREAM_BAD;
	}

	return 0;
}

/*
 * Set DP to enhanced mode. We use this for EVT1
 * param dp	pointer to main s5p-dp structure
 * return	status
 */
static int s5p_dp_enable_rx_to_enhanced_mode(struct s5p_dp_device *dp)
{
	u8 data;

	if (s5p_dp_read_byte_from_dpcd(dp, DPCD_ADDR_LANE_COUNT_SET, &data)) {
		printk(BIOS_DEBUG, "DPCD read error\n");
		return -ERR_DPCD_READ_ERROR1;
	}
	if (s5p_dp_write_byte_to_dpcd(dp, DPCD_ADDR_LANE_COUNT_SET,
				      DPCD_ENHANCED_FRAME_EN |
				      (data & DPCD_LANE_COUNT_SET_MASK))) {
		printk(BIOS_DEBUG, "DPCD write error\n");
		return -ERR_DPCD_WRITE_ERROR1;
	}

	return 0;
}

/*
 * Enable scrambles mode. We use this for EVT1
 * param dp	pointer to main s5p-dp structure
 * return	status
 */
static int s5p_dp_enable_scramble(struct s5p_dp_device *dp)
{
	u8 data;
	struct exynos5_dp *base = dp->base;

	clrbits32(&base->dp_training_ptn_set, SCRAMBLING_DISABLE);

	if (s5p_dp_read_byte_from_dpcd(dp, DPCD_ADDR_TRAINING_PATTERN_SET,
				       &data)) {
		printk(BIOS_DEBUG, "DPCD read error\n");
		return -ERR_DPCD_READ_ERROR2;
	}

	if (s5p_dp_write_byte_to_dpcd(dp, DPCD_ADDR_TRAINING_PATTERN_SET,
			      (u8)(data & ~DPCD_SCRAMBLING_DISABLED))) {
		printk(BIOS_DEBUG, "DPCD write error\n");
		return -ERR_DPCD_WRITE_ERROR2;
	}

	return 0;
}

/*
 * Reset DP and prepare DP for init training
 * param dp	pointer to main s5p-dp structure
 */
static int s5p_dp_init_dp(struct s5p_dp_device *dp)
{
	int ret, i;
	struct exynos5_dp *base = dp->base;

	for (i = 0; i < DP_INIT_TRIES; i++) {
		s5p_dp_reset(dp);

		/* SW defined function Normal operation */
		clrbits32(&base->func_en_1, SW_FUNC_EN_N);

		ret = s5p_dp_init_analog_func(dp);
		if (!ret)
			break;

		udelay(5000);
		printk(BIOS_DEBUG, "LCD retry init, attempt=%d ret=%d\n", i, ret);
	}
	if (i == DP_INIT_TRIES) {
		printk(BIOS_DEBUG, "LCD initialization failed, ret=%d\n", ret);
		return ret;
	}

	s5p_dp_init_aux(dp);

	return ret;
}

/*
 * Set pre-emphasis level
 * param dp		pointer to main s5p-dp structure
 * param pre_emphasis	pre-emphasis level
 * param lane		lane number(0 - 3)
 * return		status
 */
static int s5p_dp_set_lane_lane_pre_emphasis(struct s5p_dp_device *dp,
					     int pre_emphasis, int lane)
{
	u32 reg;
	struct exynos5_dp *base = dp->base;

	reg = pre_emphasis << PRE_EMPHASIS_SET_SHIFT;
	switch (lane) {
	case 0:
		write32(&base->ln0_link_trn_ctl, reg);
		break;
	case 1:
		write32(&base->ln1_link_trn_ctl, reg);
		break;

	case 2:
		write32(&base->ln2_link_trn_ctl, reg);
		break;

	case 3:
		write32(&base->ln3_link_trn_ctl, reg);
		break;
	default:
		printk(BIOS_DEBUG, "%s: Invalid lane %d\n", __func__, lane);
		return -ERR_INVALID_LANE;
	}
	return 0;
}

/*
 * Read supported bandwidth type
 * param dp		pointer to main s5p-dp structure
 * param bandwidth	pointer to variable holding bandwidth type
 */
static void s5p_dp_get_max_rx_bandwidth(struct s5p_dp_device *dp,
					u8 *bandwidth)
{
	u8 data;

	/*
	 * For DP rev.1.1, Maximum link rate of Main Link lanes
	 * 0x06 = 1.62 Gbps, 0x0a = 2.7 Gbps
	 */
	s5p_dp_read_byte_from_dpcd(dp, DPCD_ADDR_MAX_LINK_RATE, &data);
	*bandwidth = data;
}

/*
 * Reset DP and prepare DP for init training
 * param dp		pointer to main s5p-dp structure
 * param lane_count	pointer to variable holding no of lanes
 */
static void s5p_dp_get_max_rx_lane_count(struct s5p_dp_device *dp,
					 u8 *lane_count)
{
	u8 data;

	/*
	 * For DP rev.1.1, Maximum number of Main Link lanes
	 * 0x01 = 1 lane, 0x02 = 2 lanes, 0x04 = 4 lanes
	 */
	s5p_dp_read_byte_from_dpcd(dp, DPCD_ADDR_MAX_LANE_COUNT, &data);
	*lane_count = data & DPCD_MAX_LANE_COUNT_MASK;
}

/*
 * DP H/w Link Training. Set DPCD link rate and bandwidth.
 * param dp		pointer to main s5p-dp structure
 * param max_lane	No of lanes
 * param max_rate	bandwidth
 * return status
 */
static int s5p_dp_hw_link_training(struct s5p_dp_device *dp,
				   unsigned int max_lane,
				   unsigned int max_rate)
{
	int pll_is_locked = 0;
	u32 data;
	int lane;
	struct stopwatch sw;
	struct exynos5_dp *base = dp->base;

	/* Stop Video */
	clrbits32(&base->video_ctl_1, VIDEO_EN);

	stopwatch_init_msecs_expire(&sw, PLL_LOCK_TIMEOUT);

	while ((pll_is_locked = s5p_dp_get_pll_lock_status(dp)) == PLL_UNLOCKED) {
		if (stopwatch_expired(&sw)) {
			/* Ignore this error, and try to continue */
			printk(BIOS_ERR, "PLL is not locked yet.\n");
			break;
		}
	}
	printk(BIOS_SPEW, "PLL is %slocked\n",
			pll_is_locked == PLL_LOCKED ? "": "not ");
	/* Reset Macro */
	setbits32(&base->dp_phy_test, MACRO_RST);

	/* 10 us is the minimum reset time. */
	udelay(10);

	clrbits32(&base->dp_phy_test, MACRO_RST);

	/* Set TX pre-emphasis to minimum */
	for (lane = 0; lane < max_lane; lane++)
		if (s5p_dp_set_lane_lane_pre_emphasis(dp,
					      PRE_EMPHASIS_LEVEL_0, lane)) {
			printk(BIOS_DEBUG, "Unable to set pre emphasis level\n");
			return -ERR_PRE_EMPHASIS_LEVELS;
		}

	/* All DP analog module power up */
	write32(&base->dp_phy_pd, 0x00);

	/* Initialize by reading RX's DPCD */
	s5p_dp_get_max_rx_bandwidth(dp, &dp->link_train.link_rate);
	s5p_dp_get_max_rx_lane_count(dp, &dp->link_train.lane_count);

	printk(BIOS_SPEW, "%s: rate 0x%x, lane_count %d\n", __func__,
		dp->link_train.link_rate, dp->link_train.lane_count);

	if ((dp->link_train.link_rate != LINK_RATE_1_62GBPS) &&
	    (dp->link_train.link_rate != LINK_RATE_2_70GBPS)) {
		printk(BIOS_DEBUG, "Rx Max Link Rate is abnormal :%x !\n",
		      dp->link_train.link_rate);
		/* Not Retrying */
		return -ERR_LINK_RATE_ABNORMAL;
	}

	if (dp->link_train.lane_count == 0) {
		printk(BIOS_DEBUG, "Rx Max Lane count is abnormal :%x !\n",
		      dp->link_train.lane_count);
		/* Not retrying */
		return -ERR_MAX_LANE_COUNT_ABNORMAL;
	}

	/* Setup TX lane count & rate */
	if (dp->link_train.lane_count > max_lane)
		dp->link_train.lane_count = max_lane;
	if (dp->link_train.link_rate > max_rate)
		dp->link_train.link_rate = max_rate;

	/* Set link rate and count as you want to establish*/
	write32(&base->lane_count_set, dp->link_train.lane_count);
	write32(&base->link_bw_set, dp->link_train.link_rate);

	/* Set sink to D0 (Sink Not Ready) mode. */
	s5p_dp_write_byte_to_dpcd(dp, DPCD_ADDR_SINK_POWER_STATE,
				  DPCD_SET_POWER_STATE_D0);

	/* Start HW link training */
	write32(&base->dp_hw_link_training, HW_TRAINING_EN);

	/* Wait until HW link training done */
	s5p_dp_wait_hw_link_training_done(dp);

	/* Get hardware link training status */
	data = read32(&base->dp_hw_link_training);
	printk(BIOS_SPEW, "hardware link training status: 0x%08x\n", data);
	if (data != 0) {
		printk(BIOS_ERR, " H/W link training failure: 0x%x\n", data);
		return -ERR_LINK_TRAINING_FAILURE;
	}

	/* Get Link Bandwidth */
	data = read32(&base->link_bw_set);

	dp->link_train.link_rate = data;

	data = read32(&base->lane_count_set);
	dp->link_train.lane_count = data;
	printk(BIOS_SPEW, "Done training: Link bandwidth: 0x%x, lane_count: %d\n",
		dp->link_train.link_rate, data);

	return 0;
}

/*
 * Initialize DP display
 */
int dp_controller_init(struct s5p_dp_device *dp_device)
{
	int ret;
	struct s5p_dp_device *dp = dp_device;
	struct exynos5_dp *base;

	clock_init_dp_clock();

	power_enable_dp_phy();
	ret = s5p_dp_init_dp(dp);
	if (ret) {
		printk(BIOS_ERR, "%s: Could not initialize dp\n", __func__);
		return ret;
	}

	ret = s5p_dp_hw_link_training(dp, dp->video_info->lane_count,
				      dp->video_info->link_rate);
	if (ret) {
		printk(BIOS_ERR, "unable to do link train\n");
		return ret;
	}
	/* Minimum delay after H/w Link training */
	udelay(1000);

	ret = s5p_dp_enable_scramble(dp);
	if (ret) {
		printk(BIOS_ERR, "unable to set scramble mode\n");
		return ret;
	}

	ret = s5p_dp_enable_rx_to_enhanced_mode(dp);
	if (ret) {
		printk(BIOS_ERR, "unable to set enhanced mode\n");
		return ret;
	}

	base = dp->base;
	/* Enable enhanced mode */
	setbits32(&base->sys_ctl_4, ENHANCED);

	write32(&base->lane_count_set, dp->link_train.lane_count);
	write32(&base->link_bw_set, dp->link_train.link_rate);

	s5p_dp_init_video(dp);
	ret = s5p_dp_config_video(dp, dp->video_info);
	if (ret) {
		printk(BIOS_ERR, "unable to config video\n");
		return ret;
	}

	return 0;
}

/**
 * Init the LCD controller
 *
 * @param panel_data
 * @param lcdbase	Base address of LCD frame buffer
 * @return 0 if ok, -ve error code on error
 */
int lcd_ctrl_init(unsigned long int fb_size,
		  struct exynos5_fimd_panel *panel_data, void *lcdbase)
{
	int ret = 0;

	fimd_bypass();
	fb_init(fb_size, lcdbase, panel_data);
	return ret;
}
