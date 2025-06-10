/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <edid.h>
#include <soc/dptx.h>
#include <soc/dptx_hal.h>
#include <soc/dptx_reg.h>
#include <types.h>

const int dptx_hal_driving_offset[] = {
	PHYD_DIG_LAN0_OFFSET + DRIVING_FORCE,
	PHYD_DIG_LAN1_OFFSET + DRIVING_FORCE,
	PHYD_DIG_LAN2_OFFSET + DRIVING_FORCE,
	PHYD_DIG_LAN3_OFFSET + DRIVING_FORCE,
};

const int dptx_hal_driving_offset_size = ARRAY_SIZE(dptx_hal_driving_offset);

void dptx_hal_init_setting(struct mtk_dp *mtk_dp)
{
	mtk_dp_mask(mtk_dp, 0x2000, GENMASK(1, 0), GENMASK(1, 0));

	mtk_dp_mask(mtk_dp, REG_3540_DP_TRANS_P0, BIT(3), BIT(3));
	mtk_dp_mask(mtk_dp, REG_342C_DP_TRANS_P0, 0x68, 0xFF);
	mtk_dp_mask(mtk_dp, REG_31EC_DP_ENCODER0_P0, BIT(4), BIT(4));
	mtk_dp_mask(mtk_dp, REG_304C_DP_ENCODER0_P0, 0, BIT(8));
	mtk_dp_mask(mtk_dp, REG_304C_DP_ENCODER0_P0, BIT(3), BIT(3));

	/* default marker */
	mtk_dp_mask(mtk_dp, REG_3F04_DP_ENC_4P_3, 0, 0xFFFF);
	mtk_dp_mask(mtk_dp, REG_3F08_DP_ENC_4P_3, BIT(3), BIT(3));
	mtk_dp_mask(mtk_dp, REG_3F0C_DP_ENC_4P_3, BIT(1), BIT(1));
	mtk_dp_mask(mtk_dp, REG_3F10_DP_ENC_4P_3, BIT(3), BIT(3));

	mtk_dp_mask(mtk_dp, REG_33C0_DP_ENCODER1_P0, 0, 0xF000);
	mtk_dp_mask(mtk_dp, REG_33C0_DP_ENCODER1_P0, 0x80, 0x80);
	mtk_dp_mask(mtk_dp, REG_33C4_DP_ENCODER1_P0, BIT(5), BIT(6) | BIT(5));
	mtk_dp_mask(mtk_dp, REG_3F28_DP_ENC_4P_3, 0xA << 2, GENMASK(5, 2));
	mtk_dp_mask(mtk_dp, DP_TX_TOP_RESET_AND_PROBE, 0x9100FF, 0x9100FF);

	mtk_dp_mask(mtk_dp, MTK_DP_TOP_IRQ_MASK, ENCODER_IRQ_MSK | TRANS_IRQ_MSK,
		    ENCODER_IRQ_MSK | TRANS_IRQ_MSK);
}

void dptx_hal_set_color_format(struct mtk_dp *mtk_dp, u8 out_format)
{
	u32 val;

	/* MISC0 */
	mtk_dp_mask(mtk_dp, REG_3034_DP_ENCODER0_P0, out_format << DP_TEST_COLOR_FORMAT_SHIFT,
		    DP_TEST_COLOR_FORMAT_MASK);
	mtk_dp_mask(mtk_dp, REG_3034_DP_ENCODER0_P0, BIT(3), BIT(3));

	switch (out_format) {
	case DP_COLOR_FORMAT_RGB_444:
	case DP_COLOR_FORMAT_YUV_444:
		val = 0;
		break;
	case DP_COLOR_FORMAT_YUV_422:
		val = 0x1;
		break;
	case DP_COLOR_FORMAT_YUV_420:
		val = 0x2;
		break;
	default:
		printk(BIOS_ERR, "Unsupported color format: %u\n", out_format);
		return;
	}
	mtk_dp_mask(mtk_dp, REG_303C_DP_ENCODER0_P0, val << 12, 0x7 << 12);
}

void dptx_hal_settu_setencoder(struct mtk_dp *mtk_dp)
{
	mtk_dp_mask(mtk_dp, REG_303C_DP_ENCODER0_P0, BIT(15), BIT(15));
	DP_WRITE2BYTE(mtk_dp, REG_3040_DP_ENCODER0_P0, 0x2020);
	DP_WRITE2BYTE(mtk_dp, REG_3364_DP_ENCODER1_P0, 0x2020);
	mtk_dp_mask(mtk_dp, REG_3300_DP_ENCODER1_P0, 0x2 << 8, 0x3 << 8);
	mtk_dp_mask(mtk_dp, REG_3364_DP_ENCODER1_P0, 0x40 << 8, 0x70 << 8);
}

bool dptx_hal_hpd_high(struct mtk_dp *mtk_dp)
{
	return mtk_dp_read(mtk_dp, REG_364C_AUX_TX_P0) & BIT(15);
}

bool dptx_hal_setswing_preemphasis(struct mtk_dp *mtk_dp, int lane_num, u8 swing_value,
				   u8 preemphasis)
{
	assert(lane_num <= DPTX_LANE_MAX);

	for (int i = 0; i < lane_num; ++i) {
		mtk_dp_phy_mask(mtk_dp, dptx_hal_driving_offset[i],
				swing_value << EDP_TX_LN_VOLT_SWING_VAL_SHIFT |
				preemphasis << EDP_TX_LN_PRE_EMPH_VAL_SHIFT,
				EDP_TX_LN_VOLT_SWING_VAL_MASK | EDP_TX_LN_PRE_EMPH_VAL_MASK);
		printk(BIOS_DEBUG, "lane(%d), set swing(%u), emp(%u)\n",
		       i, swing_value, preemphasis);
	}
	mtk_dp_mask(mtk_dp, 0x2000, GENMASK(1, 0), GENMASK(1, 0));
	return true;
}

void dptx_hal_hpd_detect_setting(struct mtk_dp *mtk_dp)
{
	mtk_dp_mask(mtk_dp, REG_364C_AUX_TX_P0, HPD_INT_THD_FLDMASK_VAL << 4,
		    HPD_INT_THD_FLDMASK);
}

void dptx_hal_phy_init(struct mtk_dp *mtk_dp)
{
	mtk_dp_phy_mask(mtk_dp, IPMUX_CONTROL, 0 << EDPTX_DSI_PHYD_SEL_FLDMASK_POS,
			EDPTX_DSI_PHYD_SEL_FLDMASK);

	mtk_dp_phy_mask(mtk_dp, PHYD_DIG_GLB_OFFSET + 0x10, GENMASK(2, 0), GENMASK(2, 0));
	dptx_hal_ssc_en(mtk_dp, false);
}

void dptx_hal_set_txrate(struct mtk_dp *mtk_dp, u8 value)
{
	printk(BIOS_INFO, "Link rate = 0x%x\n", value);
	switch (value) {
	case DP_LINKRATE_RBR:
		mtk_dp_phy_write(mtk_dp, 0x143C, DP_LINKRATE_RBR_VAL);
		break;
	case DP_LINKRATE_HBR:
		mtk_dp_phy_write(mtk_dp, 0x143C, DP_LINKRATE_HBR_VAL);
		break;
	case DP_LINKRATE_HBR2:
		mtk_dp_phy_write(mtk_dp, 0x143C, DP_LINKRATE_HBR2_VAL);
		break;
	case DP_LINKRATE_HBR3:
		mtk_dp_phy_write(mtk_dp, 0x143C, DP_LINKRATE_HBR3_VAL);
		break;
	default:
		printk(BIOS_ERR, "Link rate not support(%d)\n", value);
		return;
	}
}

void dptx_hal_phy_setting(struct mtk_dp *mtk_dp)
{
	u8 link_rate = mtk_dp->train_info.linkrate;
	u8 lane_count = mtk_dp->train_info.linklane_count;

	dptx_hal_phy_set_lanes(mtk_dp, lane_count);
	dptx_hal_set_txrate(mtk_dp, link_rate);

	/* Turn on phy power after phy configure */
	mtk_dp_mask(mtk_dp, REG_3FF8_DP_ENC_4P_3, PHY_STATE_W_1_DP_ENC_4P_3,
		    PHY_STATE_W_1_DP_ENC_4P_3_MASK);
	mtk_dp_mask(mtk_dp, 0x2000, GENMASK(1, 0), GENMASK(1, 0));
	printk(BIOS_DEBUG, "[eDPTX] DP_PHY_DIG_PLL_CTL_1:%#x\n",
	       mtk_dp_phy_read(mtk_dp, DP_PHY_DIG_PLL_CTL_1));
}

void dptx_hal_ssc_en(struct mtk_dp *mtk_dp, bool enable)
{
	if (enable) {
		printk(BIOS_DEBUG, "[eDPTX] enable ssc\n");
		mtk_dp_phy_mask(mtk_dp, DP_PHY_DIG_PLL_CTL_1, BIT(8), BIT(8));
	} else {
		printk(BIOS_DEBUG, "[eDPTX] disable ssc\n");
		mtk_dp_phy_mask(mtk_dp, DP_PHY_DIG_PLL_CTL_1, 0x0, BIT(8));
	}
	printk(BIOS_DEBUG, "[eDPTX] DP_PHY_DIG_PLL_CTL_1:%#x\n",
	       mtk_dp_phy_read(mtk_dp, DP_PHY_DIG_PLL_CTL_1));

	mdelay(1);
}

void dptx_hal_aux_setting(struct mtk_dp *mtk_dp)
{
	/* Modify timeout threshold = 1595 [12 : 8] */
	mtk_dp_mask(mtk_dp, REG_360C_AUX_TX_P0, 0x1D0C, 0x1FFF);
	mtk_dp_mask(mtk_dp, REG_3658_AUX_TX_P0, 0, BIT(0));
	/* [0]mtk_dp, REG_aux_tx_ov_en */
	mtk_dp_mask(mtk_dp, REG_36A0_AUX_TX_4P, 0xFFFC, 0xFFFC);

	/* 26M */
	mtk_dp_mask(mtk_dp, REG_3634_AUX_TX_P0, AUX_TX_OVER_SAMPLE_RATE_FOR_26M << 8,
		    0xff << 8);
	mtk_dp_mask(mtk_dp, REG_3614_AUX_TX_P0, AUX_RX_UI_CNT_THR_AUX_FOR_26M, 0x7f);

	/* Modify, 13 for 26M */
	mtk_dp_mask(mtk_dp, REG_37C8_AUX_TX_P0, MTK_ATOP_EN_AUX_TX_4P, MTK_ATOP_EN_AUX_TX_4P);
	/* Disable aux sync_stop detect function */
	mtk_dp_mask(mtk_dp, REG_3690_AUX_TX_P0, RX_REPLY_COMPLETE_MODE_AUX_TX_4P,
		    RX_REPLY_COMPLETE_MODE_AUX_TX_4P);

	mtk_dp_mask(mtk_dp, REG_367C_AUX_TX_P0, HPD_CONN_THD_AUX_TX_P0_FLDMASK_POS << 6,
		    HPD_CONN_THD_AUX_TX_P0_FLDMASK);

	mtk_dp_mask(mtk_dp, REG_37A0_AUX_TX_P0, HPD_DISC_THD_AUX_TX_P0_FLDMASK_POS << 4,
		    HPD_DISC_THD_AUX_TX_P0_FLDMASK);
	mtk_dp_mask(mtk_dp, REG_3690_AUX_TX_P0, RX_REPLY_COMPLETE_MODE_AUX_TX_4P,
		    RX_REPLY_COMPLETE_MODE_AUX_TX_4P);
	mtk_dp_mask(mtk_dp, REG_3FF8_DP_ENC_4P_3, XTAL_FREQ_FOR_PSR_DP_ENC_4P_3_VALUE << 9,
		    XTAL_FREQ_FOR_PSR_DP_ENC_4P_3_MASK);
	mtk_dp_mask(mtk_dp, REG_366C_AUX_TX_P0, XTAL_FREQ_DP_TX_AUX_366C_VALUE << 8,
		    XTAL_FREQ_DP_TX_AUX_366C_MASK);
}

void dptx_hal_digital_setting(struct mtk_dp *mtk_dp)
{
	mtk_dp_mask(mtk_dp, REG_304C_DP_ENCODER0_P0, 0, VBID_VIDEO_MUTE_DP_ENC0_4P_MASK);
	mtk_dp_mask(mtk_dp, REG_3368_DP_ENCODER1_P0, BS2BS_MODE_DP_ENC1_4P_VAL << 12,
		    0x3 << 12);
	mtk_dp_mask(mtk_dp, REG_3030_DP_ENCODER0_P0, BIT(11), BIT(11));

	/* dp I-mode enable */
	mtk_dp_mask(mtk_dp, BS2BS_MODE_DP_ENCODER1_P0_FLDMASK, DP_I_MODE_ENABLE,
		    DP_I_MODE_ENABLE);
	mtk_dp_mask(mtk_dp, REG_3030_DP_ENCODER0_P0, 0x3ff, 0x3ff);
	mtk_dp_mask(mtk_dp, REG_3028_DP_ENCODER0_P0, 0, BIT(15));
	mtk_dp_mask(mtk_dp, REG_302C_DP_ENCODER0_P0, 0, BIT(15));

	/* set color format */
	dptx_hal_set_color_format(mtk_dp, DP_COLOR_FORMAT_RGB_444);

	/* set color depth */
	dptx_hal_set_color_depth(mtk_dp, DP_COLOR_DEPTH_8BIT);

	/* reg_bs)symbol_cnt_reset */
	mtk_dp_mask(mtk_dp, REG_3000_DP_ENCODER0_P0, REG_BS_SYMBOL_CNT_RESET,
		    REG_BS_SYMBOL_CNT_RESET);
	mtk_dp_mask(mtk_dp, REG_3004_DP_ENCODER0_P0, BIT(14), BIT(14));
	printk(BIOS_DEBUG, "[eDPTX] REG_3004_DP_ENCODER0_P0:%#x\n",
	       mtk_dp_read(mtk_dp, REG_3004_DP_ENCODER0_P0));

	mtk_dp_mask(mtk_dp, REG_3368_DP_ENCODER1_P0, 0x2, BIT(1) | BIT(2));
	mtk_dp_mask(mtk_dp, REG_3368_DP_ENCODER1_P0, BIT(15), BIT(15));
	mtk_dp_mask(mtk_dp, REG_304C_DP_ENCODER0_P0, 0, BIT(8));

	/* [5:0]video sram start address */
	mtk_dp_mask(mtk_dp, REG_303C_DP_ENCODER0_P0, 0x8, 0x3F);

	/* reg_psr_patgen_avt_en patch */
	mtk_dp_mask(mtk_dp, REG_3F80_DP_ENC_4P_3, 0, PSR_PATGEN_AVT_EN_FLDMASK);

	/* phy D enable */
	mtk_dp_mask(mtk_dp, REG_3FF8_DP_ENC_4P_3, PHY_STATE_W_1_DP_ENC_4P_3,
		    PHY_STATE_W_1_DP_ENC_4P_3_MASK);

	/* reg_dvo_on_ow_en */
	mtk_dp_mask(mtk_dp, REG_3FF8_DP_ENC_4P_3, DVO_ON_W_1_FLDMASK, DVO_ON_W_1_FLDMASK);

	/* dp tx encoder reset all sw */
	mtk_dp_mask(mtk_dp, REG_3004_DP_ENCODER0_P0, DP_TX_ENCODER_4P_RESET_SW_DP_ENC0_P0,
		    DP_TX_ENCODER_4P_RESET_SW_DP_ENC0_P0);
	printk(BIOS_DEBUG, "[eDPTX] REG_3004_DP_ENCODER0_P0:%#x\n",
	       mtk_dp_read(mtk_dp, REG_3004_DP_ENCODER0_P0));

	mtk_dp_mask(mtk_dp, REG_3004_DP_ENCODER0_P0, 0, BIT(9));
	printk(BIOS_DEBUG, "[eDPTX] REG_3004_DP_ENCODER0_P0:%#x\n",
	       mtk_dp_read(mtk_dp, REG_3004_DP_ENCODER0_P0));

	mtk_dp_mask(mtk_dp, REG_3FF8_DP_ENC_4P_3, 0xff, 0xff);

	/* Wait for sw reset to complete */
	mdelay(1);
}

void dptx_hal_digital_swreset(struct mtk_dp *mtk_dp)
{
	printk(BIOS_DEBUG, "[eDPTX] DP_PHY_DIG_TX_CTL_0:%#x\n",
	       mtk_dp_phy_read(mtk_dp, DP_PHY_DIG_TX_CTL_0));
	mtk_dp_mask(mtk_dp, REG_340C_DP_TRANS_P0, BIT(13), BIT(13));
	mdelay(1);
	mtk_dp_mask(mtk_dp, REG_340C_DP_TRANS_P0, 0, BIT(13));

	mtk_dp_mask(mtk_dp, EDP_TX_TOP_CLKGEN_0, 0xF, 0xF);
	printk(BIOS_DEBUG, "[eDPTX] DP_PHY_DIG_TX_CTL_0:%#x\n",
	       mtk_dp_phy_read(mtk_dp, DP_PHY_DIG_TX_CTL_0));
}

void dptx_hal_swing_emp_reset(struct mtk_dp *mtk_dp)
{
	mtk_dp_mask(mtk_dp, REG_2004_TOP_SWING_EMP, 0,
		    DP_TX0_VOLT_SWING_MASK | DP_TX1_VOLT_SWING_MASK | DP_TX2_VOLT_SWING_MASK |
		    DP_TX3_VOLT_SWING_MASK | DP_TX0_PRE_EMPH_MASK | DP_TX1_PRE_EMPH_MASK |
		    DP_TX2_PRE_EMPH_MASK | DP_TX3_PRE_EMPH_MASK);
}

void dptx_hal_phy_wait_aux_ldo_ready(struct mtk_dp *mtk_dp)
{
	u32 mask = RGS_BG_CORE_EN_READY_MASK | RGS_AUX_LDO_EN_READY_MASK;

	if (!retry(WAIT_AUX_READY_RETRY_TIMES,
		   (mtk_dp_phy_read(mtk_dp, DP_PHY_DIG_GLB_STATUS_02_OFSSET) & mask) == mask,
		   mdelay(2))) {
		printk(BIOS_ERR, "Wait AUX LDO ready timeout!\n");
	}
}

void dptx_hal_set_txlane(struct mtk_dp *mtk_dp, u8 value)
{
	/* Turn off phy power before phy configure */
	mtk_dp_mask(mtk_dp, REG_3F44_DP_ENC_4P_3, PHY_PWR_STATE_OW_EN_DP_ENC_4P_3,
		    PHY_PWR_STATE_OW_EN_DP_ENC_4P_3_MASK);
	mtk_dp_mask(mtk_dp, REG_3F44_DP_ENC_4P_3, BIAS_POWER_ON,
		    PHY_PWR_STATE_OW_VALUE_DP_ENC_4P_3_MASK);
	dptx_hal_phy_wait_aux_ldo_ready(mtk_dp);
	mtk_dp_mask(mtk_dp, REG_3F44_DP_ENC_4P_3, 0 << 2, PHY_PWR_STATE_OW_EN_DP_ENC_4P_3_MASK);

	if (value == 0)
		mtk_dp_mask(mtk_dp, REG_35F0_DP_TRANS_P0, 0, BIT(3) | BIT(2));
	else
		mtk_dp_mask(mtk_dp, REG_35F0_DP_TRANS_P0, BIT(3), BIT(3) | BIT(2));

	assert((value << 2) <= UINT8_MAX);
	mtk_dp_mask(mtk_dp, REG_3000_DP_ENCODER0_P0, value, BIT(1) | BIT(0));
	mtk_dp_mask(mtk_dp, REG_34A4_DP_TRANS_P0, value << 2, BIT(3) | BIT(2));
}

void dptx_hal_phy_set_swing_preemphasis(struct mtk_dp *mtk_dp, u8 lane_count, u8 *swing_val,
					 u8 *preemphasis)
{
	assert(lane_count <= DPTX_LANE_MAX);

	for (int i = 0; i < lane_count; ++i) {
		mtk_dp_phy_mask(mtk_dp, dptx_hal_driving_offset[i],
				swing_val[i] << EDP_TX_LN_VOLT_SWING_VAL_SHIFT |
				preemphasis[i] << EDP_TX_LN_PRE_EMPH_VAL_SHIFT,
				EDP_TX_LN_VOLT_SWING_VAL_MASK | EDP_TX_LN_PRE_EMPH_VAL_MASK);
		printk(BIOS_DEBUG, "lane(%d), set swing(%u), emp(%u)\n",
		       i, swing_val[i], preemphasis[i]);
	}
}

void dptx_hal_phy_set_idle_pattern(struct mtk_dp *mtk_dp, u8 lane_count, bool enable)
{
	u32 val = 0x0;

	printk(BIOS_INFO, "lane_count: %x idle pattern enable: %d\n", lane_count, enable);
	switch (lane_count) {
	case 1:
		val = POST_MISC_DATA_LANE0_OV_DP_TRANS_4P_MASK;
		break;
	case 2:
		val = POST_MISC_DATA_LANE0_OV_DP_TRANS_4P_MASK |
		      POST_MISC_DATA_LANE1_OV_DP_TRANS_4P_MASK;
		break;
	case 4:
		val = POST_MISC_DATA_LANE0_OV_DP_TRANS_4P_MASK |
		      POST_MISC_DATA_LANE1_OV_DP_TRANS_4P_MASK |
		      POST_MISC_DATA_LANE2_OV_DP_TRANS_4P_MASK |
		      POST_MISC_DATA_LANE3_OV_DP_TRANS_4P_MASK;
		break;
	default:
		printk(BIOS_ERR, "%s: Unexpected lane count %d\n", __func__, lane_count);
		return;
	}
	mtk_dp_mask(mtk_dp, REG_3580_DP_TRANS_P0, (enable ? val : 0x0) << 8,
		    POST_MISC_DATA_LANE_OV_DP_TRANS_4P_MASK);
}
