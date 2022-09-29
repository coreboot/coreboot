/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_DP_DPTX_H
#define SOC_MEDIATEK_COMMON_DP_DPTX_H

#define DPTX_TBC_BUF_READSTARTADRTHRD	0x08
#define ENABLE_DPTX_EF_MODE		0x1
#define DPTX_AUX_SET_ENAHNCED_FRAME	0x80

union misc_t {
	struct {
		u8 is_sync_clock : 1;
		u8 color_format : 2;
		u8 spec_def1 : 2;
		u8 color_depth : 3;
		u8 interlaced : 1;
		u8 stereo_attr : 2;
		u8 reserved : 3;
		u8 is_vsc_sdp : 1;
		u8 spec_def2 : 1;
	} dp_misc;
	u8 cmisc[2];
};

struct dptx_training_info {
	bool sink_extcap_en;
	bool tps3;
	bool tps4;
	bool sink_ssc_en;
	bool dp_mstcap;
	bool dp_mstbranch;
	bool down_stream_port_present;
	bool cr_done;
	bool eq_done;
	u8 sys_max_linkrate;
	u8 linkrate;
	u8 linklane_count;
	u8 dpcd_rev;
	u8 sink_count_num;
};

struct dptx_info {
	uint8_t depth;
	uint8_t format;
	uint8_t resolution;
};

struct mtk_dp {
	int id;
	struct edid *edid;
	u8 rx_cap[16];
	struct dptx_info info;
	int state;
	int state_pre;
	struct dptx_training_info train_info;
	int training_state;
	u8 irq_status;
	u32 min_clock;
	u32 max_clock;
	u32 max_hdisplay;
	u32 max_vdisplay;
	void *regs;
	int disp_status;
	bool power_on;
	bool audio_enable;
	bool video_enable;
	bool dp_ready;
	bool has_dsc;
	bool has_fec;
	bool dsc_enable;
	bool enabled;
	bool powered;
};

int mtk_edp_init(struct edid *edid);

#endif /* SOC_MEDIATEK_COMMON_DP_DPTX_H */
