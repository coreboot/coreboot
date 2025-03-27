/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/dp_intf.h>
#include <soc/dptx.h>
#include <soc/dptx_hal.h>
#include <soc/dptx_reg.h>

static void mtk_edp_pattern(struct mtk_dp *mtk_dp, u8 lane_count, u8 pattern)
{
	u8 aux_offset;

	switch (pattern) {
	case DPTX_PATTERN_TPS1:
		aux_offset = DP_TRAINING_PATTERN_1 | DP_LINK_SCRAMBLING_DISABLE;
		dptx_hal_phy_set_idle_pattern(mtk_dp, lane_count, false);
		mtk_dp_mask(mtk_dp, REG_3400_DP_TRANS_P0, PATTERN1_EN_DP_TRANS_4P_TPS1,
			    PATTERN_EN_DP_TRANS_4P_MASK);
		break;
	case DPTX_PATTERN_TPS2:
		aux_offset = DP_TRAINING_PATTERN_2 | DP_LINK_SCRAMBLING_DISABLE;
		mtk_dp_mask(mtk_dp, REG_3400_DP_TRANS_P0, PATTERN2_EN_DP_TRANS_4P_TPS2,
			    PATTERN_EN_DP_TRANS_4P_MASK);
		break;
	case DPTX_PATTERN_TPS3:
		aux_offset = DP_TRAINING_PATTERN_3 | DP_LINK_SCRAMBLING_DISABLE;
		mtk_dp_mask(mtk_dp, REG_3400_DP_TRANS_P0, PATTERN3_EN_DP_TRANS_4P_TPS3,
			    PATTERN_EN_DP_TRANS_4P_MASK);
		break;
	case DPTX_PATTERN_TPS4:
		aux_offset = DP_TRAINING_PATTERN_4;
		mtk_dp_mask(mtk_dp, REG_3400_DP_TRANS_P0, PATTERN4_EN_DP_TRANS_4P_TPS4,
			    PATTERN_EN_DP_TRANS_4P_MASK);
		break;
	case DPTX_PATTERN_UNKNOWN:
	default:
		printk(BIOS_ERR, "Set default or unknown pattern\n");
		mtk_dp_mask(mtk_dp, REG_3400_DP_TRANS_P0, 0x0, PATTERN_EN_DP_TRANS_4P_MASK);
		return;
	}

	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00102, 0x1, &aux_offset);
}

static void update_swing_preemphasis(struct mtk_dp *mtk_dp, u8 lane_count,
				     u8 dpcd_adjust_req[DP_LANSE_ADJUST_SIZE])
{
	u8 swing_val[MAX_LANECOUNT];
	u8 preemphasis[MAX_LANECOUNT];

	for (int lane = 0; lane < lane_count; lane++) {
		u8 val;
		int index = lane / 2;
		assert(index < DP_LANSE_ADJUST_SIZE);
		int shift = lane % 2 ? DP_ADJUST_VOLTAGE_SWING_LANE1_SHIFT : 0;
		u8 swing_value = dpcd_adjust_req[index] >> shift;

		if (mtk_dp->force_max_swing) {
			swing_val[lane] = DPTX_SWING3;
			printk(BIOS_INFO, "%s: Force swing setting to %u (500 mV)\n",
			       __func__, swing_val[lane]);
		} else {
			swing_val[lane] = swing_value & DP_ADJUST_VOLTAGE_SWING_LANE0_MASK;
		}
		preemphasis[lane] = swing_value & DP_ADJUST_PRE_EMPHASIS_LANE0_MASK;
		preemphasis[lane] >>= DP_ADJUST_PRE_EMPHASIS_LANE0_SHIFT;
		val = swing_val[lane] << DP_TRAIN_VOLTAGE_SWING_SHIFT |
		      preemphasis[lane] << DP_TRAIN_PRE_EMPHASIS_SHIFT;

		if (swing_val[lane] == DPTX_SWING3)
			val |= DP_TRAIN_MAX_SWING_REACHED;
		if (preemphasis[lane] == DPTX_PREEMPHASIS3)
			val |= DP_TRAIN_MAX_PRE_EMPHASIS_REACHED;

		dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00103 + lane, 0x1, &val);
	}

	dptx_hal_phy_set_swing_preemphasis(mtk_dp, lane_count, swing_val, preemphasis);
}

static void dptx_training_changemode(struct mtk_dp *mtk_dp)
{
	dptx_hal_phyd_reset(mtk_dp);
	dptx_hal_swing_emp_reset(mtk_dp);

	mdelay(2);
}

static void mtk_edp_train_setting(struct mtk_dp *mtk_dp,
				  u8 linkrate, u8 lanecount)
{
	u8 lanecount_enhanced_frame;
	u8 dpcd_data;

	lanecount_enhanced_frame = lanecount | DP_LANE_COUNT_ENHANCED_FRAME_EN;

	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00100, 0x1, &linkrate);
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00101, 0x1,
			   &lanecount_enhanced_frame);
	if (mtk_dp->train_info.sink_ssc_en) {
		dpcd_data = DP_SPREAD_AMP_0_5;
		dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00107, 0x1, &dpcd_data);
		dptx_hal_ssc_en(mtk_dp, true);
	} else {
		dpcd_data = 0x0;
		dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00107, 0x1, &dpcd_data);
		dptx_hal_ssc_en(mtk_dp, false);
	}
	dptx_hal_set_txlane(mtk_dp, lanecount / 2);
	dptx_hal_phy_setting(mtk_dp);

	printk(BIOS_INFO, "Link train linkrate = %#x, lanecount = %#x\n",
	       linkrate, lanecount);
}

static void drm_dp_dpcd_read_link_status(struct mtk_dp *mtk_dp, u8 status[DP_LINK_STATUS_SIZE])
{
	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ, DPCD_00202, DP_LINK_STATUS_SIZE, status);
}

static int mtk_edp_train_cr(struct mtk_dp *mtk_dp, u8 lane_count)
{
	u8 lane_adjust[DP_LANSE_ADJUST_SIZE];
	u8 link_status[DP_LINK_STATUS_SIZE];
	u8 prev_lane_adjust = 0xff;
	int train_retries = 0;
	int voltage_retries = 0;
	u8 buff;

	mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_TPS1);

	/* In DP spec 1.4, the retry count of CR is defined as 10. */
	do {
		train_retries++;
		if (!dptx_hal_hpd_high(mtk_dp)) {
			mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_UNKNOWN);
			return DPTX_TRANING_FAIL;
		}

		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ, DPCD_00206, sizeof(lane_adjust),
				  lane_adjust);
		printk(BIOS_DEBUG, "[aux_read] dpcd206 lane_adjust[0]=%#x\n", lane_adjust[0]);
		printk(BIOS_DEBUG, "[aux_read] dpcd207 lane_adjust[1]=%#x\n", lane_adjust[1]);
		update_swing_preemphasis(mtk_dp, lane_count, lane_adjust);

		dptx_link_train_clock_recovery_delay(mtk_dp->rx_cap);

		/* Check link status from sink device */
		drm_dp_dpcd_read_link_status(mtk_dp, link_status);
		if (dptx_clock_recovery_ok(link_status, lane_count)) {
			printk(BIOS_INFO, "Link train CR pass\n");
			return 0;
		}

		/*
		 * In DP spec 1.4, if current voltage level is the same
		 * with previous voltage level, we need to retry 5 times.
		 */
		if (prev_lane_adjust == link_status[4]) {
			voltage_retries++;
			/*
			 * Condition of CR fail:
			 * 1. Failed to pass CR using the same voltage
			 *    level over five times.
			 * 2. Failed to pass CR when the current voltage
			 *    level is the same with previous voltage
			 *    level and reach max voltage level (3).
			 */
			if (voltage_retries > MTK_DP_TRAIN_VOLTAGE_LEVEL_RETRY ||
			    (prev_lane_adjust & DP_ADJUST_VOLTAGE_SWING_LANE0_MASK) == 3) {
				printk(BIOS_ERR, "Link train CR fail\n");
				break;
			}
		} else {
			/*
			 * If the voltage level is changed, we need to
			 * re-calculate this retry count.
			 */
			voltage_retries = 0;
		}
		prev_lane_adjust = link_status[4];
		printk(BIOS_INFO, "CR training retries %d\n", voltage_retries);
	} while (train_retries < MTK_DP_TRAIN_DOWNSCALE_RETRY);

	/* Failed to train CR, and disable pattern. */
	buff = DP_TRAINING_PATTERN_DISABLE;
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00102, 0x1, &buff);
	mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_UNKNOWN);

	return DPTX_TRANING_FAIL;
}

static int mtk_edp_train_eq(struct mtk_dp *mtk_dp, u8 lane_count)
{
	u8 lane_adjust[2];
	u8 link_status[DP_LINK_STATUS_SIZE];
	int train_retries = 0;
	u8 buff;

	if (mtk_dp->train_info.tps4)
		mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_TPS4);
	else if (mtk_dp->train_info.tps3)
		mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_TPS3);
	else
		mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_TPS2);

	do {
		train_retries++;
		if (!dptx_hal_hpd_high(mtk_dp)) {
			mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_UNKNOWN);
			return DPTX_TRANING_FAIL;
		}

		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ, DPCD_00206, sizeof(lane_adjust),
				  lane_adjust);
		printk(BIOS_DEBUG, "[aux_read] dpcd206 lane_adjust[0]=%#x\n", lane_adjust[0]);
		printk(BIOS_DEBUG, "[aux_read] dpcd207 lane_adjust[1]=%#x\n", lane_adjust[1]);
		update_swing_preemphasis(mtk_dp, lane_count, lane_adjust);

		dptx_link_train_channel_eq_delay(mtk_dp->rx_cap);

		/* check link status from sink device */
		drm_dp_dpcd_read_link_status(mtk_dp, link_status);
		if (dptx_channel_eq_ok(link_status, lane_count)) {
			printk(BIOS_DEBUG, "Link train EQ pass\n");

			/* Training done, and disable pattern. */
			buff = DP_TRAINING_PATTERN_DISABLE;
			dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00102, 0x1, &buff);
			mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_UNKNOWN);
			return 0;
		}
		printk(BIOS_INFO, "Link train EQ fail retry:%d\n", train_retries);
	} while (train_retries < MTK_DP_TRAIN_DOWNSCALE_RETRY);

	/* Failed to train EQ, and disable pattern. */
	buff = DP_TRAINING_PATTERN_DISABLE;
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00102, 0x1, &buff);
	mtk_edp_pattern(mtk_dp, lane_count, DPTX_PATTERN_UNKNOWN);

	return DPTX_TIMEOUT;
}

static int dptx_set_trainingstart(struct mtk_dp *mtk_dp)
{
	u8 lanecount;
	u8 linkrate;
	u8 buffer;
	u8 train_limit;
	u8 max_linkrate;
	int ret;

	buffer = 0x1;
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00600, 0x1, &buffer);

	linkrate = mtk_dp->rx_cap[1];
	lanecount = mtk_dp->rx_cap[2] & 0x1F;

	printk(BIOS_INFO, "RX support linkrate = %#x, lanecount = %#x\n", linkrate, lanecount);

	if (mtk_dp->train_info.sink_extcap_en)
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ, DPCD_02002, 0x1, &buffer);
	else
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ, DPCD_00200, 0x1, &buffer);

	if ((buffer & 0xBF) != 0)
		mtk_dp->train_info.sink_count_num = buffer & 0xBF;

	dptx_hal_setscramble(mtk_dp, false);
	switch (linkrate) {
	case DP_LINKRATE_RBR:
	case DP_LINKRATE_HBR:
	case DP_LINKRATE_HBR2:
	case DP_LINKRATE_HBR25:
	case DP_LINKRATE_HBR3:
		break;
	default:
		mtk_dp->train_info.linkrate = DP_LINKRATE_HBR3;
		break;
	};

	max_linkrate = linkrate;
	printk(BIOS_INFO, "[eDPTX] linkrate = %#x, lanecount = %#x\n", linkrate, lanecount);
	for (train_limit = 6; train_limit > 0; train_limit--) {
		dptx_training_changemode(mtk_dp);

		mtk_edp_train_setting(mtk_dp, linkrate, lanecount);
		ret = mtk_edp_train_cr(mtk_dp, lanecount);
		if (ret == DPTX_TRANING_FAIL) {
			return ret;
		} else if (ret) {
			/* Reduce link rate */
			switch (linkrate) {
			case DP_LINKRATE_RBR:
				lanecount = lanecount / 2;
				linkrate = max_linkrate;

				if (lanecount == 0)
					return DPTX_TRANING_FAIL;
				break;
			case DP_LINKRATE_HBR:
				linkrate = DP_LINKRATE_RBR;
				break;
			case DP_LINKRATE_HBR2:
				linkrate = DP_LINKRATE_HBR;
				break;
			case DP_LINKRATE_HBR3:
				linkrate = DP_LINKRATE_HBR2;
				break;
			default:
				return DPTX_TRANING_FAIL;
			}
			continue;
		}

		ret = mtk_edp_train_eq(mtk_dp, lanecount);
		if (ret == DPTX_TIMEOUT) {
			return DPTX_TIMEOUT;
		} else if (ret) {
			/* Reduce lane count */
			if (lanecount == DP_LANECOUNT_4)
				lanecount = DP_LANECOUNT_2;
			else if (lanecount == DP_LANECOUNT_2)
				lanecount = DP_LANECOUNT_1;
			else
				return DPTX_TRANING_FAIL;
			continue;
		}
		printk(BIOS_INFO, "[eDPTX] EQ training pass\n");
		/* If we reach here, training is done. */
		break;
	}

	if (train_limit == 0)
		return DPTX_TRANING_FAIL;

	mtk_dp->train_info.linkrate = linkrate;
	mtk_dp->train_info.linklane_count = lanecount;
	/*
	 * After training done, we need to output normal stream instead of TPS,
	 * so we need to enable scramble.
	 */
	dptx_hal_setscramble(mtk_dp, true);
	dptx_hal_set_ef_mode(mtk_dp, ENABLE_DPTX_EF_MODE);

	printk(BIOS_INFO, "%s: done\n", __func__);

	return DPTX_PASS;
}

static void dptx_init_port(struct mtk_dp *mtk_dp)
{
	dptx_hal_phy_setidlepattern(mtk_dp, true);
	dptx_hal_init_setting(mtk_dp);
	dptx_hal_aux_setting(mtk_dp);
	dptx_hal_digital_setting(mtk_dp);
	dptx_hal_phy_init(mtk_dp);
	dptx_hal_phy_setting(mtk_dp);
	dptx_hal_hpd_detect_setting(mtk_dp);

	dptx_hal_digital_swreset(mtk_dp);

	dptx_hal_analog_power_en(mtk_dp, true);
	dptx_hal_hpd_int_en(mtk_dp, true);
}

int mtk_edp_init(struct mtk_dp *mtk_dp, struct edid *edid)
{
	write32p(EDP_CLK_BASE + DISP_EDPTX_PWR_CON, 0xC2FC224D);
	/* Add 26Mhz clock */
	write32p(CKSYS_GP2_BASE + CKSYS2_CLK_CFG_3_CLR, 0xFF000000);
	write32p(CKSYS_GP2_BASE + CKSYS2_CLK_CFG_UPDATE, 0x00008000);
	udelay(50);
	dptx_init_variable(mtk_dp);
	dptx_init_port(mtk_dp);

	if (!dptx_hal_hpd_high(mtk_dp)) {
		printk(BIOS_ERR, "HPD is low\n");
		return -1;
	}

	dptx_check_sinkcap(mtk_dp);

	if (dptx_get_edid(mtk_dp, edid) != 0) {
		printk(BIOS_ERR, "Failed to get EDID\n");
		return -1;
	}

	if (dptx_set_trainingstart(mtk_dp) != DPTX_PASS) {
		printk(BIOS_ERR, "%s: Failed to set training start\n", __func__);
		return -1;
	}

	dp_intf_config(edid);
	dptx_video_config(mtk_dp);

	return 0;
}


int mtk_edp_enable(struct mtk_dp *mtk_dp)
{
	if (!mtk_dp) {
		printk(BIOS_ERR, "%s: eDP is not initialized\n", __func__);
		return -1;
	}
	dptx_video_enable(mtk_dp, true);
	return 0;
}
