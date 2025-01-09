/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <edid.h>
#include <soc/dptx.h>
#include <soc/dptx_hal.h>
#include <soc/dptx_reg.h>
#include <types.h>

struct shift_mask {
	u32 shift;
	u32 mask;
};
static const struct shift_mask volt_swing[DPTX_LANE_MAX] = {
	[DPTX_LANE0] = { DP_TX0_VOLT_SWING_FLDMASK_POS, DP_TX0_VOLT_SWING_FLDMASK },
	[DPTX_LANE1] = { DP_TX1_VOLT_SWING_FLDMASK_POS, DP_TX1_VOLT_SWING_FLDMASK },
	[DPTX_LANE2] = { DP_TX2_VOLT_SWING_FLDMASK_POS, DP_TX2_VOLT_SWING_FLDMASK },
	[DPTX_LANE3] = { DP_TX3_VOLT_SWING_FLDMASK_POS, DP_TX3_VOLT_SWING_FLDMASK },
};
static const struct shift_mask volt_preemphasis[DPTX_LANE_MAX] = {
	[DPTX_LANE0] = { DP_TX0_PRE_EMPH_FLDMASK_POS, DP_TX0_PRE_EMPH_FLDMASK },
	[DPTX_LANE1] = { DP_TX1_PRE_EMPH_FLDMASK_POS, DP_TX1_PRE_EMPH_FLDMASK },
	[DPTX_LANE2] = { DP_TX2_PRE_EMPH_FLDMASK_POS, DP_TX2_PRE_EMPH_FLDMASK },
	[DPTX_LANE3] = { DP_TX3_PRE_EMPH_FLDMASK_POS, DP_TX3_PRE_EMPH_FLDMASK },
};

void dptx_hal_init_setting(struct mtk_dp *mtk_dp)
{
	DP_WRITE1BYTE(mtk_dp, REG_342C_DP_TRANS_P0, 0x69);
	mtk_dp_mask(mtk_dp, REG_3540_DP_TRANS_P0, BIT(3), BIT(3));
	mtk_dp_mask(mtk_dp, REG_31EC_DP_ENCODER0_P0, BIT(4), BIT(4));
	mtk_dp_mask(mtk_dp, REG_304C_DP_ENCODER0_P0, 0, BIT(8));
	mtk_dp_mask(mtk_dp, DP_TX_TOP_IRQ_MASK, BIT(2), BIT(2));
}

void dptx_hal_set_color_format(struct mtk_dp *mtk_dp, u8 out_format)
{
	/* MISC0 */
	mtk_dp_write_byte(mtk_dp, REG_3034_DP_ENCODER0_P0,
			  out_format << 0x1, GENMASK(2, 1));

	switch (out_format) {
	case DP_COLOR_FORMAT_RGB_444:
	case DP_COLOR_FORMAT_YUV_444:
		mtk_dp_write_byte(mtk_dp, REG_303C_DP_ENCODER0_P0 + 1,
				  0, GENMASK(6, 4));
		break;
	case DP_COLOR_FORMAT_YUV_422:
		mtk_dp_write_byte(mtk_dp, REG_303C_DP_ENCODER0_P0 + 1,
				  BIT(4), GENMASK(6, 4));
		break;
	case DP_COLOR_FORMAT_YUV_420:
		mtk_dp_write_byte(mtk_dp, REG_303C_DP_ENCODER0_P0 + 1, BIT(5),
				  GENMASK(6, 4));
		break;
	default:
		break;
	}
}

void dptx_hal_settu_setencoder(struct mtk_dp *mtk_dp)
{
	mtk_dp_write_byte(mtk_dp, REG_303C_DP_ENCODER0_P0 + 1,
			  BIT(7), BIT(7));
	DP_WRITE2BYTE(mtk_dp, REG_3040_DP_ENCODER0_P0, 0x2020);
	DP_WRITE2BYTE(mtk_dp, REG_3364_DP_ENCODER1_P0, 0x2020);
	mtk_dp_write_byte(mtk_dp, REG_3300_DP_ENCODER1_P0 + 1,
			  0x2, BIT(1) | BIT(0));
	mtk_dp_write_byte(mtk_dp, REG_3364_DP_ENCODER1_P0 + 1,
			  0x40, 0x70);
	DP_WRITE2BYTE(mtk_dp, REG_3368_DP_ENCODER1_P0, 0x1111);
}

bool dptx_hal_hpd_high(struct mtk_dp *mtk_dp)
{
	return mtk_dp_read(mtk_dp, REG_3414_DP_TRANS_P0) & BIT(2);
}

bool dptx_hal_setswing_preemphasis(struct mtk_dp *mtk_dp, int lane_num,
				   int swing_value, int preemphasis)
{
	printk(BIOS_DEBUG, "lane(%d), set swing(%#x), emp(%#x)\n",
	       lane_num, swing_value, preemphasis);

	if (lane_num >= DPTX_LANE_MAX) {
		printk(BIOS_ERR, "invalid lane number: %d\n", lane_num);
		return false;
	}

	mtk_dp_mask(mtk_dp, DP_TX_TOP_SWING_EMP,
		    swing_value << volt_swing[lane_num].shift,
		    volt_swing[lane_num].mask);
	mtk_dp_mask(mtk_dp, DP_TX_TOP_SWING_EMP,
		    preemphasis << volt_preemphasis[lane_num].shift,
		    volt_preemphasis[lane_num].mask);
	return true;
}

void dptx_hal_reset_swing_preemphasis(struct mtk_dp *mtk_dp)
{
	int lane;

	for (lane = 0; lane < DPTX_LANE_MAX; lane++)
		mtk_dp_mask(mtk_dp, DP_TX_TOP_SWING_EMP,
			    0, volt_swing[lane].mask);
	for (lane = 0; lane < DPTX_LANE_MAX; lane++)
		mtk_dp_mask(mtk_dp, DP_TX_TOP_SWING_EMP,
			    0, volt_preemphasis[lane].mask);
}

void dptx_hal_hpd_detect_setting(struct mtk_dp *mtk_dp)
{
	mtk_dp_write_byte(mtk_dp, REG_3410_DP_TRANS_P0,
			  0x8, GENMASK(3, 0));
	mtk_dp_write_byte(mtk_dp, REG_3410_DP_TRANS_P0,
			  0xa << 4, GENMASK(7, 4));

	DP_WRITE1BYTE(mtk_dp, REG_3410_DP_TRANS_P0 + 1, 0x55);
	DP_WRITE1BYTE(mtk_dp, REG_3430_DP_TRANS_P0, 0x2);
}

void dptx_hal_phy_setting(struct mtk_dp *mtk_dp)
{
	mtk_dp_mask(mtk_dp, DP_TX_TOP_PWR_STATE,
		    0x3 << DP_PWR_STATE_FLDMASK_POS, DP_PWR_STATE_FLDMASK);

	mtk_dp_write(mtk_dp, 0x2000, 0x00000001);
	mtk_dp_write(mtk_dp, 0x103c, 0x00000000);
	mtk_dp_write(mtk_dp, 0x2000, 0x00000003);
	mtk_dp_write(mtk_dp, 0x1138, 0x20181410);
	mtk_dp_write(mtk_dp, 0x1238, 0x20181410);
	mtk_dp_write(mtk_dp, 0x1338, 0x20181410);
	mtk_dp_write(mtk_dp, 0x1438, 0x20181410);
	mtk_dp_write(mtk_dp, 0x113C, 0x20241e18);
	mtk_dp_write(mtk_dp, 0x123C, 0x20241e18);
	mtk_dp_write(mtk_dp, 0x133C, 0x20241e18);
	mtk_dp_write(mtk_dp, 0x143C, 0x20241e18);
	mtk_dp_write(mtk_dp, 0x1140, 0x00003028);
	mtk_dp_write(mtk_dp, 0x1240, 0x00003028);
	mtk_dp_write(mtk_dp, 0x1340, 0x00003028);
	mtk_dp_write(mtk_dp, 0x1440, 0x00003028);
	mtk_dp_write(mtk_dp, 0x1144, 0x10080400);
	mtk_dp_write(mtk_dp, 0x1244, 0x10080400);
	mtk_dp_write(mtk_dp, 0x1344, 0x10080400);
	mtk_dp_write(mtk_dp, 0x1444, 0x10080400);
	mtk_dp_write(mtk_dp, 0x1148, 0x000c0600);
	mtk_dp_write(mtk_dp, 0x1248, 0x000c0600);
	mtk_dp_write(mtk_dp, 0x1348, 0x000c0600);
	mtk_dp_write(mtk_dp, 0x1448, 0x000c0600);
	mtk_dp_write(mtk_dp, 0x114C, 0x00000008);
	mtk_dp_write(mtk_dp, 0x124C, 0x00000008);
	mtk_dp_write(mtk_dp, 0x134C, 0x00000008);
	mtk_dp_write(mtk_dp, 0x144C, 0x00000008);
	mtk_dp_mask(mtk_dp, 0x3690, BIT(8), BIT(8));
}

void dptx_hal_ssc_en(struct mtk_dp *mtk_dp, bool enable)
{
	mtk_dp_mask(mtk_dp, 0x2000, BIT(0), GENMASK(1, 0));

	if (enable)
		mtk_dp_mask(mtk_dp, 0x1014, BIT(3), BIT(3));
	else
		mtk_dp_mask(mtk_dp, 0x1014, 0x0, BIT(3));

	mtk_dp_mask(mtk_dp, 0x2000, GENMASK(1, 0), GENMASK(1, 0));

	mdelay(1);
}

void dptx_hal_aux_setting(struct mtk_dp *mtk_dp)
{
	/* [12 : 8]: modify timeout threshold = 1595 */
	mtk_dp_mask(mtk_dp, REG_360C_AUX_TX_P0,
		    0x1595, AUX_TIMEOUT_THR_AUX_TX_P0_FLDMASK);
	mtk_dp_write_byte(mtk_dp, REG_3658_AUX_TX_P0, 0, BIT(0));

	/* 0x19 for 26M */
	DP_WRITE1BYTE(mtk_dp, REG_3634_AUX_TX_P0 + 1, 0x19);
	/* 0xd for 26M */
	mtk_dp_write_byte(mtk_dp, REG_3614_AUX_TX_P0,
			  0xd, GENMASK(6, 0));
	mtk_dp_mask(mtk_dp, REG_37C8_AUX_TX_P0,
		    0x01 << MTK_ATOP_EN_AUX_TX_P0_FLDMASK_POS,
		    MTK_ATOP_EN_AUX_TX_P0_FLDMASK);
}

void dptx_hal_digital_setting(struct mtk_dp *mtk_dp)
{
	mtk_dp_write_byte(mtk_dp, REG_304C_DP_ENCODER0_P0,
			  0, VBID_VIDEO_MUTE_DP_ENCODER0_P0_FLDMASK);
	/* MISC0 */
	dptx_hal_set_color_format(mtk_dp, DP_COLOR_FORMAT_RGB_444);

	dptx_hal_set_color_depth(mtk_dp, DP_COLOR_DEPTH_8BIT);
	mtk_dp_write_byte(mtk_dp, REG_3368_DP_ENCODER1_P0 + 1,
			  BIT(4), GENMASK(5, 4));
	/* DPtx encoder reset all sw. */
	mtk_dp_write_byte(mtk_dp, REG_3004_DP_ENCODER0_P0 + 1, BIT(1), BIT(1));

	mdelay(1);

	/* DPtx encoder reset all sw. */
	mtk_dp_write_byte(mtk_dp, REG_3004_DP_ENCODER0_P0 + 1, 0, BIT(1));
}

void dptx_hal_digital_swreset(struct mtk_dp *mtk_dp)
{
	mtk_dp_write_byte(mtk_dp, REG_340C_DP_TRANS_P0 + 1, BIT(5), BIT(5));
	mdelay(1);
	mtk_dp_write_byte(mtk_dp, REG_340C_DP_TRANS_P0 + 1, 0, BIT(5));
}

void dptx_hal_phyd_reset(struct mtk_dp *mtk_dp)
{
	mtk_dp_write_byte(mtk_dp, 0x1038, 0, BIT(0));
	mdelay(1);
	mtk_dp_write_byte(mtk_dp, 0x1038, BIT(0), BIT(0));
}

void dptx_hal_set_txlane(struct mtk_dp *mtk_dp, int value)
{
	if (value == 0)
		mtk_dp_write_byte(mtk_dp, REG_35F0_DP_TRANS_P0,
				  0, BIT(3) | BIT(2));
	else
		mtk_dp_write_byte(mtk_dp, REG_35F0_DP_TRANS_P0,
				  BIT(3), BIT(3) | BIT(2));

	if ((value << 2) <= UINT8_MAX) {
		mtk_dp_write_byte(mtk_dp, REG_3000_DP_ENCODER0_P0,
				  value, BIT(1) | BIT(0));
		mtk_dp_write_byte(mtk_dp, REG_34A4_DP_TRANS_P0,
				  value << 2, BIT(3) | BIT(2));
	} else {
		printk(BIOS_ERR, "[%s]value << 2 > 0xff\n", __func__);
	}
}

void dptx_hal_set_txrate(struct mtk_dp *mtk_dp, int value)
{
	/* Power off TPLL and lane */
	mtk_dp_write(mtk_dp, 0x2000, 0x00000001);
	/* Set gear : 0x0 : RBR, 0x1 : HBR, 0x2 : HBR2, 0x3 : HBR3 */
	switch (value) {
	case DP_LINKRATE_RBR:
		mtk_dp_write(mtk_dp, 0x103C, 0x0);
		break;
	case DP_LINKRATE_HBR:
		mtk_dp_write(mtk_dp, 0x103C, 0x1);
		break;
	case DP_LINKRATE_HBR2:
		mtk_dp_write(mtk_dp, 0x103C, 0x2);
		break;
	case DP_LINKRATE_HBR3:
		mtk_dp_write(mtk_dp, 0x103C, 0x3);
		break;
	default:
		printk(BIOS_ERR, "Link rate not support(%d)\n", value);
		break;
	}

	/* Power on BandGap, TPLL and Lane */
	mtk_dp_write(mtk_dp, 0x2000, 0x3);
}
