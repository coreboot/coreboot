/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_DP_DPTX_COMMON_H
#define SOC_MEDIATEK_COMMON_DP_DPTX_COMMON_H

#include <types.h>

#define DP_LANSE_ADJUST_SIZE		2
#define DPTX_TBC_BUF_READSTARTADRTHRD	0x08
#define ENABLE_DPTX_EF_MODE		0x1
#define DPTX_AUX_SET_ENAHNCED_FRAME	0x80

#define EDID_BLOCK_SIZE			128
#define EDID_BUF_SIZE			(EDID_BLOCK_SIZE * 4)

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
#define EDID_EXT_BLOCK_COUNT	0x7E

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

#define HPD_WAIT_TIMEOUT_MS			200
#define WAIT_AUX_READY_TIME_MS			1

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
	FEC_ERROR_COUNT_DISABLE			= 0x0,
	FEC_UNCORRECTED_BLOCK_ERROR_COUNT	= 0x1,
	FEC_CORRECTED_BLOCK_ERROR_COUNT		= 0x2,
	FEC_BIT_ERROR_COUNT			= 0x3,
	FEC_PARITY_BLOCK_ERROR_COUNT		= 0x4,
	FEC_PARITY_BIT_ERROR_COUNT		= 0x5,
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
	void *phy_regs;
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
	bool force_max_swing;
	u8 edp_version;
};

int mtk_edp_init(struct mtk_dp *mtk_dp, struct edid *edid);
int mtk_edp_enable(struct mtk_dp *mtk_dp);

void dptx_set_tx_power_con(void);
void dptx_set_26mhz_clock(void);
int dptx_set_trainingstart(struct mtk_dp *mtk_dp);
bool dptx_auxread_dpcd(struct mtk_dp *mtk_dp, u8 cmd, u32 dpcd_addr,
		       size_t length, u8 *rxbuf);
bool dptx_auxwrite_dpcd(struct mtk_dp *mtk_dp, u8 cmd, u32 dpcd_addr,
			size_t length, u8 *data);
bool dptx_channel_eq_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
			int lane_count);
bool dptx_clock_recovery_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
			    int lane_count);
void dptx_init_variable(struct mtk_dp *mtk_dp);
void dptx_link_train_channel_eq_delay(const u8 dpcd[DP_RECEIVER_CAP_SIZE]);
void dptx_link_train_clock_recovery_delay(const u8 dpcd[DP_RECEIVER_CAP_SIZE]);
void dptx_power_on(void);
void dptx_video_config(struct mtk_dp *mtk_dp);
void dptx_video_enable(struct mtk_dp *mtk_dp, bool enable);

#endif /* SOC_MEDIATEK_COMMON_DP_DPTX_COMMON_H */
