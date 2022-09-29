/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_DP_DPTX_HAL_H
#define SOC_MEDIATEK_COMMON_DP_DPTX_HAL_H

#define AUX_CMD_I2C_R_MOT0	0x1
#define AUX_CMD_I2C_R		0x5
#define AUX_CMD_NATIVE_R	0x9
#define AUX_WAITREPLY_LPNUM	20000

#define DP_AUX_I2C_WRITE		0x0
#define DP_AUX_I2C_READ			0x1
#define DP_AUX_I2C_WRITE_STATUS_UPDATE	0x2
#define DP_AUX_I2C_MOT			0x4
#define DP_AUX_NATIVE_WRITE		0x8
#define DP_AUX_NATIVE_READ		0x9

#define DP_WRITE1BYTE(mtk_dp, reg, u8_val) \
	mtk_dp_write_byte(mtk_dp, reg, u8_val, 0xff)
#define DP_WRITE2BYTE(mtk_dp, reg, u16_val) \
	mtk_dp_mask(mtk_dp, reg, u16_val, 0xffff)

enum {
	DPTX_LANE0	= 0x0,
	DPTX_LANE1	= 0x1,
	DPTX_LANE2	= 0x2,
	DPTX_LANE3	= 0x3,
	DPTX_LANE_MAX,
};

enum {
	DP_LINKRATE_RBR		= 0x6,
	DP_LINKRATE_HBR		= 0xa,
	DP_LINKRATE_HBR2	= 0x14,
	DP_LINKRATE_HBR25	= 0x19,
	DP_LINKRATE_HBR3	= 0x1e,
};

enum {
	DP_COLOR_FORMAT_RGB_444		= 0,
	DP_COLOR_FORMAT_YUV_422		= 1,
	DP_COLOR_FORMAT_YUV_444		= 2,
	DP_COLOR_FORMAT_YUV_420		= 3,
	DP_COLOR_FORMAT_YONLY		= 4,
	DP_COLOR_FORMAT_RAW		= 5,
	DP_COLOR_FORMAT_RESERVED	= 6,
	DP_COLOR_FORMAT_DEFAULT		= DP_COLOR_FORMAT_RGB_444,
	DP_COLOR_FORMAT_UNKNOWN		= 15,
};

enum {
	DP_COLOR_DEPTH_6BIT	= 0,
	DP_COLOR_DEPTH_8BIT	= 1,
	DP_COLOR_DEPTH_10BIT	= 2,
	DP_COLOR_DEPTH_12BIT	= 3,
	DP_COLOR_DEPTH_16BIT	= 4,
	DP_COLOR_DEPTH_UNKNOWN	= 5,
};

bool dptx_hal_hpd_high(struct mtk_dp *mtk_dp);
bool dptx_hal_auxread_bytes(struct mtk_dp *mtk_dp, u8 cmd,
			    u32 dpcd_addr, size_t length, u8 *rx_buf);
bool dptx_hal_auxwrite_bytes(struct mtk_dp *mtk_dp, u8 cmd,
			     u32 dpcd_addr, size_t length, u8 *data);
bool dptx_hal_setswing_preemphasis(struct mtk_dp *mtk_dp, int lane_num,
				   int swing_value, int preemphasis);
u8 dptx_hal_get_colorbpp(struct mtk_dp *mtk_dp);
u32 mtk_dp_read(struct mtk_dp *mtk_dp, u32 offset);
void mtk_dp_write_byte(struct mtk_dp *mtk_dp, u32 addr, u8 val, u32 mask);
void mtk_dp_mask(struct mtk_dp *mtk_dp, u32 offset, u32 val, u32 mask);
void mtk_dp_write(struct mtk_dp *mtk_dp, u32 offset, u32 val);
void dptx_hal_verify_clock(struct mtk_dp *mtk_dp);
void dptx_hal_reset_swing_preemphasis(struct mtk_dp *mtk_dp);
void dptx_hal_digital_swreset(struct mtk_dp *mtk_dp);
void dptx_hal_ssc_en(struct mtk_dp *mtk_dp, bool enable);
void dptx_hal_hpd_int_en(struct mtk_dp *mtk_dp, bool enable);
void dptx_hal_hpd_detect_setting(struct mtk_dp *mtk_dp);
void dptx_hal_phy_setting(struct mtk_dp *mtk_dp);
void dptx_hal_aux_setting(struct mtk_dp *mtk_dp);
void dptx_hal_digital_setting(struct mtk_dp *mtk_dp);
void dptx_hal_set_txlane(struct mtk_dp *mtk_dp, int value);
void dptx_hal_phy_setidlepattern(struct mtk_dp *mtk_dp, bool enable);
void dptx_hal_phyd_reset(struct mtk_dp *mtk_dp);
void dptx_hal_set_txtrainingpattern(struct mtk_dp *mtk_dp, int value);
void dptx_hal_set_ef_mode(struct mtk_dp *mtk_dp, bool enable);
void dptx_hal_setscramble(struct mtk_dp *mtk_dp, bool enable);
void dptx_hal_init_setting(struct mtk_dp *mtk_dp);
void dptx_hal_videomute(struct mtk_dp *mtk_dp, bool enable);
void dptx_hal_bypassmsa_en(struct mtk_dp *mtk_dp, bool enable);
void dptx_hal_overwrite_mn(struct mtk_dp *mtk_dp, bool enable,
			   u32 video_m, u32 video_n);
void dptx_hal_settu_sramrd_start(struct mtk_dp *mtk_dp, u16 value);
void dptx_hal_setsdp_downcnt_init_inhblanking(struct mtk_dp *mtk_dp, u16 value);
void dptx_hal_setsdp_downcnt_init(struct mtk_dp *mtk_dp, u16 value);
void dptx_hal_settu_setencoder(struct mtk_dp *mtk_dp);
void dptx_hal_set_msa(struct mtk_dp *mtk_dp);
void dptx_hal_setmisc(struct mtk_dp *mtk_dp, u8 cmisc[2]);
void dptx_hal_set_color_depth(struct mtk_dp *mtk_dp, u8 color_depth);
void dptx_hal_set_color_format(struct mtk_dp *mtk_dp, u8 color_format);
void dptx_hal_set_txrate(struct mtk_dp *mtk_dp, int value);
void dptx_hal_analog_power_en(struct mtk_dp *mtk_dp, bool enable);

#endif /* SOC_MEDIATEK_COMMON_DP_DPTX_HAL_H */
