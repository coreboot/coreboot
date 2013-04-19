/*
 * LCD driver for Exynos
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2012 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <arch/io.h>
#include <stdlib.h>
#include <string.h>
#include <console/console.h>
#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/sysreg.h>
#include <drivers/maxim/max77686/max77686.h>

#include "device/i2c.h"
#include "cpu/samsung/exynos5-common/i2c.h"
#include "cpu/samsung/exynos5250/dsim.h"
#include "cpu/samsung/exynos5250/fimd.h"

#include "cpu/samsung/exynos5250/s5p-dp.h"
#include "s5p-dp-core.h"

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
/* ok, this is stupid, but we're going to leave the variables in here until we know it works.
 * one cleanup task at a time.
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


#ifdef CONFIG_EXYNOS_DISPLAYPORT
static struct s5p_dp_device dp_device;

#endif

/* Bypass FIMD of DISP1_BLK */
static void fimd_bypass(void)
{
	struct exynos5_sysreg *sysreg = samsung_get_base_sysreg();

	setbits_le32(&sysreg->disp1blk_cfg, FIMDBYPASS_DISP1);
	sysreg->disp1blk_cfg &= ~FIMDBYPASS_DISP1;
}

/* Calculate the size of Framebuffer from the resolution */
static u32 calc_fbsize(vidinfo_t *panel_info)
{
	/* They had PAGE_SIZE here instead of 4096.
	 * but that's a totally arbitrary number -- everything nowadays
	 * has lots of page sizes.
	 * So keep it obvious.
	 */
	return ALIGN((panel_info->vl_col * panel_info->vl_row *
		      ((1<<panel_info->vl_bpix) / 8)), 4096);
}

/*
 * Initialize display controller.
 *
 * @param lcdbase	pointer to the base address of framebuffer.
 * @pd			pointer to the main panel_data structure
 */
void fb_init(vidinfo_t *panel_info, void *lcdbase,
	struct exynos5_fimd_panel *pd)
{
	unsigned int val;
	u32 fbsize;
	struct exynos5_fimd *fimd =
		samsung_get_base_fimd();
	struct exynos5_disp_ctrl *disp_ctrl =
		samsung_get_base_disp_ctrl();

	writel(pd->ivclk | pd->fixvclk, &disp_ctrl->vidcon1);
	val = ENVID_ON | ENVID_F_ON | (pd->clkval_f << CLKVAL_F_OFFSET);
	writel(val, &fimd->vidcon0);

	val = (pd->vsync << VSYNC_PULSE_WIDTH_OFFSET) |
		(pd->lower_margin << V_FRONT_PORCH_OFFSET) |
		(pd->upper_margin << V_BACK_PORCH_OFFSET);
	writel(val, &disp_ctrl->vidtcon0);

	val = (pd->hsync << HSYNC_PULSE_WIDTH_OFFSET) |
		(pd->right_margin << H_FRONT_PORCH_OFFSET) |
		(pd->left_margin << H_BACK_PORCH_OFFSET);
	writel(val, &disp_ctrl->vidtcon1);

	val = ((pd->xres - 1) << HOZVAL_OFFSET) |
		((pd->yres - 1) << LINEVAL_OFFSET);
	writel(val, &disp_ctrl->vidtcon2);

	writel((unsigned int)lcdbase, &fimd->vidw00add0b0);

	fbsize = calc_fbsize(panel_info);
	writel((unsigned int)lcdbase + fbsize, &fimd->vidw00add1b0);

	writel(pd->xres * 2, &fimd->vidw00add2);

	val = ((pd->xres - 1) << OSD_RIGHTBOTX_F_OFFSET);
	val |= ((pd->yres - 1) << OSD_RIGHTBOTY_F_OFFSET);
	writel(val, &fimd->vidosd0b);
	writel(pd->xres * pd->yres, &fimd->vidosd0c);

	setbits_le32(&fimd->shadowcon, CHANNEL0_EN);

	val = BPPMODE_F_RGB_16BIT_565 << BPPMODE_F_OFFSET;
	val |= ENWIN_F_ENABLE | HALF_WORD_SWAP_EN;
	writel(val, &fimd->wincon0);

	/* DPCLKCON_ENABLE */
	writel(1 << 1, &fimd->dpclkcon);
}

void exynos_fimd_disable(void);
void exynos_fimd_disable(void)
{
	struct exynos5_fimd *fimd = samsung_get_base_fimd();

	writel(0, &fimd->wincon0);
	clrbits_le32(&fimd->shadowcon, CHANNEL0_EN);
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
	u32 start;
	struct exynos5_dp *base = dp->base;

	s5p_dp_config_video_slave_mode(dp, video_info);

	s5p_dp_set_video_color_format(dp, video_info->color_depth,
				      video_info->color_space,
				      video_info->dynamic_range,
				      video_info->ycbcr_coeff);

	if (s5p_dp_get_pll_lock_status(dp) == PLL_UNLOCKED) {
		printk(BIOS_DEBUG, "PLL is not locked yet.\n");
		return -ERR_PLL_NOT_UNLOCKED;
	}

	start = get_timer(0);
	do {
		if (s5p_dp_is_slave_video_stream_clock_on(dp) == 0) {
			timeout++;
			break;
		}
	} while (get_timer(start) <= STREAM_ON_TIMEOUT);

	if (!timeout) {
		printk(BIOS_DEBUG, "Video Clock Not ok\n");
		return -ERR_VIDEO_CLOCK_BAD;
	}

	/* Set to use the register calculated M/N video */
	s5p_dp_set_video_cr_mn(dp, CALCULATED_M, 0, 0);

	clrbits_le32(&base->video_ctl_10, FORMAT_SEL);

	/* Disable video mute */
	clrbits_le32(&base->video_ctl_1, HDCP_VIDEO_MUTE);

	/* Configure video slave mode */
	s5p_dp_enable_video_master(dp);

	/* Enable video */
	setbits_le32(&base->video_ctl_1, VIDEO_EN);
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

	clrbits_le32(&base->dp_training_ptn_set, SCRAMBLING_DISABLE);

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
		clrbits_le32(&base->func_en_1, SW_FUNC_EN_N);

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
		writel(reg, &base->ln0_link_trn_ctl);
		break;
	case 1:
		writel(reg, &base->ln1_link_trn_ctl);
		break;

	case 2:
		writel(reg, &base->ln2_link_trn_ctl);
		break;

	case 3:
		writel(reg, &base->ln3_link_trn_ctl);
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
	u32 data;
	u32 start;
	int lane;
	struct exynos5_dp *base = dp->base;

	/* Stop Video */
	clrbits_le32(&base->video_ctl_1, VIDEO_EN);

	start = get_timer(0);
	while (s5p_dp_get_pll_lock_status(dp) == PLL_UNLOCKED) {
		if (get_timer(start) > PLL_LOCK_TIMEOUT) {
			/* Ignore this error, and try to continue */
			printk(BIOS_ERR, "PLL is not locked yet.\n");
			break;
		}
	}

	/* Reset Macro */
	setbits_le32(&base->dp_phy_test, MACRO_RST);

	/* 10 us is the minimum reset time. */
	udelay(10);

	clrbits_le32(&base->dp_phy_test, MACRO_RST);

	/* Set TX pre-emphasis to minimum */
	for (lane = 0; lane < max_lane; lane++)
		if (s5p_dp_set_lane_lane_pre_emphasis(dp,
					      PRE_EMPHASIS_LEVEL_0, lane)) {
			printk(BIOS_DEBUG, "Unable to set pre emphasis level\n");
			return -ERR_PRE_EMPHASIS_LEVELS;
		}

	/* All DP analog module power up */
	writel(0x00, &base->dp_phy_pd);

	/* Initialize by reading RX's DPCD */
	s5p_dp_get_max_rx_bandwidth(dp, &dp->link_train.link_rate);
	s5p_dp_get_max_rx_lane_count(dp, &dp->link_train.lane_count);

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
	writel(dp->link_train.lane_count, &base->lane_count_set);
	writel(dp->link_train.link_rate, &base->link_bw_set);

	/* Set sink to D0 (Sink Not Ready) mode. */
	s5p_dp_write_byte_to_dpcd(dp, DPCD_ADDR_SINK_POWER_STATE,
				  DPCD_SET_POWER_STATE_D0);

	/* Start HW link training */
	writel(HW_TRAINING_EN, &base->dp_hw_link_training);

	/* Wait unitl HW link training done */
	s5p_dp_wait_hw_link_training_done(dp);

	/* Get hardware link training status */
	data = readl(&base->dp_hw_link_training);
	if (data != 0) {
		printk(BIOS_DEBUG, " H/W link training failure: 0x%x\n", data);
		return -ERR_LINK_TRAINING_FAILURE;
	}

	/* Get Link Bandwidth */
	data = readl(&base->link_bw_set);

	dp->link_train.link_rate = data;

	data = readl(&base->lane_count_set);
	dp->link_train.lane_count = data;

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

	//dp->base = (struct exynos5_dp *)addr;
	/* yes. we're a snow. Yet somehow our config is from a development kit?
	 * This Must Change */
	//dp->video_info = &smdk5250_dp_config;

	clock_init_dp_clock();

	power_enable_dp_phy();
	ret = s5p_dp_init_dp(dp);
	if (ret) {
		printk(BIOS_DEBUG, "%s: Could not initialize dp\n", __func__);
		return ret;
	}

	ret = s5p_dp_hw_link_training(dp, dp->video_info->lane_count,
				      dp->video_info->link_rate);
	if (ret) {
		printk(BIOS_DEBUG, "unable to do link train\n");
		return ret;
	}
	/* Minimum delay after H/w Link training */
	udelay(1000);

	ret = s5p_dp_enable_scramble(dp);
	if (ret) {
		printk(BIOS_DEBUG, "unable to set scramble mode\n");
		return ret;
	}

	ret = s5p_dp_enable_rx_to_enhanced_mode(dp);
	if (ret) {
		printk(BIOS_DEBUG, "unable to set enhanced mode\n");
		return ret;
	}


	base = dp->base;
	/* Enable enhanced mode */
	setbits_le32(&base->sys_ctl_4, ENHANCED);

	writel(dp->link_train.lane_count, &base->lane_count_set);
	writel(dp->link_train.link_rate, &base->link_bw_set);

	s5p_dp_init_video(dp);
	ret = s5p_dp_config_video(dp, dp->video_info);
	if (ret) {
		printk(BIOS_DEBUG, "unable to config video\n");
		return ret;
	}

	return 0;
}

/**
 * Init the LCD controller
 *
 * @param lcdbase	Base address of LCD frame buffer
 * @return 0 if ok, -ve error code on error
 */
int lcd_ctrl_init(vidinfo_t *panel_info,
		struct exynos5_fimd_panel *panel_data, void *lcdbase)
{
	int ret = 0;

	fimd_bypass();
	fb_init(panel_info, lcdbase, panel_data);
	printk(BIOS_SPEW,
		"fb_init(%p, %p, %p) done\n", panel_info, lcdbase, panel_data);

	/* Enable flushing after LCD writes if requested */
	// forget it. lcd_set_flush_dcache(1);
	return ret;
}
