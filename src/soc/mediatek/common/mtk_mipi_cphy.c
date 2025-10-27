/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <device/mmio.h>
#include <delay.h>
#include <soc/dsi.h>
#include <soc/pll.h>
#include <types.h>

#define DE_EMPHASIS_EN		BIT(9)
#define DSI_CPHY_EN		BIT(3)
#define DSI_HSTX_LDO_REF_SEL	GENMASK(9, 6)
#define HFP_HS_EN		BIT(31)

#define MIPITX_CPHY_LANE_SEL0_SETTING	0x65432101
#define MIPITX_CPHY_LANE_SEL1_SETTING	0x24210987
#define MIPITX_CPHY_LANE_SEL2_SETTING	0x68543102
#define MIPITX_CPHY_LANE_SEL3_SETTING	0x00000007

void mtk_dsi_cphy_lane_sel_setting(void)
{
	write32(&mipi_tx->phy_sel[0], MIPITX_CPHY_LANE_SEL0_SETTING);
	write32(&mipi_tx->phy_sel[1], MIPITX_CPHY_LANE_SEL1_SETTING);
	write32(&mipi_tx->phy_sel[2], MIPITX_CPHY_LANE_SEL2_SETTING);
	write32(&mipi_tx->phy_sel[3], MIPITX_CPHY_LANE_SEL3_SETTING);
}

void mtk_dsi_cphy_enable(void)
{
	setbits32(&mipi_tx->lane_con, DSI_CPHY_EN);
}

void mtk_dsi_cphy_disable_ck_mode(void)
{
	clrsetbits32(&mipi_tx->voltage_sel, DSI_HSTX_LDO_REF_SEL, 0xF << 6);
	clrbits32(&mipi_tx->ck_ckmode_en, DSI_CK_CKMODE_EN);
	setbits32(&mipi_tx->lane_con, DE_EMPHASIS_EN);
}

void mtk_dsi_cphy_enable_cmdq_6byte(void)
{
	clrbits32(&dsi0->dsi_cmd_type1_hs, CMD_CPHY_6BYTE_EN);
}

void mtk_dsi_cphy_timing(u32 data_rate, struct mtk_phy_timing *timing)
{
	u32 cycle_time, value;

	data_rate = data_rate / MHz;
	cycle_time = 7000 / data_rate;
	printk(BIOS_INFO, "cycle_time = %d data_rate= %d\n", cycle_time, data_rate);
	/* spec. lpx > 50ns */
	timing->lpx = DIV_ROUND_UP(75, cycle_time);
	/* spec.  38ns < hs_prpr < 95ns */
	timing->da_hs_prepare = DIV_ROUND_UP(64, cycle_time);
	/* spec.  7ui < hs_zero(prebegin) < 448ui */
	timing->da_hs_zero = 48;
	/* spec.  7ui < hs_trail(post) < 224ui */
	timing->da_hs_trail = 32;

	/* spec. ta_get = 5*lpx */
	timing->ta_get = 5 * timing->lpx;
	/* spec. ta_sure = 1.5*lpx */
	timing->ta_sure = 3 * timing->lpx / 2;
	/* spec. ta_go = 4*lpx */
	timing->ta_go = 4 * timing->lpx;
	/* spec. da_hs_exit > 100ns */
	timing->da_hs_exit = DIV_ROUND_UP(125, cycle_time);

	/* Allow board-specific tuning. */
	mtk_dsi_override_phy_timing(timing);

	value = timing->lpx | timing->da_hs_prepare << 8 |
		  timing->da_hs_zero << 16 | timing->da_hs_trail << 24;
	write32(&dsi0->dsi_phy_timecon0, value);

	value = timing->ta_go | timing->ta_sure << 8 |
		  timing->ta_get << 16 | timing->da_hs_exit << 24;
	write32(&dsi0->dsi_phy_timecon1, value);

	write32(&dsi0->dsi_cphy_con0, 0x012C0003);
	write32(&dsi0->dsi_bllp_wc, 16 * 3);
}

void mtk_dsi_cphy_vdo_timing(const u32 lanes,
			     const struct edid *edid,
			     const struct mtk_phy_timing *phy_timing,
			     const u32 bytes_per_pixel,
			     const u32 hbp,
			     const u32 hfp,
			     s32 *hbp_byte,
			     s32 *hfp_byte,
			     u32 *hsync_active_byte)
{
	s32 active_byte, phy_cycle, phy_trail, tmp;
	u32 hs_vb_ps_wc, ps_wc, data_phy_cycles;

	active_byte = edid->mode.hspw * bytes_per_pixel - 10 * lanes - 26;
	*hsync_active_byte = MAX(4, active_byte);
	active_byte =  hbp * bytes_per_pixel - 12 * lanes - 26;
	*hbp_byte = MAX(4, active_byte);
	data_phy_cycles = phy_timing->lpx + phy_timing->da_hs_prepare +
				phy_timing->da_hs_zero + phy_timing->da_hs_exit + 5;
	phy_cycle = 8 * lanes + 28 + 2 * data_phy_cycles * lanes;
	phy_trail = 2 * (phy_timing->da_hs_trail + 1) * lanes - 6 * lanes - 14;
	tmp = hfp * bytes_per_pixel - phy_cycle;
	*hfp_byte = MIN(MAX(8, tmp), phy_trail);
	ps_wc = edid->mode.ha * bytes_per_pixel;
	hs_vb_ps_wc = ps_wc - (phy_timing->lpx + phy_timing->da_hs_exit +
		phy_timing->da_hs_prepare + phy_timing->da_hs_zero + 2) * lanes;
	*hfp_byte |= hs_vb_ps_wc << 16 | HFP_HS_EN;
}
