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

#define ONE_BLOCK_SIZE			128

#define DP_LINK_CONSTANT_N_VALUE	0x8000
#define DP_LINK_STATUS_SIZE		6

#define DP_LANE0_1_STATUS		0x202
#define DP_LANE2_3_STATUS		0x203
#define DP_LANE_CR_DONE			(1 << 0)
#define DP_LANE_CHANNEL_EQ_DONE		(1 << 1)
#define DP_LANE_SYMBOL_LOCKED		(1 << 2)

#define DP_BRANCH_OUI_HEADER_SIZE	0xc
#define DP_RECEIVER_CAP_SIZE		0xf
#define DP_DSC_RECEIVER_CAP_SIZE	0xf
#define EDP_PSR_RECEIVER_CAP_SIZE	2
#define EDP_DISPLAY_CTL_CAP_SIZE	3
#define DP_LTTPR_COMMON_CAP_SIZE	8
#define DP_LTTPR_PHY_CAP_SIZE		3

#define DP_TRAINING_AUX_RD_INTERVAL		0x00e
#define DP_TRAINING_AUX_RD_MASK			0x7f
#define DP_EXTENDED_RECEIVER_CAP_FIELD_PRESENT	(1 << 7)

#define DP_EDP_DPCD_REV		0x700
#define DP_EDP_11		0x00
#define DP_EDP_12		0x01
#define DP_EDP_13		0x02
#define DP_EDP_14		0x03
#define DP_EDP_14a		0x04
#define DP_EDP_14b		0x05

/* Receiver Capability */
#define DP_DPCD_REV		0x000
#define DP_DPCD_REV_10		0x10
#define DP_DPCD_REV_11		0x11
#define DP_DPCD_REV_12		0x12
#define DP_DPCD_REV_13		0x13
#define DP_DPCD_REV_14		0x14

#define DP_CHANNEL_EQ_BITS (DP_LANE_CR_DONE | \
			  DP_LANE_CHANNEL_EQ_DONE | \
			  DP_LANE_SYMBOL_LOCKED)

#define DP_LANE_ALIGN_STATUS_UPDATED		0x204
#define DP_INTERLANE_ALIGN_DONE			(1 << 0)
#define DP_DOWNSTREAM_PORT_STATUS_CHANGED	(1 << 6)
#define DP_LINK_STATUS_UPDATED			(1 << 7)

#define DP_SINK_STATUS				0x205
#define DP_RECEIVE_PORT_0_STATUS		(1 << 0)
#define DP_RECEIVE_PORT_1_STATUS		(1 << 1)
#define DP_STREAM_REGENERATION_STATUS		(1 << 2)

#define DP_AUX_MAX_PAYLOAD_BYTES		16

#define MAX_LANECOUNT				4

enum {
	DP_LANECOUNT_1	= 0x1,
	DP_LANECOUNT_2	= 0x2,
	DP_LANECOUNT_4	= 0x4,
};

enum {
	DP_VERSION_11		= 0x11,
	DP_VERSION_12		= 0x12,
	DP_VERSION_14		= 0x14,
	DP_VERSION_12_14	= 0x16,
	DP_VERSION_14_14	= 0x17,
	DP_VERSION_MAX,
};

enum {
	DPTX_SWING0		= 0x00,
	DPTX_SWING1		= 0x01,
	DPTX_SWING2		= 0x02,
	DPTX_SWING3		= 0x03,
};

enum {
	DPTX_PREEMPHASIS0	= 0x00,
	DPTX_PREEMPHASIS1	= 0x01,
	DPTX_PREEMPHASIS2	= 0x02,
	DPTX_PREEMPHASIS3	= 0x03,
};

enum {
	DPTX_PASS			= 0,
	DPTX_PLUG_OUT			= 1,
	DPTX_TIMEOUT			= 2,
	DPTX_AUTH_FAIL			= 3,
	DPTX_EDID_FAIL			= 4,
	DPTX_TRANING_FAIL		= 5,
	DPTX_TRANING_STATE_CHANGE	= 6,
};

enum {
	FEC_ERROR_COUNT_DISABLE                = 0x0,
	FEC_UNCORRECTED_BLOCK_ERROR_COUNT      = 0x1,
	FEC_CORRECTED_BLOCK_ERROR_COUNT        = 0x2,
	FEC_BIT_ERROR_COUNT                    = 0x3,
	FEC_PARITY_BLOCK_ERROR_COUNT           = 0x4,
	FEC_PARITY_BIT_ERROR_COUNT             = 0x5,
};

enum {
	DPTX_NTSTATE_STARTUP		= 0x0,
	DPTX_NTSTATE_CHECKCAP		= 0x1,
	DPTX_NTSTATE_CHECKEDID		= 0x2,
	DPTX_NTSTATE_TRAINING_PRE	= 0x3,
	DPTX_NTSTATE_TRAINING		= 0x4,
	DPTX_NTSTATE_CHECKTIMING	= 0x5,
	DPTX_NTSTATE_NORMAL		= 0x6,
	DPTX_NTSTATE_POWERSAVE		= 0x7,
	DPTX_NTSTATE_DPIDLE		= 0x8,
	DPTX_NTSTATE_MAX,
};

enum {
	DPTX_DISP_NONE		= 0,
	DPTX_DISP_RESUME	= 1,
	DPTX_DISP_SUSPEND	= 2,
};

enum {
	TRAIN_STEP_SUCCESS		= 0,
	TRAIN_STEP_FAIL_BREAK		= 1,
	TRAIN_STEP_FAIL_NOT_BREAK	= 2,
};

#define DPTX_TRAIN_RETRY_LIMIT		0x8
#define DPTX_TRAIN_MAX_ITERATION	0x5

#define HPD_INT_EVNET		BIT(3)
#define HPD_CONNECT		BIT(2)
#define HPD_DISCONNECT		BIT(1)
#define HPD_INITIAL_STATE	0

static bool dptx_auxwrite_bytes(struct mtk_dp *mtk_dp, u8 cmd,
				u32 dpcd_addr, size_t length, u8 *data)
{
	if (retry(7, dptx_hal_auxwrite_bytes(mtk_dp, cmd, dpcd_addr, length, data),
		  mdelay(1)))
		return true;

	printk(BIOS_ERR, "aux write fail: cmd = %d, addr = %#x, len = %ld\n",
	       cmd, dpcd_addr, length);

	return false;
}

static bool dptx_auxwrite_dpcd(struct mtk_dp *mtk_dp, u8 cmd,
			       u32 dpcd_addr, size_t length, u8 *data)
{
	bool ret = true;
	size_t offset = 0;
	size_t len;

	while (offset < length) {
		len = MIN(length - offset, DP_AUX_MAX_PAYLOAD_BYTES);
		ret &= dptx_auxwrite_bytes(mtk_dp, cmd, dpcd_addr + offset,
					   len, data + offset);
		offset += len;
	}
	return ret;
}

static bool dptx_auxread_bytes(struct mtk_dp *mtk_dp, u8 cmd,
			       u32 dpcd_addr, size_t length, u8 *data)
{
	if (retry(7, dptx_hal_auxread_bytes(mtk_dp, cmd, dpcd_addr, length, data),
		  mdelay(1)))
		return true;

	printk(BIOS_ERR, "aux read fail: cmd = %d, addr = %#x, len = %ld\n",
	       cmd, dpcd_addr, length);

	return false;
}

static bool dptx_auxread_dpcd(struct mtk_dp *mtk_dp, u8 cmd,
			      u32 dpcd_addr, size_t length, u8 *rxbuf)
{
	bool ret = true;
	size_t offset = 0;
	size_t len;

	while (offset < length) {
		len = MIN(length - offset, DP_AUX_MAX_PAYLOAD_BYTES);
		ret &= dptx_auxread_bytes(mtk_dp, cmd, dpcd_addr + offset,
					  len, rxbuf + offset);
		offset += len;
	}
	return ret;
}

static int dptx_get_edid(struct mtk_dp *mtk_dp, struct edid *out)
{
	int ret;
	u8 edid[ONE_BLOCK_SIZE];
	u8 tmp = 0;

	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_I2C_WRITE, 0x50, 0x1, &tmp);

	for (tmp = 0; tmp < ONE_BLOCK_SIZE / DP_AUX_MAX_PAYLOAD_BYTES; tmp++)
		dptx_auxread_dpcd(mtk_dp, DP_AUX_I2C_READ,
				  0x50, DP_AUX_MAX_PAYLOAD_BYTES,
				  edid + tmp * 16);

	ret = decode_edid(edid, ONE_BLOCK_SIZE, out);
	if (ret != EDID_CONFORMANT) {
		printk(BIOS_ERR, "failed to decode edid(%d).\n", ret);
		return -1;
	}

	mtk_dp->edid = out;
	return 0;
}

static u8 dptx_link_status(const u8 link_status[DP_LINK_STATUS_SIZE], int r)
{
	return link_status[r - DP_LANE0_1_STATUS];
}

static u8 dptx_get_lane_status(const u8 link_status[DP_LINK_STATUS_SIZE],
			       int lane)
{
	int i = DP_LANE0_1_STATUS + (lane >> 1);
	int s = (lane & 1) * 4;
	u8 l = dptx_link_status(link_status, i);

	return (l >> s) & 0xf;
}

static bool dptx_clock_recovery_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
				   int lane_count)
{
	int lane;
	u8 lane_status;

	for (lane = 0; lane < lane_count; lane++) {
		lane_status = dptx_get_lane_status(link_status, lane);
		if ((lane_status & DP_LANE_CR_DONE) == 0)
			return false;
	}
	return true;
}

static void dptx_link_train_clock_recovery_delay(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	u32 rd_interval = dpcd[DP_TRAINING_AUX_RD_INTERVAL] &
			  DP_TRAINING_AUX_RD_MASK;

	if (rd_interval > 4)
		printk(BIOS_ERR, "aux interval %d, out of range (max 4)\n",
		       rd_interval);

	if (rd_interval == 0 || dpcd[DP_DPCD_REV] >= DP_DPCD_REV_14)
		rd_interval = 1;
	else
		rd_interval *= 4;

	mdelay(rd_interval);
}

static void dptx_link_train_channel_eq_delay(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	u32 rd_interval = dpcd[DP_TRAINING_AUX_RD_INTERVAL] &
			  DP_TRAINING_AUX_RD_MASK;

	if (rd_interval > 4)
		printk(BIOS_ERR, "aux interval %d, out of range (max 4)\n",
		       rd_interval);

	if (rd_interval == 0)
		rd_interval = 1;
	else
		rd_interval *= 4;

	mdelay(rd_interval);
}

static bool dptx_channel_eq_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
			       int lane_count)
{
	u8 lane_align;
	u8 lane_status;
	int lane;

	lane_align = dptx_link_status(link_status,
				      DP_LANE_ALIGN_STATUS_UPDATED);

	if ((lane_align & DP_INTERLANE_ALIGN_DONE) == 0)
		return false;

	for (lane = 0; lane < lane_count; lane++) {
		lane_status = dptx_get_lane_status(link_status, lane);
		if ((lane_status & DP_CHANNEL_EQ_BITS) != DP_CHANNEL_EQ_BITS)
			return false;
	}
	return true;
}

static void dptx_videomute(struct mtk_dp *mtk_dp, bool enable)
{
	dptx_hal_videomute(mtk_dp, enable);
}

static void dptx_fec_ready(struct mtk_dp *mtk_dp, u8 err_cnt_sel)
{
	u8 i, data[3];

	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ, 0x90, 0x1, data);
	printk(BIOS_DEBUG, "FEC Capable[0], [0:3] should be 1: %#x\n",
	       data[0]);

	/*
	 * FEC error count select 120[3:1]:
	 * 000b: FEC_ERROR_COUNT_DIS
	 * 001b: UNCORRECTED_BLOCK_ERROR_COUNT
	 * 010b: CORRECTED_BLOCK_ERROR_COUNT
	 * 011b: BIT_ERROR_COUNT
	 * 100b: PARITY_BLOCK_ERROR_COUNT
	 * 101b: PARITY_BIT_ERROR_COUNT
	 */
	if (data[0] & BIT(0)) {
		mtk_dp->has_fec = true;
		data[0] = (err_cnt_sel << 1) | 0x1;

		dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
				   0x120, 0x1, data);
		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
				  0x280, 0x3, data);

		for (i = 0; i < 3; i++)
			printk(BIOS_DEBUG, "FEC status & error Count: %#x\n",
			       data[i]);
	}
}

static void dptx_init_variable(struct mtk_dp *mtk_dp)
{
	mtk_dp->regs = (void *)EDP_BASE;
	mtk_dp->train_info.sys_max_linkrate = DP_LINKRATE_HBR3;
	mtk_dp->train_info.linkrate = DP_LINKRATE_HBR2;
	mtk_dp->train_info.linklane_count = DP_LANECOUNT_4;
	mtk_dp->train_info.sink_extcap_en = false;
	mtk_dp->train_info.sink_ssc_en = false;
	mtk_dp->train_info.tps3 = true;
	mtk_dp->train_info.tps4 = true;
	mtk_dp->training_state = DPTX_NTSTATE_STARTUP;
	mtk_dp->info.format = DP_COLOR_FORMAT_RGB_444;
	mtk_dp->info.depth = DP_COLOR_DEPTH_8BIT;
	mtk_dp->power_on = false;
	mtk_dp->video_enable = false;
	mtk_dp->dp_ready = false;
	mtk_dp->has_dsc = false;
	mtk_dp->has_fec = false;
	mtk_dp->dsc_enable = false;
}

static inline bool dptx_check_res_sample_rate(const struct edid *edid)
{
	return edid->mode.va + edid->mode.vbl <= 525;
}

static void dptx_setsdp_downcnt_init(struct mtk_dp *mtk_dp, u16 sram_read_start)
{
	u32 count = 0; /* count: sdp_down_cnt_init */
	u8 offset;

	if (mtk_dp->edid->mode.pixel_clock > 0)
		count = (sram_read_start * 2700 * 8 *
			 mtk_dp->train_info.linkrate) /
			(mtk_dp->edid->mode.pixel_clock * 4);

	switch (mtk_dp->train_info.linklane_count) {
	case DP_LANECOUNT_1:
		count = (count > 0x1a) ? count : 0x1a;
		break;
	case DP_LANECOUNT_2:
		/* Case for LowResolution and High Audio Sample Rate. */
		offset = dptx_check_res_sample_rate(mtk_dp->edid) ?
			 0x04 : 0x00;
		count = (count > 0x10) ? count : 0x10 + offset;
		break;
	case DP_LANECOUNT_4:
	default:
		count = (count > 0x06) ? count : 0x06;
		break;
	}

	printk(BIOS_DEBUG, "pixel rate(khz) = %d, sdp_dc_init = %#x\n",
	       mtk_dp->edid->mode.pixel_clock, count);

	dptx_hal_setsdp_downcnt_init(mtk_dp, count);
}

static void dptx_setsdp_downcnt_init_inhblanking(struct mtk_dp *mtk_dp)
{
	int pixclk_mhz = mtk_dp->edid->mode.pixel_clock / 1000;
	u8 offset;
	u16 count = 0; /* count: sdp_down_cnt_init*/

	switch (mtk_dp->train_info.linklane_count) {
	case DP_LANECOUNT_1:
		count = 0x20;
		break;
	case DP_LANECOUNT_2:
		offset = dptx_check_res_sample_rate(mtk_dp->edid) ?
			 0x14 : 0x00;
		count = 0x0018 + offset;
		break;
	case DP_LANECOUNT_4:
	default:
		offset = dptx_check_res_sample_rate(mtk_dp->edid) ?
			 0x08 : 0x00;
		if (pixclk_mhz > mtk_dp->train_info.linkrate * 27) {
			count = 0x8;
			printk(BIOS_ERR, "Pixclk > LinkRateChange\n");
		} else {
			count = 0x10 + offset;
		}
		break;
	}

	dptx_hal_setsdp_downcnt_init_inhblanking(mtk_dp, count);
}

static void dptx_set_tu(struct mtk_dp *mtk_dp)
{
	u8 bpp;
	u16 sram_read_start = DPTX_TBC_BUF_READSTARTADRTHRD;
	int tu_size, n_value, f_value, pixclk_mhz;

	bpp = dptx_hal_get_colorbpp(mtk_dp);
	pixclk_mhz = mtk_dp->edid->mode.pixel_clock / 1000;
	tu_size = (640 * pixclk_mhz * bpp) /
		  (mtk_dp->train_info.linkrate * 27 *
		   mtk_dp->train_info.linklane_count * 8);

	n_value = tu_size / 10;
	f_value = tu_size % 10;
	printk(BIOS_DEBUG, "TU_size %d, FValue %d\n", tu_size, f_value);

	if (mtk_dp->train_info.linklane_count > 0) {
		sram_read_start = mtk_dp->edid->mode.ha /
				  (mtk_dp->train_info.linklane_count *
				   4 * 2 * 2);
		sram_read_start = MIN(sram_read_start,
				      DPTX_TBC_BUF_READSTARTADRTHRD);
		dptx_hal_settu_sramrd_start(mtk_dp, sram_read_start);
	}

	dptx_hal_settu_setencoder(mtk_dp);
	dptx_setsdp_downcnt_init_inhblanking(mtk_dp);
	dptx_setsdp_downcnt_init(mtk_dp, sram_read_start);
}

static void dptx_set_misc(struct mtk_dp *mtk_dp)
{
	u8 format, depth;
	union misc_t dptx_misc;

	format = mtk_dp->info.format;
	depth = mtk_dp->info.depth;

	/*
	 * MISC 0/1 reference to spec 1.4a p143 Table 2-96.
	 * MISC0[7:5] color depth.
	 */
	dptx_misc.dp_misc.color_depth = depth;

	/*
	 * MISC0[3]: 0->RGB, 1->YUV
	 * MISC0[2:1]: 01b->4:2:2, 10b->4:4:4
	 */
	switch (format) {
	case DP_COLOR_FORMAT_YUV_444:
		dptx_misc.dp_misc.color_format = 0x1;
		break;
	case DP_COLOR_FORMAT_YUV_422:
		dptx_misc.dp_misc.color_format = 0x2;
		break;
	case DP_COLOR_FORMAT_RAW:
		dptx_misc.dp_misc.color_format = 0x1;
		dptx_misc.dp_misc.spec_def2 = 0x1;
		break;
	case DP_COLOR_FORMAT_YONLY:
		dptx_misc.dp_misc.color_format = 0x0;
		dptx_misc.dp_misc.spec_def2 = 0x1;
		break;
	case DP_COLOR_FORMAT_YUV_420:
	case DP_COLOR_FORMAT_RGB_444:
	default:
		break;
	}

	dptx_hal_setmisc(mtk_dp, dptx_misc.cmisc);
}

static void dptx_set_dptxout(struct mtk_dp *mtk_dp)
{
	dptx_hal_bypassmsa_en(mtk_dp, false);
	dptx_set_tu(mtk_dp);
}

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

static void dptx_check_sinkcap(struct mtk_dp *mtk_dp)
{
	u8 buffer[16];

	memset(buffer, 0x0, sizeof(buffer));

	buffer[0] = 0x1;
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
			   DPCD_00600, 0x1, buffer);

	mdelay(2);

	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
			  DPCD_00000, 0x10, buffer);

	mtk_dp->train_info.sink_extcap_en = false;
	mtk_dp->train_info.dpcd_rev = buffer[0];

	printk(BIOS_INFO, "SINK DPCD version: %#x\n",
	       mtk_dp->train_info.dpcd_rev);

	memcpy(mtk_dp->rx_cap, buffer, sizeof(mtk_dp->rx_cap));

	mtk_dp->rx_cap[14] &= 0x7f;

	if (mtk_dp->train_info.dpcd_rev >= 0x14)
		dptx_fec_ready(mtk_dp, FEC_BIT_ERROR_COUNT);

	mtk_dp->train_info.linkrate = MIN(buffer[0x1],
					  mtk_dp->train_info.sys_max_linkrate);
	mtk_dp->train_info.linklane_count = MIN(buffer[2] & 0x1F,
						MAX_LANECOUNT);

	mtk_dp->train_info.tps3 = (buffer[2] & BIT(6)) >> 0x6;
	mtk_dp->train_info.tps4 = (buffer[3] & BIT(7)) >> 0x7;

	mtk_dp->train_info.down_stream_port_present = (buffer[5] & BIT(0));

	if ((buffer[3] & BIT(0)) == 0x1) {
		printk(BIOS_INFO, "SINK SUPPORT SSC!\n");
		mtk_dp->train_info.sink_ssc_en = true;
	} else {
		printk(BIOS_INFO, "SINK NOT SUPPORT SSC!\n");
		mtk_dp->train_info.sink_ssc_en = false;
	}

	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
			  DPCD_00021, 0x1, buffer);
	mtk_dp->train_info.dp_mstcap = (buffer[0] & BIT(0));
	mtk_dp->train_info.dp_mstbranch = false;

	if (mtk_dp->train_info.dp_mstcap == BIT(0)) {
		if (mtk_dp->train_info.down_stream_port_present == 0x1)
			mtk_dp->train_info.dp_mstbranch = true;

		dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
				  DPCD_02003, 0x1, buffer);
		if (buffer[0] != 0x0)
			dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
					   DPCD_02003, 0x1, buffer);
	}

	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
			  DPCD_00600, 0x1, buffer);
	if (buffer[0] != 0x1) {
		buffer[0] = 0x1;
		dptx_auxwrite_dpcd(mtk_dp, DP_AUX_NATIVE_WRITE,
				   DPCD_00600, 0x1, buffer);
	}

	dptx_auxread_dpcd(mtk_dp, DP_AUX_NATIVE_READ,
			  DPCD_00200, 0x2, buffer);
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
	int ret = DPTX_PASS;
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
		ret = dptx_trainingflow(mtk_dp, linkrate, lanecount);

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

static void dptx_video_enable(struct mtk_dp *mtk_dp, bool enable)
{
	printk(BIOS_INFO, "Output Video %s!\n", enable ?
	       "enable" : "disable");

	if (enable) {
		dptx_set_dptxout(mtk_dp);
		dptx_videomute(mtk_dp, false);
		dptx_hal_verify_clock(mtk_dp);
	} else
		dptx_videomute(mtk_dp, true);
}

static void dptx_set_color_format(struct mtk_dp *mtk_dp, u8 color_format)
{
	dptx_hal_set_color_format(mtk_dp, color_format);
}

static void dptx_set_color_depth(struct mtk_dp *mtk_dp, u8 color_depth)
{
	dptx_hal_set_color_depth(mtk_dp, color_depth);
}

static void dptx_video_config(struct mtk_dp *mtk_dp)
{
	u32 mvid = 0;
	bool overwrite = false;

	dptx_hal_overwrite_mn(mtk_dp, overwrite, mvid, 0x8000);

	/* Interlace does not support. */
	dptx_hal_set_msa(mtk_dp);
	dptx_set_misc(mtk_dp);
	dptx_set_color_depth(mtk_dp, mtk_dp->info.depth);
	dptx_set_color_format(mtk_dp, mtk_dp->info.format);
}

int mtk_edp_init(struct edid *edid)
{
	struct mtk_dp mtk_edp;

	dptx_init_variable(&mtk_edp);
	dptx_init_port(&mtk_edp);

	if (!dptx_hal_hpd_high(&mtk_edp)) {
		printk(BIOS_ERR, "HPD is low\n");
		return -1;
	}

	dptx_check_sinkcap(&mtk_edp);

	if (dptx_get_edid(&mtk_edp, edid) != 0) {
		printk(BIOS_ERR, "Failed to get EDID\n");
		return -1;
	}

	dptx_set_trainingstart(&mtk_edp);
	dp_intf_config(edid);
	dptx_video_config(&mtk_edp);
	dptx_video_enable(&mtk_edp, true);

	return 0;
}
