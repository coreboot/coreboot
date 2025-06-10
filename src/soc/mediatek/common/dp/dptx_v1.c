/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/dptx.h>
#include <soc/dptx_hal.h>
#include <soc/dptx_reg.h>
#include <soc/dp_intf.h>
#include <string.h>

static bool dptx_training_checkswingpre(struct mtk_dp *mtk_dp,
					u8 target_lane_count,
					const u8 *dpcp202_x, u8 *dpcp_buf)
{
	bool ret = true;
	u8 swing_value, preemphasis;

	/* Lane 0 */
	if (target_lane_count >= 0x1) {
		swing_value = (dpcp202_x[4] & 0x3);
		preemphasis = (dpcp202_x[4] & 0xc) >> 2;

		/* Adjust the swing and pre-emphasis */
		ret &= dptx_hal_setswing_preemphasis(mtk_dp, DPTX_LANE0,
						     swing_value, preemphasis);

		/*
		 * Adjust the swing and pre-emphasis done,
		 * and notify sink side.
		 */
		dpcp_buf[0] = swing_value | (preemphasis << 3);

		/* Max swing reached. */
		if (swing_value == DPTX_SWING3)
			dpcp_buf[0] |= BIT(2);

		/* Max pre-emphasis reached. */
		if (preemphasis == DPTX_PREEMPHASIS3)
			dpcp_buf[0] |= BIT(5);
	}

	/* Lane 1 */
	if (target_lane_count >= 0x2) {
		swing_value = (dpcp202_x[4] & 0x30) >> 4;
		preemphasis = (dpcp202_x[4] & 0xc0) >> 6;

		/* Adjust the swing and pre-emphasis */
		ret &= dptx_hal_setswing_preemphasis(mtk_dp, DPTX_LANE1,
						     swing_value, preemphasis);

		/*
		 * Adjust the swing and pre-emphasis done,
		 * and notify sink side.
		 */
		dpcp_buf[1] = swing_value | (preemphasis << 3);

		/* Max swing reached. */
		if (swing_value == DPTX_SWING3)
			dpcp_buf[1] |= BIT(2);

		/* Max pre-emphasis reached. */
		if (preemphasis == DPTX_PREEMPHASIS3)
			dpcp_buf[1] |= BIT(5);
	}

	/* Lane 2 and Lane 3 */
	if (target_lane_count == 0x4) {
		/* Lane 2 */
		swing_value = (dpcp202_x[5] & 0x3);
		preemphasis = (dpcp202_x[5] & 0x0c) >> 2;

		/* Adjust the swing and pre-emphasis */
		ret &= dptx_hal_setswing_preemphasis(mtk_dp, DPTX_LANE2,
						     swing_value, preemphasis);

		/*
		 * Adjust the swing and pre-emphasis done,
		 * and notify sink side.
		 */
		dpcp_buf[2] = swing_value | (preemphasis << 3);

		/* Max swing reached. */
		if (swing_value == DPTX_SWING3)
			dpcp_buf[2] |= BIT(2);

		/* Max pre-emphasis reached. */
		if (preemphasis == DPTX_PREEMPHASIS3)
			dpcp_buf[2] |= BIT(5);

		/* Lane 3 */
		swing_value = (dpcp202_x[5] & 0x30) >> 4;
		preemphasis = (dpcp202_x[5] & 0xc0) >> 6;

		/* Adjust the swing and pre-emphasis */
		ret &= dptx_hal_setswing_preemphasis(mtk_dp, DPTX_LANE3,
						     swing_value, preemphasis);

		/*
		 * Adjust the swing and pre-emphasis done,
		 * and notify sink side.
		 */
		dpcp_buf[0x3] = swing_value | (preemphasis << 3);

		/* Max swing reached. */
		if (swing_value == DPTX_SWING3)
			dpcp_buf[3] |= BIT(2);

		/* Max pre-emphasis reached. */
		if (preemphasis == DPTX_PREEMPHASIS3)
			dpcp_buf[3] |= BIT(5);
	}

	/* Wait signal stable enough */
	mdelay(2);

	return ret;
}

static int dptx_train_tps1(struct mtk_dp *mtk_dp, u8 target_lanecount,
			   int *status_ctrl, int *iteration, u8 *dpcp_buffer,
			   u8 *dpcd206)
{
	u8 tmp_val[6];
	u8 dpcd200c[3];

	printk(BIOS_INFO, "CR Training START\n");
	dptx_hal_setscramble(mtk_dp, false);

	if (*status_ctrl == 0x0) {
		dptx_hal_set_txtrainingpattern(mtk_dp, BIT(4));
		*status_ctrl = 0x1;
		tmp_val[0] = 0x21;
		dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
				   DPCD_00102, 0x1, tmp_val);
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
				  DPCD_00206, 0x2, tmp_val + 4);
		*iteration = *iteration + 1;

		dptx_training_checkswingpre(mtk_dp, target_lanecount,
					    tmp_val, dpcp_buffer);
	}

	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
			   DPCD_00103, target_lanecount, dpcp_buffer);

	dptx_link_train_clock_recovery_delay(mtk_dp->rx_cap);

	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
			  DPCD_00202, 0x6, tmp_val);

	if (mtk_dp->train_info.sink_extcap_en) {
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
				  DPCD_0200C, 0x3, dpcd200c);
		tmp_val[0] = dpcd200c[0];
		tmp_val[1] = dpcd200c[1];
		tmp_val[2] = dpcd200c[2];
	}

	if (dptx_clock_recovery_ok(tmp_val, target_lanecount)) {
		printk(BIOS_INFO, "CR Training Success\n");

		mtk_dp->train_info.cr_done = true;
		*iteration = 0x1;

		return TRAIN_STEP_SUCCESS;
	}

	printk(BIOS_INFO, "CR Training Fail\n");

	/* Requested swing and emp is the same with last time. */
	if (*dpcd206 == tmp_val[4]) {
		*iteration = *iteration + 1;
		if (*dpcd206 & 0x3)
			return TRAIN_STEP_FAIL_BREAK;
	} else {
		*dpcd206 = tmp_val[4];
	}

	return TRAIN_STEP_FAIL_NOT_BREAK;
}

static int dptx_train_tps2_3(struct mtk_dp *mtk_dp, u8 target_lanecount,
			     int *status_ctrl, int *iteration, u8 *dpcp_buffer,
			     u8 *dpcd206)
{
	u8 tmp_val[6];
	u8 dpcd200c[3];

	printk(BIOS_INFO, "EQ Training START\n");

	if (*status_ctrl == 0x1) {
		if (mtk_dp->train_info.tps4)
			dptx_hal_set_txtrainingpattern(mtk_dp, BIT(7));
		else if (mtk_dp->train_info.tps3)
			dptx_hal_set_txtrainingpattern(mtk_dp, BIT(6));
		else
			dptx_hal_set_txtrainingpattern(mtk_dp, BIT(5));

		if (mtk_dp->train_info.tps4) {
			tmp_val[0] = 0x07;
			dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
					   DPCD_00102, 0x1, tmp_val);
		} else if (mtk_dp->train_info.tps3) {
			tmp_val[0] = 0x23;
			dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
					   DPCD_00102, 0x1, tmp_val);
		} else {
			tmp_val[0] = 0x22;
			dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
					   DPCD_00102, 0x1, tmp_val);
		}

		*status_ctrl = 0x2;
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
				  DPCD_00206, 0x2, tmp_val + 4);

		*iteration = *iteration + 1;
		dptx_training_checkswingpre(mtk_dp, target_lanecount,
					    tmp_val, dpcp_buffer);
	}

	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE, DPCD_00103,
			   target_lanecount, dpcp_buffer);
	dptx_link_train_channel_eq_delay(mtk_dp->rx_cap);

	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
			  DPCD_00202, 0x6, tmp_val);

	if (mtk_dp->train_info.sink_extcap_en) {
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
				  DPCD_0200C, 0x3, dpcd200c);
		tmp_val[0] |= dpcd200c[0];
		tmp_val[1] |= dpcd200c[1];
		tmp_val[2] |= dpcd200c[2];
	}

	if (!dptx_clock_recovery_ok(tmp_val, target_lanecount)) {
		printk(BIOS_INFO, "EQ Training Fail\n");
		mtk_dp->train_info.cr_done = false;
		mtk_dp->train_info.eq_done = false;
		return TRAIN_STEP_FAIL_BREAK;
	}

	if (dptx_channel_eq_ok(tmp_val, target_lanecount)) {
		printk(BIOS_INFO, "EQ Training Success\n");
		mtk_dp->train_info.eq_done = true;
		return TRAIN_STEP_SUCCESS;
	}
	printk(BIOS_INFO, "EQ Training Fail\n");

	if (*dpcd206 == tmp_val[4])
		*iteration = *iteration + 1;
	else
		*dpcd206 = tmp_val[4];

	return TRAIN_STEP_FAIL_NOT_BREAK;
}

static int dptx_trainingflow(struct mtk_dp *mtk_dp,
			     u8 lanerate, u8 lanecount)
{
	u8 tmp_val[6];
	u8 target_linkrate = lanerate;
	u8 target_lanecount = lanecount;
	u8 dpcp_buffer[4];
	u8 dpcd206;
	bool pass_tps1 = false;
	bool pass_tps2_3 = false;
	int train_retry, status_ctrl, iteration;

	memset(tmp_val, 0, sizeof(tmp_val));
	memset(dpcp_buffer, 0, sizeof(dpcp_buffer));

	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
			  DPCD_00600, 0x1, tmp_val);
	if (tmp_val[0] != 0x1) {
		tmp_val[0] = 0x1;
		dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
				   DPCD_00600, 0x1, tmp_val);
		mdelay(1);
	}

	tmp_val[0] = target_linkrate;
	tmp_val[1] = target_lanecount | DPTX_AUX_SET_ENAHNCED_FRAME;
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
			   DPCD_00100, 0x2, tmp_val);

	if (mtk_dp->train_info.sink_ssc_en) {
		tmp_val[0x0] = 0x10;
		dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
				   DPCD_00107, 0x1, tmp_val);
	}

	train_retry = 0x0;
	status_ctrl = 0x0;
	iteration = 0x1;
	dpcd206 = 0xff;

	dptx_hal_set_txlane(mtk_dp, target_lanecount / 2);
	dptx_hal_set_txrate(mtk_dp, target_linkrate);

	do {
		train_retry++;
		if (!pass_tps1) {
			int ret = dptx_train_tps1(mtk_dp, target_lanecount,
						  &status_ctrl, &iteration,
						  dpcp_buffer, &dpcd206);
			if (ret == TRAIN_STEP_FAIL_BREAK)
				break;
			if (ret == TRAIN_STEP_SUCCESS) {
				pass_tps1 = true;
				train_retry = 0;
			}
		} else {
			int ret = dptx_train_tps2_3(mtk_dp, target_lanecount,
						    &status_ctrl, &iteration,
						    dpcp_buffer, &dpcd206);
			if (ret == TRAIN_STEP_FAIL_BREAK)
				break;
			if (ret == TRAIN_STEP_SUCCESS) {
				pass_tps2_3 = true;
				break;
			}
		}

		dptx_training_checkswingpre(mtk_dp, target_lanecount,
					    tmp_val, dpcp_buffer);

	} while (train_retry < DPTX_TRAIN_RETRY_LIMIT &&
		 iteration < DPTX_TRAIN_MAX_ITERATION);

	tmp_val[0] = 0x0;
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
			   DPCD_00102, 0x1, tmp_val);
	dptx_hal_set_txtrainingpattern(mtk_dp, 0);

	if (!pass_tps2_3) {
		printk(BIOS_ERR, "Link Training Fail\n");
		return DPTX_TRANING_FAIL;
	}

	mtk_dp->train_info.linkrate = target_linkrate;
	mtk_dp->train_info.linklane_count = target_lanecount;

	dptx_hal_setscramble(mtk_dp, true);

	tmp_val[0] = target_lanecount | DPTX_AUX_SET_ENAHNCED_FRAME;

	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
			   DPCD_00101, 0x1, tmp_val);
	dptx_hal_set_ef_mode(mtk_dp, ENABLE_DPTX_EF_MODE);

	printk(BIOS_INFO, "Link Training Success\n");
	return DPTX_PASS;
}

static void dptx_training_changemode(struct mtk_dp *mtk_dp)
{
	dptx_hal_phyd_reset(mtk_dp);
	dptx_hal_reset_swing_preemphasis(mtk_dp);
	dptx_hal_ssc_en(mtk_dp, mtk_dp->train_info.sink_ssc_en);

	mdelay(2);
}

static int dptx_set_trainingstart(struct mtk_dp *mtk_dp)
{
	u8 lanecount;
	u8 linkrate;
	u8 buffer;
	u8 limit;
	u8 max_linkrate;

	buffer = 0x1;
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
			   DPCD_00600, 0x1, &buffer);

	linkrate = mtk_dp->rx_cap[1];
	lanecount = mtk_dp->rx_cap[2] & 0x1f;

	printk(BIOS_INFO, "RX support linkrate = %#x, lanecount = %#x\n",
	       linkrate, lanecount);

	mtk_dp->train_info.linkrate =
		(linkrate >= mtk_dp->train_info.sys_max_linkrate) ?
		mtk_dp->train_info.sys_max_linkrate : linkrate;
	mtk_dp->train_info.linklane_count = (lanecount >= MAX_LANECOUNT) ?
					    MAX_LANECOUNT : lanecount;

	if (mtk_dp->train_info.sink_extcap_en)
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
				  DPCD_02002, 0x1, &buffer);
	else
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
				  DPCD_00200, 0x1, &buffer);

	if ((buffer & 0xbf) != 0)
		mtk_dp->train_info.sink_count_num = buffer & 0xbf;

	linkrate = mtk_dp->train_info.linkrate;
	lanecount = mtk_dp->train_info.linklane_count;

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
	limit = 0x6;

	do {
		mtk_dp->train_info.cr_done = false;
		mtk_dp->train_info.eq_done = false;

		dptx_training_changemode(mtk_dp);
		dptx_trainingflow(mtk_dp, linkrate, lanecount);

		if (!mtk_dp->train_info.cr_done) {
			/* CR fail and reduce link capability. */
			switch (linkrate) {
			case DP_LINKRATE_RBR:
				lanecount = lanecount / 2;
				linkrate = max_linkrate;

				if (lanecount == 0x0)
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
			};
		} else if (!mtk_dp->train_info.eq_done) {
			/* EQ fail and reduce lane counts. */
			if (lanecount == DP_LANECOUNT_4)
				lanecount = DP_LANECOUNT_2;
			else if (lanecount == DP_LANECOUNT_2)
				lanecount = DP_LANECOUNT_1;
			else
				return DPTX_TRANING_FAIL;
		} else {
			return DPTX_PASS;
		}
	} while (--limit > 0);

	return DPTX_TRANING_FAIL;
}

static void dptx_init_port(struct mtk_dp *mtk_dp)
{
	dptx_hal_phy_setidlepattern(mtk_dp, true);
	dptx_hal_init_setting(mtk_dp);
	dptx_hal_aux_setting(mtk_dp);
	dptx_hal_digital_setting(mtk_dp);
	dptx_hal_phy_setting(mtk_dp);
	dptx_hal_hpd_detect_setting(mtk_dp);

	dptx_hal_digital_swreset(mtk_dp);
	dptx_hal_analog_power_en(mtk_dp, true);
	dptx_hal_hpd_int_en(mtk_dp, true);
}

int mtk_edp_init(struct mtk_dp *mtk_dp, struct edid *edid)
{
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

	dptx_set_trainingstart(mtk_dp);
	dp_intf_config(edid);
	dptx_video_config(mtk_dp);

	return 0;
}

int mtk_edp_enable(struct mtk_dp *mtk_dp)
{
	dptx_video_enable(mtk_dp, true);
	return 0;
}
