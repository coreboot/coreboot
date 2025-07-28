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
#include <timer.h>

static bool dptx_auxwrite_bytes(struct mtk_dp *mtk_dp, u8 cmd, u32 dpcd_addr,
			 size_t length, u8 *data)
{
	if (retry(7, dptx_hal_auxwrite_bytes(mtk_dp, cmd, dpcd_addr, length, data),
		  mdelay(1)))
		return true;

	printk(BIOS_ERR, "aux write fail: cmd = %d, addr = %#x, len = %ld\n",
	       cmd, dpcd_addr, length);

	return false;
}

bool dptx_auxwrite_dpcd(struct mtk_dp *mtk_dp, u8 cmd, u32 dpcd_addr,
			size_t length, u8 *data)
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

bool dptx_auxread_dpcd(struct mtk_dp *mtk_dp, u8 cmd, u32 dpcd_addr,
		       size_t length, u8 *rxbuf)
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
	u8 edid[EDID_BUF_SIZE];
	u8 tmp = 0;
	u8 extblock = 0;
	size_t total_size;
	struct stopwatch sw;

	stopwatch_init(&sw);
	dptx_auxwrite_dpcd(mtk_dp, DP_AUX_I2C_WRITE, 0x50, 0x1, &tmp);

	/* Read 1st block */
	for (tmp = 0; tmp < EDID_BLOCK_SIZE / DP_AUX_MAX_PAYLOAD_BYTES; tmp++)
		dptx_auxread_dpcd(mtk_dp, DP_AUX_I2C_READ,
				  0x50, DP_AUX_MAX_PAYLOAD_BYTES,
				  edid + tmp * DP_AUX_MAX_PAYLOAD_BYTES);

	extblock = edid[EDID_EXT_BLOCK_COUNT];
	total_size = EDID_BLOCK_SIZE * (1 + extblock);

	if (total_size > EDID_BUF_SIZE) {
		printk(BIOS_WARNING, "The edid block size(%zu) is larger than %u bytes\n",
		       total_size, EDID_BUF_SIZE);
		printk(BIOS_WARNING, "Read first %u bytes only\n", EDID_BUF_SIZE);
		total_size = EDID_BUF_SIZE;
	}

	if (total_size > EDID_BLOCK_SIZE) {
		for (tmp = EDID_BLOCK_SIZE / DP_AUX_MAX_PAYLOAD_BYTES;
		     tmp < total_size / DP_AUX_MAX_PAYLOAD_BYTES; tmp++)
			dptx_auxread_dpcd(mtk_dp, DP_AUX_I2C_READ,
					  0x50, DP_AUX_MAX_PAYLOAD_BYTES,
					  edid + tmp * DP_AUX_MAX_PAYLOAD_BYTES);
	}

	ret = decode_edid(edid, total_size, out);
	if (ret != EDID_CONFORMANT) {
		printk(BIOS_ERR, "failed to decode edid(%d).\n", ret);
		return -1;
	}

	mtk_dp->edid = out;
	printk(BIOS_INFO, "%s done after %lld msecs\n", __func__,
	       stopwatch_duration_msecs(&sw));

	return 0;
}

static u8 dptx_link_status(const u8 link_status[DP_LINK_STATUS_SIZE], int r)
{
	return link_status[r - DP_LANE0_1_STATUS];
}

static u8 dptx_get_lane_status(const u8 link_status[DP_LINK_STATUS_SIZE], int lane)
{
	int i = DP_LANE0_1_STATUS + (lane >> 1);
	int s = (lane & 1) * 4;
	u8 l = dptx_link_status(link_status, i);

	return (l >> s) & 0xf;
}

bool dptx_clock_recovery_ok(const u8 link_status[DP_LINK_STATUS_SIZE], int lane_count)
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

void dptx_link_train_clock_recovery_delay(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
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

void dptx_link_train_channel_eq_delay(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
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

bool dptx_channel_eq_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
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

void dptx_init_variable(struct mtk_dp *mtk_dp)
{
	mtk_dp->regs = (void *)EDP_BASE;
	mtk_dp->phy_regs = (void *)EDP_PHY_BASE;
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
	mtk_dp->edp_version = EDP_VERSION;
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
	int tu_size, f_value, pixclk_mhz;

	bpp = dptx_hal_get_colorbpp(mtk_dp);
	pixclk_mhz = mtk_dp->edid->mode.pixel_clock / 1000;
	tu_size = (640 * pixclk_mhz * bpp) /
		  (mtk_dp->train_info.linkrate * 27 *
		   mtk_dp->train_info.linklane_count * 8);

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
	union misc_t dptx_misc = { .cmisc = {} };

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

static void dptx_check_sinkcap(struct mtk_dp *mtk_dp)
{
	u8 buffer[16];
	struct stopwatch sw;

	stopwatch_init(&sw);
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
	printk(BIOS_INFO, "%s done after %lld msecs\n", __func__,
	       stopwatch_duration_msecs(&sw));
}

void dptx_video_enable(struct mtk_dp *mtk_dp, bool enable)
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

void dptx_video_config(struct mtk_dp *mtk_dp)
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

__weak void dptx_power_on(void) { /* do nothing */ };

int mtk_edp_init(struct mtk_dp *mtk_dp, struct edid *edid)
{
	struct stopwatch sw;

	stopwatch_init(&sw);
	dptx_power_on();
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
	printk(BIOS_INFO, "%s done after %lld msecs\n", __func__,
	       stopwatch_duration_msecs(&sw));

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
