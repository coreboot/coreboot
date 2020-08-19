/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <soc/dp.h>
#include <soc/fimd.h>
#include <soc/i2c.h>
#include <soc/power.h>
#include <soc/sysreg.h>
#include <string.h>

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

static int exynos_dp_init_dp(void)
{
	int ret;
	exynos_dp_reset();

	/* SW defined function Normal operation */
	exynos_dp_enable_sw_func(DP_ENABLE);

	ret = exynos_dp_init_analog_func();
	if (ret != EXYNOS_DP_SUCCESS)
		return ret;

	exynos_dp_init_hpd();
	exynos_dp_init_aux();

	return ret;
}

static unsigned char exynos_dp_calc_edid_check_sum(unsigned char *edid_data)
{
	int i;
	unsigned char sum = 0;

	for (i = 0; i < EDID_BLOCK_LENGTH; i++)
		sum = sum + edid_data[i];

	return sum;
}

static unsigned int exynos_dp_read_edid(void)
{
	unsigned char edid[EDID_BLOCK_LENGTH * 2];
	unsigned int extend_block = 0;
	unsigned char sum;
	unsigned char test_vector;
	int retval = 0;

	/*
	 * EDID device address is 0x50.
	 * However, if necessary, you must have set upper address
	 * into E-EDID in I2C device, 0x30.
	 */

	/* Read Extension Flag, Number of 128-byte EDID extension blocks */
	if (exynos_dp_read_byte_from_i2c
	    (I2C_EDID_DEVICE_ADDR, EDID_EXTENSION_FLAG, &extend_block))
		return -1;

	if (extend_block > 0) {
		/* Read EDID data */
		retval = exynos_dp_read_bytes_from_i2c(I2C_EDID_DEVICE_ADDR,
						EDID_HEADER_PATTERN,
						EDID_BLOCK_LENGTH,
						&edid[EDID_HEADER_PATTERN]);

		if (retval != 0) {
			printk(BIOS_ERR, "DP EDID Read failed!\n");
			return -1;
		}
		sum = exynos_dp_calc_edid_check_sum(edid);
		if (sum != 0) {
			printk(BIOS_ERR, "DP EDID bad checksum!\n");
			return -1;
		}
		/* Read additional EDID data */
		retval = exynos_dp_read_bytes_from_i2c(I2C_EDID_DEVICE_ADDR,
				EDID_BLOCK_LENGTH,
				EDID_BLOCK_LENGTH,
				&edid[EDID_BLOCK_LENGTH]);
		if (retval != 0) {
			printk(BIOS_ERR, "DP EDID Read failed!\n");
			return -1;
		}
		sum = exynos_dp_calc_edid_check_sum(&edid[EDID_BLOCK_LENGTH]);
		if (sum != 0) {
			printk(BIOS_ERR, "DP EDID bad checksum!\n");
			return -1;
		}
		exynos_dp_read_byte_from_dpcd(DPCD_TEST_REQUEST,
					&test_vector);
		if (test_vector & DPCD_TEST_EDID_READ) {
			exynos_dp_write_byte_to_dpcd(DPCD_TEST_EDID_CHECKSUM,
				edid[EDID_BLOCK_LENGTH + EDID_CHECKSUM]);
			exynos_dp_write_byte_to_dpcd(DPCD_TEST_RESPONSE,
				DPCD_TEST_EDID_CHECKSUM_WRITE);
		}
	} else {
		/* Read EDID data */
		retval = exynos_dp_read_bytes_from_i2c(I2C_EDID_DEVICE_ADDR,
				EDID_HEADER_PATTERN,
				EDID_BLOCK_LENGTH,
				&edid[EDID_HEADER_PATTERN]);

		if (retval != 0) {
			printk(BIOS_ERR, "DP EDID Read failed!\n");
			return -1;
		}
		sum = exynos_dp_calc_edid_check_sum(edid);
		if (sum != 0) {
			printk(BIOS_ERR, "DP EDID bad checksum!\n");
			return -1;
		}

		exynos_dp_read_byte_from_dpcd(DPCD_TEST_REQUEST,
			&test_vector);
		if (test_vector & DPCD_TEST_EDID_READ) {
			exynos_dp_write_byte_to_dpcd(DPCD_TEST_EDID_CHECKSUM,
				edid[EDID_CHECKSUM]);
			exynos_dp_write_byte_to_dpcd(DPCD_TEST_RESPONSE,
				DPCD_TEST_EDID_CHECKSUM_WRITE);
		}

	}

	return 0;
}

static unsigned int exynos_dp_handle_edid(struct edp_device_info *edp_info)
{
	unsigned char buf[12];
	unsigned int ret;
	unsigned char temp;
	unsigned char retry_cnt;
	unsigned char dpcd_rev[16];
	unsigned char lane_bw[16];
	unsigned char lane_cnt[16];

	memset(dpcd_rev, 0, sizeof(dpcd_rev));
	memset(lane_bw, 0, sizeof(lane_bw));
	memset(lane_cnt, 0, sizeof(lane_cnt));
	memset(buf, 0, sizeof(buf));

	retry_cnt = 5;
	while (retry_cnt) {
		/* Read DPCD 0x0000-0x000b */
		ret = exynos_dp_read_bytes_from_dpcd(DPCD_DPCD_REV, 12,
				buf);
		if (ret != EXYNOS_DP_SUCCESS) {
			if (retry_cnt == 0) {
				printk(BIOS_ERR, "DP read_byte_from_dpcd() failed\n");
				return ret;
			}
			retry_cnt--;
		} else
			break;
	}
	/* */
	temp = buf[DPCD_DPCD_REV];
	if (temp == DP_DPCD_REV_10 || temp == DP_DPCD_REV_11)
		edp_info->dpcd_rev = temp;
	else {
		printk(BIOS_ERR, "DP Wrong DPCD Rev : %x\n", temp);
		return -1;
	}
	temp = buf[DPCD_MAX_LINK_RATE];
	if (temp == DP_LANE_BW_1_62 || temp == DP_LANE_BW_2_70)
		edp_info->lane_bw = temp;
	else {
		printk(BIOS_ERR, "DP Wrong MAX LINK RATE : %x\n", temp);
		return -1;
	}
	/*Refer VESA Display Port Standard Ver1.1a Page 120 */
	if (edp_info->dpcd_rev == DP_DPCD_REV_11) {
		temp = buf[DPCD_MAX_LANE_COUNT] & 0x1f;
		if (buf[DPCD_MAX_LANE_COUNT] & 0x80)
			edp_info->dpcd_efc = 1;
		else
			edp_info->dpcd_efc = 0;
	} else {
		temp = buf[DPCD_MAX_LANE_COUNT];
		edp_info->dpcd_efc = 0;
	}

	if (temp == DP_LANE_CNT_1 || temp == DP_LANE_CNT_2 ||
			temp == DP_LANE_CNT_4) {
		edp_info->lane_cnt = temp;
	} else {
		printk(BIOS_ERR, "DP Wrong MAX LANE COUNT : %x\n", temp);
		return -1;
	}

	if (edp_info->raw_edid){
		ret = EXYNOS_DP_SUCCESS;
		printk(BIOS_SPEW, "EDID compiled in, skipping read\n");
	} else {
		ret = exynos_dp_read_edid();
		if (ret != EXYNOS_DP_SUCCESS) {
			printk(BIOS_ERR, "DP exynos_dp_read_edid() failed\n");
			return -1;
		}
	}

	return ret;
}

static void exynos_dp_init_training(void)
{
	/*
	 * MACRO_RST must be applied after the PLL_LOCK to avoid
	 * the DP inter pair skew issue for at least 10 us
	 */
	exynos_dp_reset_macro();

	/* All DP analog module power up */
	exynos_dp_set_analog_power_down(POWER_ALL, 0);
}

static unsigned int exynos_dp_link_start(struct edp_device_info *edp_info)
{
	unsigned char buf[5];
	unsigned int ret;

	edp_info->lt_info.lt_status = DP_LT_CR;
	edp_info->lt_info.ep_loop = 0;
	edp_info->lt_info.cr_loop[0] = 0;
	edp_info->lt_info.cr_loop[1] = 0;
	edp_info->lt_info.cr_loop[2] = 0;
	edp_info->lt_info.cr_loop[3] = 0;

	/* Set sink to D0 (Sink Not Ready) mode. */
	ret = exynos_dp_write_byte_to_dpcd(DPCD_SINK_POWER_STATE,
					   DPCD_SET_POWER_STATE_D0);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP write_dpcd_byte failed\n");
		return ret;
	}

	/* Set link rate and count as you want to establish*/
	exynos_dp_set_link_bandwidth(edp_info->lane_bw);
	exynos_dp_set_lane_count(edp_info->lane_cnt);

	/* Setup RX configuration */
	buf[0] = edp_info->lane_bw;
	buf[1] = edp_info->lane_cnt;

	ret = exynos_dp_write_bytes_to_dpcd(DPCD_LINK_BW_SET, 2,
			buf);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP write_dpcd_byte failed\n");
		return ret;
	}

	exynos_dp_set_lane_pre_emphasis(PRE_EMPHASIS_LEVEL_0,
			edp_info->lane_cnt);

	/* Set training pattern 1 */
	exynos_dp_set_training_pattern(TRAINING_PTN1);

	/* Set RX training pattern */
	buf[0] = DPCD_SCRAMBLING_DISABLED | DPCD_TRAINING_PATTERN_1;

	buf[1] = DPCD_PRE_EMPHASIS_SET_PATTERN_2_LEVEL_0 |
		DPCD_VOLTAGE_SWING_SET_PATTERN_1_LEVEL_0;
	buf[2] = DPCD_PRE_EMPHASIS_SET_PATTERN_2_LEVEL_0 |
		DPCD_VOLTAGE_SWING_SET_PATTERN_1_LEVEL_0;
	buf[3] = DPCD_PRE_EMPHASIS_SET_PATTERN_2_LEVEL_0 |
		DPCD_VOLTAGE_SWING_SET_PATTERN_1_LEVEL_0;
	buf[4] = DPCD_PRE_EMPHASIS_SET_PATTERN_2_LEVEL_0 |
		DPCD_VOLTAGE_SWING_SET_PATTERN_1_LEVEL_0;

	ret = exynos_dp_write_bytes_to_dpcd(DPCD_TRAINING_PATTERN_SET,
			5, buf);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP write_dpcd_byte failed\n");
		return ret;
	}
	return ret;
}

static unsigned int exynos_dp_training_pattern_dis(void)
{
	unsigned int ret;

	exynos_dp_set_training_pattern(DP_NONE);

	ret = exynos_dp_write_byte_to_dpcd(DPCD_TRAINING_PATTERN_SET,
			DPCD_TRAINING_PATTERN_DISABLED);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP requst_link_traninig_req failed\n");
		return -1;
	}

	return ret;
}

static unsigned int exynos_dp_enable_rx_to_enhanced_mode(unsigned char enable)
{
	unsigned char data;
	unsigned int ret;

	ret = exynos_dp_read_byte_from_dpcd(DPCD_LANE_COUNT_SET,
			&data);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP read_from_dpcd failed\n");
		return -1;
	}

	if (enable)
		data = DPCD_ENHANCED_FRAME_EN | DPCD_LN_COUNT_SET(data);
	else
		data = DPCD_LN_COUNT_SET(data);

	ret = exynos_dp_write_byte_to_dpcd(DPCD_LANE_COUNT_SET,
			data);
	if (ret != EXYNOS_DP_SUCCESS) {
			printk(BIOS_ERR, "DP write_to_dpcd failed\n");
			return -1;

	}

	return ret;
}

static unsigned int exynos_dp_set_enhanced_mode(unsigned char enhance_mode)
{
	unsigned int ret;

	ret = exynos_dp_enable_rx_to_enhanced_mode(enhance_mode);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP rx_enhance_mode failed\n");
		return -1;
	}

	exynos_dp_enable_enhanced_mode(enhance_mode);

	return ret;
}

static int exynos_dp_read_dpcd_lane_stat(struct edp_device_info *edp_info,
		unsigned char *status)
{
	unsigned int ret, i;
	unsigned char buf[2];
	unsigned char lane_stat[DP_LANE_CNT_4] = {0,};
	const unsigned char shift_val[] = {0, 4, 0, 4};

	ret = exynos_dp_read_bytes_from_dpcd(DPCD_LANE0_1_STATUS, 2, buf);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP read lane status failed\n");
		return ret;
	}

	for (i = 0; i < edp_info->lane_cnt; i++) {
		lane_stat[i] = (buf[(i / 2)] >> shift_val[i]) & 0x0f;
		if (lane_stat[0] != lane_stat[i]) {
			printk(BIOS_ERR, "Wrong lane status\n");
			return -1;
		}
	}

	*status = lane_stat[0];

	return ret;
}

static unsigned int exynos_dp_read_dpcd_adj_req(unsigned char lane_num,
		unsigned char *sw, unsigned char *em)
{
	const unsigned char shift_val[] = {0, 4, 0, 4};
	unsigned int ret;
	unsigned char buf;
	unsigned int dpcd_addr;

	/*lane_num value is used as array index, so this range 0 ~ 3 */
	dpcd_addr = DPCD_ADJUST_REQUEST_LANE0_1 + (lane_num / 2);

	ret = exynos_dp_read_byte_from_dpcd(dpcd_addr, &buf);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP read adjust request failed\n");
		return -1;
	}

	*sw = ((buf >> shift_val[lane_num]) & 0x03);
	*em = ((buf >> shift_val[lane_num]) & 0x0c) >> 2;

	return ret;
}

static int exynos_dp_equalizer_err_link(struct edp_device_info *edp_info)
{
	int ret;

	ret = exynos_dp_training_pattern_dis();
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP training_patter_disable() failed\n");
		edp_info->lt_info.lt_status = DP_LT_FAIL;
	}

	ret = exynos_dp_set_enhanced_mode(edp_info->dpcd_efc);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP set_enhanced_mode() failed\n");
		edp_info->lt_info.lt_status = DP_LT_FAIL;
	}

	return ret;
}

static int exynos_dp_reduce_link_rate(struct edp_device_info *edp_info)
{
	int ret;

	if (edp_info->lane_bw == DP_LANE_BW_2_70) {
		edp_info->lane_bw = DP_LANE_BW_1_62;
		printk(BIOS_ERR, "DP Change lane bw to 1.62Gbps\n");
		edp_info->lt_info.lt_status = DP_LT_START;
		ret = EXYNOS_DP_SUCCESS;
	} else {
		ret = exynos_dp_training_pattern_dis();
		if (ret != EXYNOS_DP_SUCCESS)
			printk(BIOS_ERR, "DP training_patter_disable() failed\n");

		ret = exynos_dp_set_enhanced_mode(edp_info->dpcd_efc);
		if (ret != EXYNOS_DP_SUCCESS)
			printk(BIOS_ERR, "DP set_enhanced_mode() failed\n");

		edp_info->lt_info.lt_status = DP_LT_FAIL;
	}

	return ret;
}

static unsigned int exynos_dp_process_clock_recovery(struct edp_device_info
							*edp_info)
{
	unsigned int ret;
	unsigned char lane_stat;
	unsigned char lt_ctl_val[DP_LANE_CNT_4] = {0, };
	unsigned int i;
	unsigned char adj_req_sw;
	unsigned char adj_req_em;
	unsigned char buf[5];

	mdelay(1);

	ret = exynos_dp_read_dpcd_lane_stat(edp_info, &lane_stat);
	if (ret != EXYNOS_DP_SUCCESS) {
			printk(BIOS_ERR, "DP read lane status failed\n");
			edp_info->lt_info.lt_status = DP_LT_FAIL;
			return ret;
	}

	if (lane_stat & DP_LANE_STAT_CR_DONE) {
		printk(BIOS_DEBUG,"DP clock Recovery training succeed\n");
		exynos_dp_set_training_pattern(TRAINING_PTN2);

		for (i = 0; i < edp_info->lane_cnt; i++) {
			ret = exynos_dp_read_dpcd_adj_req(i, &adj_req_sw,
					&adj_req_em);
			if (ret != EXYNOS_DP_SUCCESS) {
				edp_info->lt_info.lt_status = DP_LT_FAIL;
				return ret;
			}

			lt_ctl_val[i] = 0;
			lt_ctl_val[i] = adj_req_em << 3 | adj_req_sw;

			if ((adj_req_sw == VOLTAGE_LEVEL_3)
				|| (adj_req_em == PRE_EMPHASIS_LEVEL_3)) {
				lt_ctl_val[i] |= MAX_DRIVE_CURRENT_REACH_3 |
					MAX_PRE_EMPHASIS_REACH_3;
			}
			exynos_dp_set_lanex_pre_emphasis(lt_ctl_val[i], i);
		}

		buf[0] =  DPCD_SCRAMBLING_DISABLED | DPCD_TRAINING_PATTERN_2;
		buf[1] = lt_ctl_val[0];
		buf[2] = lt_ctl_val[1];
		buf[3] = lt_ctl_val[2];
		buf[4] = lt_ctl_val[3];

		ret = exynos_dp_write_bytes_to_dpcd(
				DPCD_TRAINING_PATTERN_SET, 5, buf);
		if (ret != EXYNOS_DP_SUCCESS) {
			printk(BIOS_ERR, "DP write training pattern1 failed\n");
			edp_info->lt_info.lt_status = DP_LT_FAIL;
			return ret;
		} else
			edp_info->lt_info.lt_status = DP_LT_ET;
	} else {
		for (i = 0; i < edp_info->lane_cnt; i++) {
			lt_ctl_val[i] = exynos_dp_get_lanex_pre_emphasis(i);
				ret = exynos_dp_read_dpcd_adj_req(i,
						&adj_req_sw, &adj_req_em);
			if (ret != EXYNOS_DP_SUCCESS) {
				printk(BIOS_ERR, "DP read adj req failed\n");
				edp_info->lt_info.lt_status = DP_LT_FAIL;
				return ret;
			}

			if ((adj_req_sw == VOLTAGE_LEVEL_3) ||
					(adj_req_em == PRE_EMPHASIS_LEVEL_3))
				ret = exynos_dp_reduce_link_rate(edp_info);

			if ((DRIVE_CURRENT_SET_0_GET(lt_ctl_val[i]) ==
						adj_req_sw) &&
				(PRE_EMPHASIS_SET_0_GET(lt_ctl_val[i]) ==
						adj_req_em)) {
				edp_info->lt_info.cr_loop[i]++;
				if (edp_info->lt_info.cr_loop[i] == MAX_CR_LOOP)
					ret = exynos_dp_reduce_link_rate(
							edp_info);
			}

			lt_ctl_val[i] = 0;
			lt_ctl_val[i] = adj_req_em << 3 | adj_req_sw;

			if ((adj_req_sw == VOLTAGE_LEVEL_3) ||
					(adj_req_em == PRE_EMPHASIS_LEVEL_3)) {
				lt_ctl_val[i] |= MAX_DRIVE_CURRENT_REACH_3 |
					MAX_PRE_EMPHASIS_REACH_3;
			}
			exynos_dp_set_lanex_pre_emphasis(lt_ctl_val[i], i);
		}

		ret = exynos_dp_write_bytes_to_dpcd(
				DPCD_TRAINING_LANE0_SET, 4, lt_ctl_val);
		if (ret != EXYNOS_DP_SUCCESS) {
			printk(BIOS_ERR, "DP write training pattern2 failed\n");
			edp_info->lt_info.lt_status = DP_LT_FAIL;
			return ret;
		}
	}

	return ret;
}

static unsigned int exynos_dp_process_equalizer_training(struct edp_device_info
		*edp_info)
{
	unsigned int ret;
	unsigned char lane_stat, adj_req_sw, adj_req_em, i;
	unsigned char lt_ctl_val[DP_LANE_CNT_4] = {0,};
	unsigned char interlane_aligned = 0;
	unsigned char f_bw;
	unsigned char f_lane_cnt;
	unsigned char sink_stat;

	mdelay(1);

	ret = exynos_dp_read_dpcd_lane_stat(edp_info, &lane_stat);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP read lane status failed\n");
		edp_info->lt_info.lt_status = DP_LT_FAIL;
		return ret;
	}

	printk(BIOS_DEBUG,"DP lane stat : %x\n", lane_stat);

	if (lane_stat & DP_LANE_STAT_CR_DONE) {
		printk(BIOS_DEBUG, "DP_LANE_STAT_CR_DONE ok\n");
		ret = exynos_dp_read_byte_from_dpcd(DPCD_LN_ALIGN_UPDATED,
				&sink_stat);
		if (ret != EXYNOS_DP_SUCCESS) {
			edp_info->lt_info.lt_status = DP_LT_FAIL;
			printk(BIOS_ERR, "DP read DPCD_LN_ALIGN_UPDATED failed\n");
			return ret;
		}

		interlane_aligned = (sink_stat & DPCD_INTERLANE_ALIGN_DONE);
		printk(BIOS_DEBUG, "interlane_aligned: %d\n", interlane_aligned);
		printk(BIOS_DEBUG, "Check %d lanes\n", edp_info->lane_cnt);

		for (i = 0; i < edp_info->lane_cnt; i++) {
			ret = exynos_dp_read_dpcd_adj_req(i,
					&adj_req_sw, &adj_req_em);
			if (ret != EXYNOS_DP_SUCCESS) {
				printk(BIOS_ERR, "DP read adj req 1 failed\n");
				edp_info->lt_info.lt_status = DP_LT_FAIL;

				return ret;
			}

			lt_ctl_val[i] = 0;
			lt_ctl_val[i] = adj_req_em << 3 | adj_req_sw;

			if ((adj_req_sw == VOLTAGE_LEVEL_3) ||
				(adj_req_em == PRE_EMPHASIS_LEVEL_3)) {
				lt_ctl_val[i] |= MAX_DRIVE_CURRENT_REACH_3;
				lt_ctl_val[i] |= MAX_PRE_EMPHASIS_REACH_3;
			}
		}

		if (((lane_stat&DP_LANE_STAT_CE_DONE) &&
			(lane_stat&DP_LANE_STAT_SYM_LOCK))
			&& (interlane_aligned == DPCD_INTERLANE_ALIGN_DONE)) {
			printk(BIOS_DEBUG,"DP Equalizer training succeed\n");

			f_bw = exynos_dp_get_link_bandwidth();
			f_lane_cnt = exynos_dp_get_lane_count();

			printk(BIOS_DEBUG,"DP final BandWidth : %x\n", f_bw);
			printk(BIOS_DEBUG,"DP final Lane Count : %x\n", f_lane_cnt);

			edp_info->lt_info.lt_status = DP_LT_FINISHED;

			exynos_dp_equalizer_err_link(edp_info);

		} else {
			edp_info->lt_info.ep_loop++;

			if (edp_info->lt_info.ep_loop > MAX_EQ_LOOP) {
				if (edp_info->lane_bw == DP_LANE_BW_2_70) {
					ret = exynos_dp_reduce_link_rate(
							edp_info);
				} else {
					edp_info->lt_info.lt_status =
								DP_LT_FAIL;
					exynos_dp_equalizer_err_link(edp_info);
				}
			} else {
				for (i = 0; i < edp_info->lane_cnt; i++)
					exynos_dp_set_lanex_pre_emphasis(
							lt_ctl_val[i], i);

				ret = exynos_dp_write_bytes_to_dpcd(
					DPCD_TRAINING_LANE0_SET,
					4, lt_ctl_val);
				if (ret != EXYNOS_DP_SUCCESS) {
					printk(BIOS_ERR, "DP set lt pattern failed\n");
					edp_info->lt_info.lt_status =
								DP_LT_FAIL;
					exynos_dp_equalizer_err_link(edp_info);
				}
			}
		}
	} else if (edp_info->lane_bw == DP_LANE_BW_2_70) {
		ret = exynos_dp_reduce_link_rate(edp_info);
	} else {
		edp_info->lt_info.lt_status = DP_LT_FAIL;
		exynos_dp_equalizer_err_link(edp_info);
	}

	return ret;
}

static unsigned int exynos_dp_sw_link_training(struct edp_device_info *edp_info)
{
	/* the C compiler is almost smart enough to know this gets set.
	 * But not quite.
	 */
	unsigned int ret = 0;
	int training_finished;

	/* Turn off unnecessary lane */
	if (edp_info->lane_cnt == 1)
		exynos_dp_set_analog_power_down(CH1_BLOCK, 1);

	training_finished = 0;

	edp_info->lt_info.lt_status = DP_LT_START;

	/* Process here */
	while (!training_finished) {
		switch (edp_info->lt_info.lt_status) {
		case DP_LT_START:
			ret = exynos_dp_link_start(edp_info);
			if (ret != EXYNOS_DP_SUCCESS) {
				printk(BIOS_ERR, "DP LT:link start failed\n");
				training_finished = 1;
			}
			break;
		case DP_LT_CR:
			ret = exynos_dp_process_clock_recovery(edp_info);
			if (ret != EXYNOS_DP_SUCCESS) {
				printk(BIOS_ERR, "DP LT:clock recovery failed\n");
				training_finished = 1;
			}
			break;
		case DP_LT_ET:
			ret = exynos_dp_process_equalizer_training(edp_info);
			if (ret != EXYNOS_DP_SUCCESS) {
				printk(BIOS_ERR, "DP LT:equalizer training failed\n");
				training_finished = 1;
			}
			break;
		case DP_LT_FINISHED:
			training_finished = 1;
			break;
		case DP_LT_FAIL:
			printk(BIOS_ERR,"DP: %s: DP_LT_FAIL: failed\n", __func__);
			training_finished = 1;
			ret = -1;
		}
	}

	return ret;
}

static unsigned int exynos_dp_set_link_train(struct edp_device_info *edp_info)
{
	unsigned int ret;

	exynos_dp_init_training();

	ret = exynos_dp_sw_link_training(edp_info);
	if (ret != EXYNOS_DP_SUCCESS)
		printk(BIOS_ERR, "DP dp_sw_link_traning() failed\n");

	return ret;
}

static void exynos_dp_enable_scramble(unsigned int enable)
{
	unsigned char data;

	if (enable) {
		exynos_dp_enable_scrambling(DP_ENABLE);

		exynos_dp_read_byte_from_dpcd(DPCD_TRAINING_PATTERN_SET,
				&data);
		exynos_dp_write_byte_to_dpcd(DPCD_TRAINING_PATTERN_SET,
			(u8)(data & ~DPCD_SCRAMBLING_DISABLED));
	} else {
		exynos_dp_enable_scrambling(DP_DISABLE);
		exynos_dp_read_byte_from_dpcd(DPCD_TRAINING_PATTERN_SET,
				&data);
		exynos_dp_write_byte_to_dpcd(DPCD_TRAINING_PATTERN_SET,
			(u8)(data | DPCD_SCRAMBLING_DISABLED));
	}
}

static unsigned int exynos_dp_config_video(struct edp_device_info *edp_info)
{
	unsigned int ret = 0;
	unsigned int retry_cnt;

	mdelay(1);

	if (edp_info->video_info.master_mode) {
		printk(BIOS_ERR,
		       "DP does not support master mode: bailing out\n");
		return -1;
	} else {
		/* debug slave */
		exynos_dp_config_video_slave_mode(&edp_info->video_info);
	}

	exynos_dp_set_video_color_format(&edp_info->video_info);

	ret = exynos_dp_get_pll_lock_status();
	if (ret != PLL_LOCKED) {
		printk(BIOS_ERR, "DP PLL is not locked yet\n");
		return -1;
	}

	if (edp_info->video_info.master_mode == 0) {
		retry_cnt = 10;
		while (retry_cnt) {
			ret = exynos_dp_is_slave_video_stream_clock_on();
			if (ret != EXYNOS_DP_SUCCESS) {
				if (retry_cnt == 0) {
					printk(BIOS_ERR, "DP stream_clock_on failed\n");
					return ret;
				}
				retry_cnt--;
				mdelay(1);
			} else {
				printk(BIOS_DEBUG, "DP stream_clock succeeds\n");
				break;
			}
		}
	}

	/* Set to use the register calculated M/N video */
	exynos_dp_set_video_cr_mn(CALCULATED_M, 0, 0);

	/* For video bist, Video timing must be generated by register
	* not clear if we still need this. We could take it out and it
	* might appear to work, then fail strangely.
	 */
	exynos_dp_set_video_timing_mode(VIDEO_TIMING_FROM_CAPTURE);

	/* we need to be sure this is off. */
	exynos_dp_disable_video_bist();

	/* Disable video mute */
	exynos_dp_enable_video_mute(DP_DISABLE);

	/* Configure video Master or Slave mode */
	exynos_dp_enable_video_master(edp_info->video_info.master_mode);

	/* Enable video */
	exynos_dp_start_video();

	if (edp_info->video_info.master_mode == 0) {
		retry_cnt = 500;
		while (retry_cnt) {
			ret = exynos_dp_is_video_stream_on();
			if (ret != EXYNOS_DP_SUCCESS) {
				retry_cnt--;
				if (retry_cnt == 0) {
					printk(BIOS_ERR, "DP Timeout of video stream\n");
				}
			} else {
				printk(BIOS_DEBUG, "DP video stream is on\n");
				break;
			}
			/* this is a cheap operation, involving some register
			 * reads, and no AUX channel IO. A ms. delay is fine.
			 */
			mdelay(1);
		}
	}

	return ret;
}

int exynos_init_dp(struct edp_device_info *edp_info)
{
	unsigned int ret;

	dp_phy_control(1);

	ret = exynos_dp_init_dp();
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP exynos_dp_init_dp() failed\n");
		return ret;
	}

	ret = exynos_dp_handle_edid(edp_info);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "EDP handle_edid fail\n");
		return ret;
	}

	ret = exynos_dp_set_link_train(edp_info);
	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "DP link training fail\n");
		return ret;
	}
	printk(BIOS_DEBUG, "EDP link training ok\n");

	exynos_dp_enable_scramble(DP_ENABLE);
	exynos_dp_enable_rx_to_enhanced_mode(DP_ENABLE);
	exynos_dp_enable_enhanced_mode(DP_ENABLE);

	exynos_dp_set_link_bandwidth(edp_info->lane_bw);
	exynos_dp_set_lane_count(edp_info->lane_cnt);

	exynos_dp_init_video();
	ret = exynos_dp_config_video(edp_info);

	if (ret != EXYNOS_DP_SUCCESS) {
		printk(BIOS_ERR, "Exynos DP init failed\n");
		return ret;
	}
	printk(BIOS_DEBUG, "Exynos DP init done\n");

	return ret;
}
