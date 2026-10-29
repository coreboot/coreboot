/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <commonlib/rational.h>
#include <console/console.h>
#include <edid.h>
#include <delay.h>
#include <types.h>
#include <string.h>
#include <soc/clock.h>
#include <gpio.h>
#include <soc/display/edp_reg.h>
#include <soc/display/edp_aux.h>
#include <soc/display/edp_ctrl.h>
#include <soc/display/edp_phy.h>
#include <soc/display/edp_link_train.h>
#include <soc/display/mdssreg.h>

#define BPC_TO_ENUM(bpc) ((enum edp_color_depth)((bpc / 2) - 3))

static uint8_t dp_get_lane_status(const uint8_t link_status[DP_LINK_STATUS_SIZE], int lane)
{
	int i = DP_LANE0_1_STATUS + (lane >> 1);
	int s = (lane & 1) * 4;
	uint8_t l = link_status[i - DP_LANE0_1_STATUS];
	return (l >> s) & 0xf;
}

static uint8_t edp_get_adjust_request_voltage(const uint8_t link_status[DP_LINK_STATUS_SIZE],
					      int lane)
{
	int i = DP_ADJUST_REQUEST_LANE0_1 + (lane >> 1);
	int s = ((lane & 1) ? DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT :
			      DP_ADJUST_VOLTAGE_SWING_LANE0_SHIFT);
	uint8_t l = link_status[i - DP_LANE0_1_STATUS];

	return ((l >> s) & 0x3) << DP_TRAIN_VOLTAGE_SWING_SHIFT;
}

static uint8_t
edp_get_adjust_request_pre_emphasis(const uint8_t link_status[DP_LINK_STATUS_SIZE], int lane)
{
	int i = DP_ADJUST_REQUEST_LANE0_1 + (lane >> 1);
	int s = ((lane & 1) ? DP_ADJUST_PRE_EMPHASIS_LANE1_SHIFT :
			      DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT);
	uint8_t l = link_status[i - DP_LANE0_1_STATUS];

	return ((l >> s) & 0x3) << DP_TRAIN_PRE_EMPHASIS_SHIFT;
}

static void edp_link_train_clock_recovery_delay(const uint8_t dpcd[DP_RECEIVER_CAP_SIZE])
{
	int rd_interval = dpcd[DP_TRAINING_AUX_RD_INTERVAL] & DP_TRAINING_AUX_RD_MASK;

	if (rd_interval > 4)
		printk(BIOS_ERR, "AUX interval %d, out of range (max 4)\n", rd_interval);

	udelay(100);
}

static bool edp_clock_recovery_ok(const uint8_t link_status[DP_LINK_STATUS_SIZE],
				  int lane_count)
{
	for (int lane = 0; lane < lane_count; lane++) {
		uint8_t lane_status = dp_get_lane_status(link_status, lane);
		if ((lane_status & DP_LANE_CR_DONE) == 0) {
			printk(BIOS_ERR, "clock recovery ok failed : %x\n", lane_status);
			return false;
		} else {
			printk(BIOS_DEBUG, "Lane status if not failed 0x%08x\n", lane_status);
		}
	}
	return true;
}

static void edp_link_train_channel_eq_delay(const uint8_t dpcd[DP_RECEIVER_CAP_SIZE])
{
	int rd_interval = dpcd[DP_TRAINING_AUX_RD_INTERVAL] & DP_TRAINING_AUX_RD_MASK;

	if (rd_interval > 4)
		printk(BIOS_INFO, "AUX interval %d, out of range (max 4)\n", rd_interval);

	/*
     * The DPCD stores the AUX read interval in units of 4 ms.
     * If the TRAINING_AUX_RD_INTERVAL field is 0, the channel equalization
     * should use 400 us AUX read intervals.
     */
	printk(BIOS_INFO, "AUX interval %d\n", rd_interval);
	if (rd_interval == 0)
		udelay(400);
	else
		mdelay(rd_interval * 4);
}

static bool edp_channel_eq_ok(const uint8_t link_status[DP_LINK_STATUS_SIZE], int lane_count)
{
	uint8_t lane_align = link_status[DP_LANE_ALIGN_STATUS_UPDATED - DP_LANE0_1_STATUS];
	if ((lane_align & DP_INTERLANE_ALIGN_DONE) == 0)
		return false;

	for (int lane = 0; lane < lane_count; lane++) {
		uint8_t lane_status = dp_get_lane_status(link_status, lane);
		if ((lane_status & DP_CHANNEL_EQ_BITS) != DP_CHANNEL_EQ_BITS)
			return false;
	}
	return true;
}

static void edp_ctrl_irq_enable(int enable)
{
	if (enable) {
		write32(&edp_ahbclk->interrupt_status, EDP_INTERRUPT_STATUS1_MASK);
		write32(&edp_ahbclk->interrupt_status2, EDP_INTERRUPT_STATUS2_MASK);
	} else {
		write32(&edp_ahbclk->interrupt_status, EDP_INTERRUPT_STATUS1_ACK);
		write32(&edp_ahbclk->interrupt_status2, EDP_INTERRUPT_STATUS2_ACK);
	}
}

static void edp_state_ctrl(uint32_t state)
{
	write32(&edp_lclk->state_ctrl, state);
	printk(BIOS_DEBUG, "State =  0x%x\n", state);
}

static int edp_lane_set_write(uint8_t voltage_level, uint8_t pre_emphasis_level)
{
	uint8_t buf[4];

	if (voltage_level >= DPCD_LINK_VOLTAGE_MAX)
		voltage_level |= 0x04;

	if (pre_emphasis_level >= DPCD_LINK_PRE_EMPHASIS_MAX)
		pre_emphasis_level |= 0x20;

	for (int i = 0; i < 4; i++)
		buf[i] = voltage_level | (pre_emphasis_level << 3);

	printk(BIOS_DEBUG, "VP:perlane =0x%x:0x%x:0x%x:0x%x\n", buf[0], buf[1], buf[2], buf[3]);
	if (edp_aux_transfer(DP_TRAINING_LANE0_SET, DP_AUX_NATIVE_WRITE, buf, 4) < 4) {
		printk(BIOS_ERR, "%s: Set sw/pe to panel failed\n", __func__);
		return -1;
	}
	return 0;
}

static int edp_train_pattern_set_write(uint8_t pattern)
{
	uint8_t p = pattern;

	printk(BIOS_INFO, "pattern=%x\n", p);
	if (edp_aux_transfer(DP_TRAINING_PATTERN_SET, DP_AUX_NATIVE_WRITE, &p, 1) < 1) {
		printk(BIOS_ERR, "%s: Set training pattern to panel failed\n", __func__);
		return -1;
	}
	return 0;
}

static void edp_sink_train_set_adjust(struct edp_ctrl *ctrl, const uint8_t *link_status)
{
	uint8_t max = 0;

	/* use the max level across lanes */
	for (int i = 0; i < ctrl->lane_cnt; i++) {
		uint8_t data = edp_get_adjust_request_voltage(link_status, i);
		if (max < data)
			max = data;
	}
	ctrl->v_level = max >> DP_TRAIN_VOLTAGE_SWING_SHIFT;

	/* use the max level across lanes */
	max = 0;
	for (int i = 0; i < ctrl->lane_cnt; i++) {
		uint8_t data = edp_get_adjust_request_pre_emphasis(link_status, i);

		if (max < data)
			max = data;
	}
	ctrl->p_level = max >> DP_TRAIN_PRE_EMPHASIS_SHIFT;

	printk(BIOS_INFO, "Requested_max_all lanes v_level=%d, p_level=%d\n", ctrl->v_level,
	       ctrl->p_level);
}

static void edp_host_train_set(uint32_t train)
{
	int cnt = 10;
	uint32_t shift = train - 1;

	printk(BIOS_INFO, "train=%d\n", train);

	edp_state_ctrl(SW_LINK_TRAINING_PATTERN1 << shift);
	while (--cnt) {
		uint32_t data = read32(&edp_lclk->mainlink_ready);
		if (data & (EDP_MAINLINK_READY_TRAIN_PATTERN_1_READY << shift))
			break;
	}
	if (cnt == 0)
		printk(BIOS_INFO, "%s: set link_train failed\n", __func__);
}

static int edp_voltage_pre_emphasis_set(struct edp_ctrl *ctrl)
{
	printk(BIOS_INFO, "v=%d p=%d\n", ctrl->v_level, ctrl->p_level);
	edp_phy_config(ctrl->v_level, ctrl->p_level, ctrl->link_rate_khz);
	return edp_lane_set_write(ctrl->v_level, ctrl->p_level);
}

static int edp_start_link_train_1(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint8_t link_status[DP_LINK_STATUS_SIZE];
	uint8_t old_v_level;
	int tries;
	int ret, rlen;

	edp_state_ctrl(0);
	edp_host_train_set(DP_TRAINING_PATTERN_1);

	ret = edp_train_pattern_set_write(DP_TRAINING_PATTERN_1 | DP_LINK_SCRAMBLING_DISABLE);
	if (ret)
		return ret;

	ret = edp_voltage_pre_emphasis_set(ctrl);
	if (ret)
		return ret;

	tries = 0;
	old_v_level = ctrl->v_level;
	while (1) {
		edp_link_train_clock_recovery_delay(dpcd);

		rlen = edp_aux_transfer(DP_LANE0_1_STATUS, DP_AUX_NATIVE_READ, &link_status,
					DP_LINK_STATUS_SIZE);
		if (rlen < DP_LINK_STATUS_SIZE) {
			printk(BIOS_ERR, "%s: read link status failed\n", __func__);
			return -1;
		}

		if (edp_clock_recovery_ok(link_status, ctrl->lane_cnt)) {
			ret = 0;
			break;
		}

		if (ctrl->v_level == DPCD_LINK_VOLTAGE_MAX) {
			ret = -1;
			break;
		}

		if (old_v_level != ctrl->v_level) {
			tries++;
			if (tries >= 5) {
				ret = -1;
				break;
			}
		} else {
			tries = 0;
			old_v_level = ctrl->v_level;
		}

		edp_sink_train_set_adjust(ctrl, link_status);
		ret = edp_voltage_pre_emphasis_set(ctrl);
		if (ret)
			return ret;
	}
	return ret;
}

static int edp_start_link_train_2(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint8_t link_status[DP_LINK_STATUS_SIZE];
	int tries = 0;
	int ret, rlen;

	edp_host_train_set(DP_TRAINING_PATTERN_3);

	ret = edp_train_pattern_set_write(DP_TRAINING_PATTERN_3 | DP_LINK_SCRAMBLING_DISABLE);
	if (ret)
		return ret;

	edp_link_train_channel_eq_delay(dpcd);

	ret = edp_voltage_pre_emphasis_set(ctrl);
	if (ret)
		return ret;

	while (1) {
		edp_link_train_channel_eq_delay(dpcd);

		rlen = edp_aux_transfer(DP_LANE0_1_STATUS, DP_AUX_NATIVE_READ, &link_status,
					DP_LINK_STATUS_SIZE);
		printk(BIOS_DEBUG, "Lane status =0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n\n",
		       link_status[0], link_status[1], link_status[2], link_status[3],
		       link_status[4], link_status[5]);

		if (rlen < DP_LINK_STATUS_SIZE) {
			printk(BIOS_ERR, "%s: read link status failed\n", __func__);
			return -1;
		}

		if (edp_channel_eq_ok(link_status, ctrl->lane_cnt)) {
			ret = 0;
			break;
		}

		tries++;
		if (tries > 10) {
			ret = -1;
			break;
		}

		edp_sink_train_set_adjust(ctrl, link_status);
		ret = edp_voltage_pre_emphasis_set(ctrl);
		if (ret)
			return ret;
	}
	return ret;
}

static int edp_link_rate_down_shift(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	int ret = 0;
	int link_rate = ctrl->link_rate_khz;

	switch (link_rate) {
	case 810000:
		link_rate = 540000;
		break;
	case 540000:
		link_rate = 270000;
		break;
	case 270000:
		link_rate = 162000;
		break;
	case 162000:
	default:
		ret = -1;
		break;
	}

	if (!ret) {
		ctrl->link_rate_khz = link_rate;
		ctrl->link_rate = link_rate / DP_LINK_RATE_UNIT_KHZ;
		printk(BIOS_INFO, "new rate=%d\n", ctrl->link_rate_khz);
	}
	return ret;
}

static bool edp_clock_recovery_reduced_lanes(const uint8_t link_status[DP_LINK_STATUS_SIZE],
					     uint32_t lane_cnt)
{
	if (lane_cnt <= 1)
		return false;

	int reduced_lanes = lane_cnt >> 1;
	return edp_clock_recovery_ok(link_status, reduced_lanes);
}

static int edp_link_lane_down_shift(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	if (ctrl->lane_cnt <= 1)
		return -1;

	ctrl->lane_cnt = ctrl->lane_cnt >> 1;
	ctrl->link_rate_khz = dpcd[DP_MAX_LINK_RATE] * DP_LINK_RATE_UNIT_KHZ;
	ctrl->link_rate = dpcd[DP_MAX_LINK_RATE];
	ctrl->p_level = 0;
	ctrl->v_level = 0;
	return 0;
}

static int edp_clear_training_pattern(uint8_t *dpcd)
{
	int ret = edp_train_pattern_set_write(0);
	edp_link_train_channel_eq_delay(dpcd);
	return ret;
}

static int edp_do_link_train(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint8_t values[2], edp_config = 0;
	int ret;
	int rlen;
	uint8_t link_status[DP_LINK_STATUS_SIZE];

	/*
     * Set the current link rate and lane cnt to panel. They may have been
     * adjusted and the values are different from them in DPCD CAP
     */
	values[0] = ctrl->link_rate;
	values[1] = ctrl->lane_cnt;

	if (dpcd[DP_DPCD_REV] >= 0x11 && (dpcd[DP_MAX_LANE_COUNT] & DP_ENHANCED_FRAME_CAP))
		values[1] |= DP_LANE_COUNT_ENHANCED_FRAME_EN;

	if (ctrl->use_rate_select) {
		ret = edp_aux_transfer(DP_LANE_COUNT_SET, DP_AUX_NATIVE_WRITE, &values[1], 1);
		if (ret < 1) {
			printk(BIOS_ERR, "failed to write lane count, ret:%d\n", ret);
			return EDP_TRAIN_FAIL;
		}

		/* write link rate index (DPCD 0x0115) */
		ret = edp_aux_transfer(DP_LINK_RATE_SET, DP_AUX_NATIVE_WRITE,
				       &ctrl->rate_select_idx, 1);
		if (ret < 1) {
			printk(BIOS_ERR, "failed to write link rate, ret:%d\n", ret);
			return EDP_TRAIN_FAIL;
		}
	} else {
		if (edp_aux_transfer(DP_LINK_BW_SET, DP_AUX_NATIVE_WRITE, &values[0], 1) < 0)
			return EDP_TRAIN_FAIL;

		printk(BIOS_DEBUG, "Setting %8x  : %x\n", DP_LINK_BW_SET, values[0]);

		edp_aux_transfer(DP_LANE_COUNT_SET, DP_AUX_NATIVE_WRITE, &values[1], 1);
		ctrl->v_level = 0; /* start from default level */
		ctrl->p_level = 0;

		printk(BIOS_DEBUG, "Setting %8x  : %x\n", DP_LANE_COUNT_SET, values[1]);
	}

	edp_config = DP_ALTERNATE_SCRAMBLER_RESET_ENABLE;
	edp_aux_transfer(DP_EDP_CONFIGURATION_SET, DP_AUX_NATIVE_WRITE, &edp_config, 1);
	printk(BIOS_DEBUG, "Setting %8x  : %x\n", DP_EDP_CONFIGURATION_SET, edp_config);

	write32(&edp_p0clk->intf_config, 0x10); /* Databus Widen, 2 Pixel / Pixels per clock */

	values[0] = 0x10;
	if (dpcd[DP_MAX_DOWNSPREAD] & 1)
		values[0] = DP_SPREAD_AMP_0_5;

	values[1] = 1;

	ret = edp_start_link_train_1(ctrl, dpcd);
	if (ret < 0) {
		rlen = edp_aux_transfer(DP_LANE0_1_STATUS, DP_AUX_NATIVE_READ, &link_status,
					DP_LINK_STATUS_SIZE);
		if (rlen < DP_LINK_STATUS_SIZE) {
			printk(BIOS_ERR, "%s: read link status failed\n", __func__);
			return -1;
		}

		ret = edp_link_rate_down_shift(ctrl, dpcd);
		if (!ret) {
			printk(BIOS_ERR, "link reconfig\n");
			ret = EDP_TRAIN_RECONFIG;
		} else if (ret < 0) {
			if (edp_clock_recovery_reduced_lanes(link_status, ctrl->lane_cnt) ==
			    0) {
				if (edp_link_lane_down_shift(ctrl, dpcd) < 0) {
					printk(BIOS_ERR, "%s: Training 1 failed\n", __func__);
					ret = EDP_TRAIN_FAIL;
				} else {
					printk(BIOS_ERR, "link reconfig\n");
					ret = EDP_TRAIN_RECONFIG;
				}
			} else {
				printk(BIOS_ERR, "%s: Training 1 failed\n", __func__);
				ret = EDP_TRAIN_FAIL;
			}
		}
		edp_clear_training_pattern(dpcd);
		return ret;
	}

	printk(BIOS_INFO, "Training 1 completed successfully\n");
	edp_state_ctrl(0);

	ret = edp_start_link_train_2(ctrl, dpcd);
	if (ret < 0) {
		rlen = edp_aux_transfer(DP_LANE0_1_STATUS, DP_AUX_NATIVE_READ, &link_status,
					DP_LINK_STATUS_SIZE);
		if (rlen < DP_LINK_STATUS_SIZE) {
			printk(BIOS_ERR, "%s: read link status failed\n", __func__);
			return -1;
		}

		if (edp_clock_recovery_ok(link_status, ctrl->lane_cnt)) {
			if (edp_link_rate_down_shift(ctrl, dpcd) == 0) {
				printk(BIOS_ERR, "link reconfig\n");
				ret = EDP_TRAIN_RECONFIG;
			} else {
				printk(BIOS_ERR, "%s: Training 2 failed\n", __func__);
				ret = EDP_TRAIN_FAIL;
			}
		} else {
			if (edp_link_lane_down_shift(ctrl, dpcd) < 0) {
				printk(BIOS_ERR, "%s: Training 1 failed\n", __func__);
				ret = EDP_TRAIN_FAIL;
			} else {
				printk(BIOS_ERR, "link reconfig\n");
				ret = EDP_TRAIN_RECONFIG;
			}
		}
		edp_clear_training_pattern(dpcd);
		return ret;
	}

	printk(BIOS_INFO, "Training 2 completed successfully\n");
	edp_config = DP_ALTERNATE_SCRAMBLER_RESET_ENABLE;
	edp_aux_transfer(DP_EDP_CONFIGURATION_SET, DP_AUX_NATIVE_WRITE, &edp_config, 1);
	return ret;
}

static int edp_ctrl_pixel_clock_dividers(struct edp_ctrl *ctrl, uint32_t *pixel_m,
					 uint32_t *pixel_n)
{
	uint32_t pixel_div = 0, dispcc_input_rate;
	unsigned long den, num;
	uint8_t rate = (ctrl->link_rate);

	uint32_t stream_rate_khz =
		(ctrl->pixel_rate) /
		2; /* Databus Widen, 2 Pixel / Pixels per clock settings in edp_do_link_train() */

	if (rate == DP_LINK_BW_8_1)
		pixel_div = 6;
	else if (rate == DP_LINK_BW_1_62 || rate == DP_LINK_BW_2_7)
		pixel_div = 2;
	else if (rate == DP_LINK_BW_5_4)
		pixel_div = 4;
	else {
		printk(BIOS_ERR, "Invalid pixel mux divider\n");
		return -1;
	}

	dispcc_input_rate = (ctrl->link_rate_khz * 10) / pixel_div;

	rational_best_approximation(dispcc_input_rate, stream_rate_khz,
				    (uint64_t)(1 << 16) - 1, (uint64_t)(1 << 16) - 1,
				    &den, &num);

	printk(BIOS_INFO, "M = %lu , N= %lu\n", num, den);
	*pixel_m = num;
	*pixel_n = den;
	return 0;
}

static int edp_check_link_bandwidth(uint32_t pixel_clock_khz, uint32_t link_rate_khz,
				 uint32_t lane_count, uint32_t bits_per_color)
{
	uint32_t bpp = bits_per_color * 3;

	uint64_t pixel_data_rate_bps = (uint64_t)pixel_clock_khz * 1000ULL * bpp;

	uint64_t raw_link_bps = (uint64_t)link_rate_khz * 1000ULL * 10ULL * lane_count;

	/* payload shrink */
	uint64_t effective_link_bps = (raw_link_bps * 8ULL) / 10ULL;

	return pixel_data_rate_bps <= effective_link_bps;
}

static uint32_t choose_best_color_depth(struct edid *edid, uint32_t link_rate_khz,
					    uint32_t lane_count)
{
	uint32_t clk = edid->mode.pixel_clock;

	printk(BIOS_DEBUG, "%d %d %d\n", clk, link_rate_khz, lane_count);

	if ((edid->panel_bits_per_color >= COLOR_DEPTH_10BPC) &&
		(edp_check_link_bandwidth(clk, link_rate_khz, lane_count, 10)))
		return COLOR_DEPTH_10BPC;

	if ((edid->panel_bits_per_color >= COLOR_DEPTH_8BPC) &&
		(edp_check_link_bandwidth(clk, link_rate_khz, lane_count, 8)))
		return COLOR_DEPTH_8BPC;

	if ((edid->panel_bits_per_color >= COLOR_DEPTH_6BPC) &&
		(edp_check_link_bandwidth(clk, link_rate_khz, lane_count, 6)))
		return COLOR_DEPTH_6BPC;

	printk(BIOS_ERR, "Searching for suitable color width failed\n");
	return 0;
}

static void edp_config_ctrl(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint32_t config = 0, depth = BPC_TO_ENUM(ctrl->color_depth);

	/* Default-> LSCLK DIV: 1/4 LCLK  */
	config |= (2 << EDP_CONFIGURATION_CTRL_LSCLK_DIV_SHIFT);

	/* Scrambler reset enable */
	if (dpcd[DP_EDP_CONFIGURATION_CAP] & DP_ALTERNATE_SCRAMBLER_RESET_CAP)
		config |= EDP_CONFIGURATION_CTRL_ASSR;

	config |= depth << EDP_CONFIGURATION_CTRL_BPC_SHIFT;

	/* Num of Lanes */
	config |= ((ctrl->lane_cnt - 1) << EDP_CONFIGURATION_CTRL_NUM_OF_LANES_SHIFT);

	if (dpcd[DP_DPCD_REV] >= 0x11 && (dpcd[DP_MAX_LANE_COUNT] & DP_ENHANCED_FRAME_CAP))
		config |= EDP_CONFIGURATION_CTRL_ENHANCED_FRAMING;

	config |= EDP_CONFIGURATION_CTRL_P_INTERLACED; /* progressive video */

	/* sync clock & static Mvid */
	config |= EDP_CONFIGURATION_CTRL_STATIC_DYNAMIC_CN;
	config |= EDP_CONFIGURATION_CTRL_SYNC_ASYNC_CLK;

	write32(&edp_lclk->configuration_ctrl, config);
}

static void edp_ctrl_config_misc(struct edp_ctrl *ctrl)
{
	uint32_t misc_val;
	enum edp_color_depth depth = BPC_TO_ENUM(ctrl->color_depth);

	misc_val = read32(&edp_lclk->misc1_misc0);

	/* clear bpp bits */
	misc_val &= ~(0x07 << EDP_MISC0_TEST_BITS_DEPTH_SHIFT);
	misc_val |= depth << EDP_MISC0_TEST_BITS_DEPTH_SHIFT;

	/* Configure clock to synchronous mode */
	misc_val |= EDP_MISC0_SYNCHRONOUS_CLK;
	write32(&edp_lclk->misc1_misc0, misc_val);
}

static int edp_ctrl_config_msa(struct edp_ctrl *ctrl)
{
	uint32_t pixel_m, pixel_n;
	uint32_t mvid, nvid;
	const u32 nvid_fixed = 0x8000;
	uint8_t rate = ctrl->link_rate;

	if (edp_ctrl_pixel_clock_dividers(ctrl, &pixel_m, &pixel_n) < 0)
		return -1;

	mvid = (pixel_m & 0xFFFF) * 5;

	nvid = (0xFFFF & (~pixel_n)) + (pixel_m & 0xFFFF);

	if (nvid < nvid_fixed) {
		printk(BIOS_INFO, "nvid is< nvid_fixed\n");
		u32 temp;

		temp = (nvid_fixed / nvid) * nvid;
		mvid = (nvid_fixed / nvid) * mvid;
		nvid = temp;
	}

	mvid = mvid * 2; /* Databus Widen, 2 Pixel / Pixels clock */

	/* Link Rate scaling */
	if (rate == DP_LINK_BW_5_4)
		nvid *= 2;

	if (rate == DP_LINK_BW_8_1)
		nvid *= 3;

	printk(BIOS_INFO, "mvid=0x%x, nvid=0x%x\n", mvid, nvid);

	write32(&edp_lclk->software_mvid, mvid);
	write32(&edp_lclk->software_nvid, nvid);

	write32(&edp_p0clk->dsc_dto, 0x0);
	return 0;
}

static void edp_ctrl_timing_cfg(struct edid *edid)
{
	uint32_t hpolarity = (edid->mode.phsync == '+');

	/* This has to be plus "+", since there is bug in EDID parsing,
	which fills + if vsyns is not present in EDID,
	we are keeping it minus "-"for missing override. */
	uint32_t vpolarity = (edid->mode.pvsync == '-');

	printk(BIOS_DEBUG,
	       "Confirm Parsing phsync=%c pvsync=%c hpol=%u vpol=%u hspw=0x%x vspw=0x%x\n",
	       edid->mode.phsync, edid->mode.pvsync, hpolarity, vpolarity, edid->mode.hspw,
	       edid->mode.vspw);

	/* Configure eDP timing to HW */
	write32(&edp_lclk->total_hor_ver,
		(edid->mode.ha + edid->mode.hbl) |
			(((edid->mode.va + edid->mode.vbl) << 16) & 0xffff0000));

	write32(&edp_lclk->start_hor_ver_from_sync,
		(edid->mode.hbl - edid->mode.hso) |
			(((edid->mode.vbl - edid->mode.vso) << 16) & 0xffff0000));

	write32(&edp_lclk->hysnc_vsync_width_polarity,
		(edid->mode.hspw & 0x7fff) |                 /* HSW[14:0] */
			(hpolarity << 15) |                  /* HSP[15] */
			((edid->mode.vspw & 0x7fff) << 16) | /* VSW[30:16] */
			(vpolarity << 31));

	write32(&edp_lclk->active_hor_ver,
		(edid->mode.ha) | ((edid->mode.va << 16) & 0xffff0000));
}

static void edp_mainlink_ctrl(int enable)
{
	uint32_t data = 0;

	write32(&edp_lclk->mainlink_ctrl, 0x2);
	if (enable)
		data |= 0x1;
	write32(&edp_lclk->mainlink_ctrl, data);
}

static void edp_ctrl_link_enable(struct edp_ctrl *ctrl, struct edid *edid, uint8_t *dpcd,
				 int enable)
{
	int ret = 0;
	uint32_t m = 0, n = 0;

	if (enable) {
		uint64_t link_hz;

		edp_phy_power_on(ctrl->link_rate_khz, ctrl->lane_cnt);

		/* link_rate_khz is in kHz -> convert to Hz */
		link_hz = (uint64_t)ctrl->link_rate_khz * 1000ULL;

		struct clock_freq_config link_cfg[] = {
			{
				.hz = link_hz,
				.src = 4,
				.div = QCOM_CLOCK_DIV(1),
			 },
		};

		printk(BIOS_DEBUG, "[ Starting mdss_dptx3_link_rcg @ %llu Hz ]\n", link_hz);

		clock_configure(&disp_cc->mdss_dptx3_link_rcg, link_cfg, link_hz,
				ARRAY_SIZE(link_cfg));

		ret = mdss_clock_enable(DISP_CC_MDSS_DPTX3_LINK_CBCR);
		if (ret)
			printk(BIOS_ERR, "failed to enable link clk\n");

		ret = mdss_clock_enable(DISP_CC_MDSS_DPTX3_LINK_INTF_CBCR);
		if (ret)
			printk(BIOS_ERR, "failed to enable link intf clk\n");

		/* Get pixel clock divider values for the current mode/link */
		edp_ctrl_pixel_clock_dividers(ctrl, &m, &n);

		/* divide by 2 because of Databus widen.
		2 pixels per clock. 326 for <if:65537> pixel clock from DPCD */
		uint64_t pixel_hz = (ctrl->pixel_rate / 1000) / 2 * MHz;

		struct clock_freq_config pixel0_cfg[] = {
			{
				.hz = pixel_hz,
				.src = 3,
				.div = QCOM_CLOCK_DIV(1),
				.m = m,
				.n = n,
				.d_2 = n,
			 },
		};

		printk(BIOS_DEBUG, "[ pixel0: hz=%llu m=%u n=%u ]\n", pixel_hz, m, n);

		clock_configure(&disp_cc->mdss_dptx3_pixel0_rcg, pixel0_cfg, pixel_hz,
				ARRAY_SIZE(pixel0_cfg));
		clock_enable(&disp_cc->mdss_dptx3_pixel0_cbcr);

		printk(BIOS_DEBUG, "[ Setting Mainlink control as 1 ]\n");

		/* mainlink_levels */
		uint32_t safe_to_exit_level = eDP_SAFE_EXIT_LVL_LANE1;
		switch (ctrl->lane_cnt) {
		case 1:
			safe_to_exit_level = eDP_SAFE_EXIT_LVL_LANE1;
			break;
		case 2:
			safe_to_exit_level = eDP_SAFE_EXIT_LVL_LANE2;
			break;
		case 4:
			safe_to_exit_level = eDP_SAFE_EXIT_LVL_LANE4;
			break;
		default:
			printk(BIOS_DEBUG, "setting the default safe_to_exit_level = %u\n",
			       safe_to_exit_level);
			break;
		}

		write32(&edp_lclk->mainlink_levels, safe_to_exit_level);

		edp_mainlink_ctrl(1);
	} else {
		edp_mainlink_ctrl(0);
		/* disable clocks here if your flow expects it */
	}
}

static int edp_ctrl_training(struct edp_ctrl *ctrl, struct edid *edid, uint8_t *dpcd)
{
	/* Do link training only when power is on */
	int ret = edp_do_link_train(ctrl, dpcd);

	/* Re-configure main link */
	while (ret == EDP_TRAIN_RECONFIG) {
		edp_ctrl_irq_enable(0);
		edp_ctrl_link_enable(ctrl, edid, dpcd, 0);

		/* edp Phy enable */
		write32(&edp_ahbclk->phy_ctrl, 0x4 | 0x1);
		write32(&edp_ahbclk->phy_ctrl, 0x0);
		edp_phy_enable(ctrl->link_rate_khz);

		edp_ctrl_irq_enable(1);
		edp_ctrl_link_enable(ctrl, edid, dpcd, 1);

		ret = edp_do_link_train(ctrl, dpcd);
	}
	return ret;
}

static void edp_get_best_rate_from_table(struct edp_ctrl *ctrl, uint8_t *dpcd)
{
	uint8_t tbl[DP_EDP_RATE_TABLE_SIZE] = {0};
	int ret;
	int best_idx = -1;
	uint32_t best_rate_khz = 0;

	ret = edp_aux_transfer(DP_SUPPORTED_LINK_RATES, DP_AUX_NATIVE_READ, tbl, sizeof(tbl));
	if (ret < 0) {
		printk(BIOS_ERR, "SUPPORTED_LINK_RATES read failed\n");
	}

	/* Parse up to 8 entries (each 2 bytes) */
	for (int i = 0; i < DP_EDP_RATE_TABLE_SIZE / 2; i++) {
		uint16_t raw = tbl[i * 2] | (tbl[i * 2 + 1] << 8);
		if (!raw)
			break;

		uint32_t rate_khz = raw * 20;

		if (rate_khz <= 810000 && rate_khz > best_rate_khz) {
			best_rate_khz = rate_khz;
			best_idx = i;
		}
	}

	ctrl->link_rate_khz = best_rate_khz;
	ctrl->use_rate_select = true;
	ctrl->rate_select_idx = best_idx;

	/* manual override for link rate < best_idx = -1;> */
	if (best_idx < 0) {
		ctrl->link_rate_khz = 540000;
		ctrl->use_rate_select = false;
		ctrl->rate_select_idx = 0;

		printk(BIOS_DEBUG, "No valid eDP rate table, fallback 540000 kHz\n");
	}

	dpcd[DP_MAX_LINK_RATE] = ctrl->link_rate_khz / DP_LINK_RATE_UNIT_KHZ;
	ctrl->link_rate = dpcd[DP_MAX_LINK_RATE];

	printk(BIOS_DEBUG, "eDP rate-table: best_rate_khz=%u idx=%d\n", best_rate_khz,
	       best_idx);
}

int edp_ctrl_on(struct edp_ctrl *ctrl, struct edid *edid, uint8_t *dpcd)
{
	uint8_t value;
	int ret;

	printk(BIOS_DEBUG, "\nDPCD provided max link rate = %u\n", dpcd[DP_MAX_LINK_RATE]);

	if (dpcd[DP_MAX_LINK_RATE] == 0) {
		edp_get_best_rate_from_table(ctrl, dpcd);
	} else {
		ctrl->link_rate_khz = dpcd[DP_MAX_LINK_RATE] * DP_LINK_RATE_UNIT_KHZ;
		ctrl->link_rate = dpcd[DP_MAX_LINK_RATE];
		ctrl->use_rate_select = false;
		ctrl->rate_select_idx = 0;
	}

	ctrl->lane_cnt = dpcd[DP_MAX_LANE_COUNT] & DP_MAX_LANE_COUNT_MASK;

	edid->panel_bits_per_color =
		choose_best_color_depth(edid, ctrl->link_rate_khz, ctrl->lane_cnt);

	edid->panel_bits_per_pixel = edid->panel_bits_per_color * 3;
	ctrl->color_depth = edid->panel_bits_per_color;
	ctrl->pixel_rate = edid->mode.pixel_clock;

	printk(BIOS_DEBUG,
	       "====================Link Training starts here ========================\n");
	printk(BIOS_DEBUG,
	       "eDP ctrl init: link_rate_khz=%u link_rate=0x%x lane_cnt=%u "
	       "v_level=%u p_level=%u pixel_rate=%u color_depth=%u\n",
	       ctrl->link_rate_khz, ctrl->link_rate, ctrl->lane_cnt, ctrl->v_level,
	       ctrl->p_level, ctrl->pixel_rate, ctrl->color_depth);

	/* DP_SET_POWER register is only available on DPCD v1.1 and later */
	if (dpcd[DP_DPCD_REV] >= 0x11) {
		ret = edp_aux_transfer(DP_SET_POWER, DP_AUX_NATIVE_READ, &value, 1);
		printk(BIOS_DEBUG, "DPCD power read address=%x\n", DP_SET_POWER);
		if (ret < 0) {
			printk(BIOS_ERR, "edp native read failure\n");
			return -1;
		}

		value &= ~DP_SET_POWER_MASK;
		value |= DP_SET_POWER_D0;

		ret = edp_aux_transfer(DP_SET_POWER, DP_AUX_NATIVE_WRITE, &value, 1);
		printk(BIOS_DEBUG, "DPCD power Set address=%x : %x\n", DP_SET_POWER, value);
		if (ret < 0) {
			printk(BIOS_ERR, "edp native read failure\n");
			return -1;
		}

		/*
		* According to the DP 1.1 specification, a "Sink Device must
		* exit the power saving state within 1 ms" (Section 2.5.3.1,
		* Table 5-52, "Sink Control Field" (register 0x600).
		*/
		udelay(1000);
	}

	edp_ctrl_irq_enable(1);
	edp_ctrl_link_enable(ctrl, edid, dpcd, 1);

	/* Start link training */
	ret = edp_ctrl_training(ctrl, edid, dpcd);
	if (ret != EDP_TRAIN_SUCCESS) {
		printk(BIOS_ERR, "edp training failure\n");
		return -1;
	}

	edp_train_pattern_set_write(0);

	printk(BIOS_DEBUG,
	       "====================Link Training Ends here ========================\n\n\n\n");
	edp_state_ctrl(SW_SEND_VIDEO);

	write32(&edp_lclk->mainlink_ctrl, 0x2000000);
	write32(&edp_lclk->mainlink_ctrl, 0x2000002);
	write32(&edp_lclk->mainlink_ctrl, 0x2000000);
	write32(&edp_lclk->mainlink_ctrl, 0x2000001);

	edp_config_ctrl(ctrl, dpcd);
	edp_ctrl_config_misc(ctrl);
	edp_ctrl_timing_cfg(edid);

	if (edp_ctrl_config_msa(ctrl) < 0)
		return -1;

	edp_ctrl_config_TU(ctrl, edid);

	edp_ctrl_irq_enable(0);

	return 0;
}
