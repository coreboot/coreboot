/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSI PHY 2.7.0 init for purwa (x1p42100).
 *
 * Ported from UEFI HALdsi_Phy_2_7_0.c.
 * Reference: QcomPkg/Library/HALDSILib/HALdsi_Phy_2_7_0.c
 *
 * Sequence (D-PHY, 4-lane, single DSI):
 *   1. Wait for refgen ready (poll PHY_STATUS bit 0)
 *   2. Power-up: deassert SLEEP_B, power DIGTOP+PLL, set global init regs
 *   3. Lane init: DLN0-3 + CLK lane register writes
 *   4. PLL setup: divider calc -> CommitPll -> PLL lock detect
 *   5. TriggerSleepB -> GlobalClockEnable -> ResyncFIFO
 *   6. Enable DSI clocks (ESC, BYTE, BYTE_INTF, PCLK)
 *   7. Timing parameter calculation and commit
 */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <edid.h>
#include <lib.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/display/dsi_phy.h>
#include <soc/display/dsi_phy_pll.h>
#include <soc/display/mdssreg.h>
#include <soc/display/panel_ops.h>
#include <timer.h>
#include <types.h>

/*
 * Mutable pointers used by static PHY init helpers.
 * Set to DSI0 or DSI1 before calling dsi_phy_init_instance().
 */
static struct dsi_phy_regs *active_phy;
static struct dsi_phy_pll_qlink_regs *active_pll;

/* Timeouts */
#define DSI_PHY_REFGEN_TIMEOUT_US   5000   /* 10 polls * 500us */
#define DSI_PHY_PLL_LOCK_TIMEOUT_US 15000  /* 150 polls * 100us */

/* D-PHY timing formula constants (from UEFI HALdsi_Phy_2_7_0.c) */
#define TIPX_NUMER        1000000000  /* 1e9 for ns->ps conversion */
#define TCLK_PREPARE_MIN  38
#define TCLK_PREPARE_MAX  95
#define THS_PREPARE_MIN_A 40
#define THS_PREPARE_MIN_B 4
#define THS_PREPARE_MAX_A 85
#define THS_PREPARE_MAX_B 6
#define THS_EXIT_MIN      100

/* Lane TX_DCTRL values (PHY 2.7.0 - different from sc7180 10nm) */
#define LANE_TX_DCTRL_DLN012  0x40
#define LANE_TX_DCTRL_DLN3    0x46
#define LANE_TX_DCTRL_CLK     0x41

/* PLL divider LUT for D-PHY (from UEFI asPllDividerLUT_DPhy) */
struct pll_div_lut_entry {
	u16 pll_post_div;
	u16 phy_post_div;
};

static const struct pll_div_lut_entry pll_div_lut[] = {
	{ 2, 5 },  /* 10 */
	{ 1, 9 },  /* 9  */
	{ 8, 1 },  /* 8  */
	{ 1, 7 },  /* 7  */
	{ 2, 3 },  /* 6  */
	{ 1, 5 },  /* 5  */
	{ 4, 1 },  /* 4  */
	{ 1, 3 },  /* 3  */
	{ 2, 1 },  /* 2  */
	{ 1, 1 },  /* 1  */
};

/* Helper: integer rounding division (round half up) */
static s32 dsi_div_round(s32 num, s32 denom)
{
	if (denom == 0)
		return 0;
	return (num + denom / 2) / denom;
}

/*
 * Step 1: Wait for refgen ready
 * UEFI: HAL_DSI_Phy_WaitForRefgen
 */
static enum cb_err dsi_phy_wait_refgen(void)
{
	u32 timeout = DSI_PHY_REFGEN_TIMEOUT_US / 500;

	/* Request refgen */
	write32(&active_phy->phy_cmn_glbl_digtop_spare10, 0x1);

	while (timeout--) {
		udelay(500);
		if (read32(&active_phy->phy_cmn_phy_status) & 0x1)
			return CB_SUCCESS;
	}
	printk(BIOS_ERR, "DSI PHY: refgen timeout\n");
	return CB_ERR;
}

/*
 * Step 2: Power-up PHY (D-PHY, 4-lane, non-split-link)
 * UEFI: HAL_DSI_Phy_PowerUp
 */
static void dsi_phy_power_up(u32 desired_bitclk_hz)
{
	u32 vreg_ctrl_0, rescode_top, rescode_bot;

	/* Deassert SLEEP_B */
	write32(&active_phy->phy_cmn_ctrl3, 0x00);

	/* Power up DIGTOP + PLL (two-step) */
	write32(&active_phy->phy_cmn_ctrl0, 0x20);
	write32(&active_phy->phy_cmn_ctrl0, 0x60);

	/* Disable PLL */
	write32(&active_phy->phy_cmn_pll_ctrl, 0x00);

	/* Resync FIFO off */
	write32(&active_phy->phy_cmn_rbuf_ctrl, 0x00);

	/* CTRL_4: DCTRL_GLBL_2 = 0x4 */
	write32(&active_phy->phy_cmn_ctrl4, 0x04);

	/*
	 * Frequency-dependent VREG and rescode values.
	 * HLOS golden: TOP=0x3C@0xF4, BOT=0x38@0xF8 (mid_ctrl), MID=0x0@0xFC
	 * Our struct: top_ctrl@0xF4, mid_ctrl@0xF8, bot_ctrl@0xFC
	 */
	if (desired_bitclk_hz <= 1500000000U) {
		vreg_ctrl_0  = 0x44;
		rescode_top  = 0x3C;   /* HLOS golden: 0x3C */
		rescode_bot  = 0x38;   /* HLOS golden: 0x38 at 0xF8 = mid_ctrl */
	} else {
		vreg_ctrl_0  = 0x44;
		rescode_top  = 0x01;
		rescode_bot  = 0x39;
	}

	/* Global common init (D-PHY) */
	write32(&active_phy->phy_cmn_vreg_ctrl_0,                 vreg_ctrl_0);
	write32(&active_phy->phy_cmn_glbl_rescode_offset_top_ctrl, rescode_top);
	write32(&active_phy->phy_cmn_glbl_rescode_offset_mid_ctrl, rescode_bot);
	write32(&active_phy->phy_cmn_glbl_rescode_offset_bot_ctrl, 0x00);
	write32(&active_phy->phy_cmn_vreg_ctrl_1,                 0x19);
	write32(&active_phy->phy_cmn_ctrl3,                       0x00);
	write32(&active_phy->phy_cmn_glbl_str_swi_cal_sel_ctrl,   0x00);
	write32(&active_phy->phy_cmn_glbl_hstx_str_ctrl_0,        0x88);
	write32(&active_phy->phy_cmn_glbl_pemph_ctrl_0,           0x00);
	write32(&active_phy->phy_cmn_glbl_lptx_str_ctrl,          0x55);

	/* Remove PLL, DIG and all lanes from power-down */
	write32(&active_phy->phy_cmn_ctrl0, 0x7F);

	/* Lane enable: DLN0-3 + CLK = 0x1F */
	write32(&active_phy->phy_cmn_dsi_lane_ctrl0, 0x1F);

	/* DCTRL and CLK_CFG0 */
	write32(&active_phy->phy_cmn_ctrl2,    0x40);
	write32(&active_phy->phy_cmn_clk_cfg0, 0x01);
	write32(&active_phy->phy_cmn_clk_cfg0, 0x01);
}

/*
 * Step 3: Lane init
 * UEFI: HAL_DSI_Phy_SetupLanePhy (D-PHY path)
 * Lane offsets within PHY: DLN0=+0x200, DLN1=+0x280, DLN2=+0x300,
 *                          DLN3=+0x380, CLK=+0x400
 */
static void dsi_phy_setup_lane(int lane_idx)
{
	/* Use the phy_ln_regs array directly - no offsetof/typeof needed */
	int is_clk  = (lane_idx == 4);
	int is_dln0 = (lane_idx == 0);
	int is_dln3 = (lane_idx == 3);
	u32 cfg2_val, tx_dctrl;

	/* LPRX_CTRL: 0->0->lprx_val (transition from 0 to value) */
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_lprx_ctrl, 0x00);
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_lprx_ctrl, 0x00);
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_lprx_ctrl,
		is_dln0 ? 0x03 : 0x00);

	/* Pin swap */
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_pin_swap, 0x00);
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_pin_swap, 0x00);

	/* CFG0: PGM delay */
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_cfg0, 0x00);
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_cfg0, 0x00);

	/* CFG1 */
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_cfg1, 0x00);

	/*
	 * CFG2: IS_CKLANE=bit7 | LPRX_DLY=0x0A
	 * DLN0 gets 0 first then the value; others get value directly
	 */
	cfg2_val = (is_clk ? BIT(7) : 0) | 0x0A;
	if (is_dln0)
		write32(&active_phy->phy_ln_regs[lane_idx].dln0_cfg2, 0x00);
	write32(&active_phy->phy_ln_regs[lane_idx].dln0_cfg2, cfg2_val);

	/* TX_DCTRL: different per lane type */
	if (is_clk)
		tx_dctrl = LANE_TX_DCTRL_CLK;
	else if (is_dln3)
		tx_dctrl = LANE_TX_DCTRL_DLN3;
	else
		tx_dctrl = LANE_TX_DCTRL_DLN012;

	write32(&active_phy->phy_ln_regs[lane_idx].dln0_tx_dctrl, tx_dctrl);
}

/* Toggle DLN3 TX_DCTRL (UEFI: HAL_DSI_Phy_Toogle_Dln3_Tx_Dctrl) */
static void dsi_phy_toggle_dln3_tx_dctrl(void)
{
	u32 val = read32(&active_phy->phy_ln_regs[3].dln0_tx_dctrl) & ~0x01;

	write32(&active_phy->phy_ln_regs[3].dln0_tx_dctrl, 0x01 | val);
	write32(&active_phy->phy_ln_regs[3].dln0_tx_dctrl, val);
}

/*
 * Step 4a: PLL divider calculation
 * UEFI: HAL_DSI_Phy_CalcClockDivider + HAL_DSI_Phy_DsiClk_Divider
 * Returns VCO frequency in Hz, fills pll_post_div and phy_post_div.
 */
static unsigned long dsi_phy_calc_vco(u32 desired_bitclk_hz,
				      u32 bpp, u32 num_lanes,
				      u32 *pll_post_div_out,
				      u32 *phy_post_div_out,
				      u32 *div_ctrl_out)
{
	u64 min_vco_hz = 600000000ULL; /* 600 MHz minimum */
	int i;

	for (i = ARRAY_SIZE(pll_div_lut) - 1; i >= 0; i--) {
		u64 total_div = (u64)pll_div_lut[i].pll_post_div *
				pll_div_lut[i].phy_post_div;
		u64 vco = (u64)desired_bitclk_hz * total_div;

		if (vco >= min_vco_hz) {
			u32 pll_post = pll_div_lut[i].pll_post_div;
			u32 phy_post = pll_div_lut[i].phy_post_div;
			/* div_ctrl = (bpp * pll_post_div) / (num_lanes * 2) */
			u32 div_ctrl = (bpp * pll_post) / (num_lanes * 2);

			*pll_post_div_out = pll_post;
			*phy_post_div_out = phy_post;
			/* CLK_CFG0: DIV_CTRL_7_4 | DIV_CTRL_3_0 (phy_post_div) */
			*div_ctrl_out = ((div_ctrl << 4) & 0xF0) | (phy_post & 0x0F);
			return (unsigned long)vco;
		}
	}
	return 0;
}

/*
 * Step 4b: PLL lock detect
 * UEFI: HAL_DSI_Phy_PllLockDetect
 */
static enum cb_err dsi_phy_pll_lock_detect(void)
{
	u32 timeout = DSI_PHY_PLL_LOCK_TIMEOUT_US / 100;

	/* Enable PLL */
	write32(&active_phy->phy_cmn_pll_ctrl, 0x01);

	while (timeout--) {
		udelay(100);
		if (read32(&active_pll->pll_common_status_one) & 0x1)
			return CB_SUCCESS;
	}
	printk(BIOS_ERR, "DSI PHY: PLL lock timeout\n");
	return CB_ERR;
}

/*
 * Step 5: TriggerSleepB + GlobalClockEnable + ResyncFIFO
 * UEFI: HAL_DSI_Phy_TriggerSleepB, PllGlobalClockEnable, ResyncFIFO
 */
static void dsi_phy_post_pll_lock(void)
{
	u32 ctrl3, clk_cfg1;

	/*
	 * TriggerSleepB: set SLEEP_B bit in CTRL_3.
	 * SLEEP_B = bit2 (HWIO: CMN_CTRL_3_SLEEP_B_BMSK=0x4, SHFT=2)
	 * HLOS golden: CTRL_3=0x04 confirms SLEEP_B=1 after post-lock.
	 */
	ctrl3 = read32(&active_phy->phy_cmn_ctrl3);
	ctrl3 |= BIT(2);
	write32(&active_phy->phy_cmn_ctrl3, ctrl3);
	/* CTRL_4: DCTRL_GLBL_2 = 0x04 */
	write32(&active_phy->phy_cmn_ctrl4, 0x04);

	/*
	 * GlobalClockEnable: set CLK_EN(bit5)=1 only.
	 * HLOS golden (hwiosave dump): CLK_CFG1=0x21 -> CLK_EN=1, CLK_EN_SEL=0, DSICLK_SEL=1
	 * CLK_EN_SEL=0 means clock enable is controlled by CLK_EN bit directly.
	 */
	clk_cfg1 = read32(&active_phy->phy_cmn_clk_cfg1);
	clk_cfg1 = (clk_cfg1 & ~BIT(4)) | BIT(5);
	write32(&active_phy->phy_cmn_clk_cfg1, clk_cfg1);

	/* ResyncFIFO: pulse RBUF_CTRL */
	write32(&active_phy->phy_cmn_rbuf_ctrl, 0x00);
	write32(&active_phy->phy_cmn_rbuf_ctrl, 0x01);
}

/*
 * DSI clock source configuration.
 * Switches pclk0/byte0 from XO (19.2MHz) to DSI PLL output after lock.
 * Source values from sDSI0ExtClocks_9xx[] in UEFI MDSSPlatform_9xx.h
 * (confirmed from purwa_boot_620 HALclkHWIOPurwa.h).
 * Called from dsi_phy_enable_clocks() after PLL lock.
 */
static void mdss_dsi_clock_configure(u32 pclk_hz, u32 byte_hz)
{
	struct clock_freq_config pclk0_cfg[] = {
		{
			.hz  = pclk_hz,
			.src = 1,  /* dsi0_phy_pll_out_dsiclk */
			.div = 0,  /* SRC_DIV=0 = bypass */
		},
	};
	struct clock_freq_config byte0_cfg[] = {
		{
			.hz  = byte_hz,
			.src = 2,  /* dsi0_phy_pll_out_byteclk */
			.div = QCOM_CLOCK_DIV(1),
		},
	};

	clock_configure(&disp_cc->mdss_pclk0_rcg, pclk0_cfg, pclk_hz,
			ARRAY_SIZE(pclk0_cfg));
	clock_configure(&disp_cc->mdss_byte0_rcg, byte0_cfg, byte_hz,
			ARRAY_SIZE(byte0_cfg));

	/* byte0_intf shares the same RCG as byte0 - only CDIVR needed. */
	write32(&disp_cc->mdss_byte0_div, 1);
}

/*
 * Configure DSI1 clocks from DSI1 PLL output.
 * Mirror of mdss_dsi_clock_configure() for DSI1 RCGs.
 * Source values: pclk1 SRC_SEL=1 (dsi1_phy_pll_out_dsiclk),
 *                byte1 SRC_SEL=2 (dsi1_phy_pll_out_byteclk)
 */
static void mdss_dsi1_clock_configure(u32 pclk_hz, u32 byte_hz)
{
	struct clock_freq_config pclk1_cfg[] = {
		{
			.hz  = pclk_hz,
			.src = 1,  /* dsi1_phy_pll_out_dsiclk */
			.div = 0,
		},
	};
	struct clock_freq_config byte1_cfg[] = {
		{
			.hz  = byte_hz,
			.src = 2,  /* dsi1_phy_pll_out_byteclk */
			.div = QCOM_CLOCK_DIV(1),
		},
	};

	clock_configure(&disp_cc->mdss_pclk1_rcg, pclk1_cfg, pclk_hz,
			ARRAY_SIZE(pclk1_cfg));
	clock_configure(&disp_cc->mdss_byte1_rcg, byte1_cfg, byte_hz,
			ARRAY_SIZE(byte1_cfg));
	write32(&disp_cc->mdss_byte1_div, 1);
}

/*
 * Step 6: Enable DSI clocks
 * UEFI: MDSSPlatform_9xx DSI0Clocks_9xx + sDSI0ExtClocks_9xx
 */
static enum cb_err dsi_phy_enable_clocks(u32 pclk_hz, u32 byte_hz)
{
	/*
	 * Switch pclk0/byte0 RCGs from XO to DSI PLL output.
	 * Source values from sDSI0ExtClocks_9xx[] in UEFI MDSSPlatform_9xx.h:
	 *   pclk0: SRC_SEL=1 (dsi0_phy_pll_out_dsiclk)
	 *   byte0: SRC_SEL=2 (dsi0_phy_pll_out_byteclk)
	 *   esc0:  stays on XO (SRC_SEL=0)
	 */
	mdss_dsi_clock_configure(pclk_hz, byte_hz);

	if (mdss_clock_enable(DISP_CC_MDSS_ESC0_CBCR) != CB_SUCCESS)
		printk(BIOS_ERR, "DSI: ESC0 clock enable failed\n");

	if (mdss_clock_enable(DISP_CC_MDSS_BYTE0_CBCR) != CB_SUCCESS)
		printk(BIOS_ERR, "DSI: BYTE0 clock enable failed\n");

	if (mdss_clock_enable(DISP_CC_MDSS_BYTE0_INTF_CBCR) != CB_SUCCESS)
		printk(BIOS_ERR, "DSI: BYTE0_INTF clock enable failed\n");

	if (mdss_clock_enable(DISP_CC_MDSS_PCLK0_CBCR) != CB_SUCCESS)
		printk(BIOS_ERR, "DSI: PCLK0 clock enable failed\n");

	printk(BIOS_INFO, "DSI: byte0_cbcr=0x%08x pclk0_cbcr=0x%08x\n",
		read32(&disp_cc->mdss_byte0_cbcr),
		read32(&disp_cc->mdss_pclk0_cbcr));

	return CB_SUCCESS;
}

/*
 * Step 7: D-PHY timing parameter calculation and commit
 * UEFI: HAL_DSI_Phy_CalculateTimingParams + CommitTimingParams
 * All math uses integer arithmetic with coeff=1000 for precision.
 */
static void dsi_phy_calc_and_commit_timings(u32 desired_bitclk_hz)
{
	struct msm_dsi_phy_ctrl t = {0};
	u32 bit_rate = desired_bitclk_hz;
	s32 coeff = 1000;
	s32 ui, ui_x8;
	s32 tmin, tmax, temp;

	if (!bit_rate)
		return;

	t.half_byte_clk_en = 0;

	/* ui = 1e12 / bit_rate (in ps, scaled by coeff) */
	ui    = (s32)((u64)TIPX_NUMER * coeff / bit_rate);
	ui_x8 = ui * 8;

	/* T_CLK_PREPARE: [38ns..95ns] / ui_x8 */
	tmin = (TCLK_PREPARE_MIN * coeff + ui_x8 - 1) / ui_x8;
	tmax = (TCLK_PREPARE_MAX * coeff) / ui_x8;
	t.clk_prepare = dsi_div_round(tmin + tmax, 2);

	/* T_CLK_ZERO: 300ns - clk_prepare*ui_x8 */
	temp = 300 * coeff - (s32)t.clk_prepare * ui_x8;
	tmin = (temp + ui_x8 - 1) / ui_x8 - 1;
	tmax = (tmin > 255) ? 511 : 255;
	t.clk_zero = dsi_div_round(tmin + tmax, 2);

	/* T_CLK_TRAIL: >= 60ns + 3*ui */
	tmin = (60 * coeff + 3 * ui + ui_x8 - 1) / ui_x8;
	tmax = (105 * coeff + 12 * ui - 20 * coeff + 3 * ui) / ui_x8;
	t.clk_trail = dsi_div_round(tmin + tmax, 2);

	/* T_HS_PREPARE: [40ns+4ui .. 85ns+6ui] / ui_x8 */
	tmin = (THS_PREPARE_MIN_A * coeff + THS_PREPARE_MIN_B * ui + ui_x8 - 1) / ui_x8;
	tmax = (THS_PREPARE_MAX_A * coeff + THS_PREPARE_MAX_B * ui) / ui_x8;
	t.hs_prepare = dsi_div_round(tmin + tmax, 2);

	/* T_HS_ZERO: 145ns + 10ui - hs_prepare*ui_x8 */
	temp = 145 * coeff + 10 * ui - (s32)t.hs_prepare * ui_x8;
	tmin = (temp + ui_x8 - 1) / ui_x8 - 1;
	tmax = 255;
	t.hs_zero = dsi_div_round(tmin + tmax, 2);

	/* T_HS_TRAIL: >= 60ns + 4ui */
	tmin = (60 * coeff + 4 * ui + ui_x8 - 1) / ui_x8 - 1;
	tmax = (105 * coeff + 12 * ui - 20 * coeff) / ui_x8 - 1;
	t.hs_trail = dsi_div_round(tmin + tmax, 2);

	/* T_HS_RQST: 50ns + (4-8)*ui */
	t.hs_rqst = (50 * coeff - 8 * ui + ui_x8 - 1) / ui_x8;
	if (t.hs_rqst > 0xFF)
		t.hs_rqst = 0xFF;

	/* T_HS_EXIT: >= 100ns */
	tmin = (THS_EXIT_MIN * coeff + ui_x8 - 1) / ui_x8 - 1;
	tmax = 255;
	t.hs_exit = dsi_div_round(tmin + tmax, 2);

	/* T_TA_GO, T_TA_SURE, T_TA_GET (fixed values) */
	t.ta_go   = 3;
	t.ta_sure = 0;
	t.ta_get  = 4;

	/* T_CLK_POST: 60ns + 52ui - 43ui */
	tmin = (60 * coeff + 52 * ui - 43 * ui + ui_x8 - 1) / ui_x8 - 1;
	tmax = 63;
	t.clk_post = dsi_div_round(tmin + tmax, 2);

	/* T_CLK_PRE */
	temp = 8 * ui + (s32)t.clk_prepare * ui_x8;
	temp += ((s32)(t.clk_zero + 3) * 8 + 11) * ui;
	temp += ((s32)t.hs_rqst * 8 + 8) * ui;
	tmin = (temp + ui_x8 - 1) / ui_x8 - 1;
	tmax = 63;
	if (tmin > tmax) {
		t.clk_pre = tmin / 2;
		t.clk_pre_inc_by_2 = true;
	} else {
		t.clk_pre = dsi_div_round(tmin + tmax, 2);
		t.clk_pre_inc_by_2 = false;
	}

	printk(BIOS_DEBUG,
	       "DSI PHY timings: clk_pre=%u clk_post=%u clk_zero=%u clk_trail=%u "
	       "clk_prepare=%u hs_exit=%u hs_zero=%u hs_prepare=%u hs_trail=%u hs_rqst=%u\n",
	       t.clk_pre, t.clk_post, t.clk_zero, t.clk_trail, t.clk_prepare,
	       t.hs_exit, t.hs_zero, t.hs_prepare, t.hs_trail, t.hs_rqst);

	/*
	 * Commit timing registers (TIMING_CTRL_0..13).
	 * Use panel-provided PHY timings if available (from DTSI
	 * qcom,mdss-dsi-panel-phy-timings), otherwise use calculated values.
	 */
	const u8 *panel_timings = mdss_get_panel_ops()->phy_timings;
	if (panel_timings) {
		/* Use DTSI-provided timings - validated against HLOS golden */
		for (int i = 0; i < DSI_PHY_TIMING_SIZE; i++)
			write32(&active_phy->phy_cmn_timing_ctrl[i], panel_timings[i]);
	} else {
		/* Use calculated timings */
		u8 calc_timings[14] = {
			0x00,          /* CTRL_0:  half_byte_clk_en */
			t.clk_zero,    /* CTRL_1 */
			t.clk_prepare, /* CTRL_2 */
			t.clk_trail,   /* CTRL_3 */
			t.hs_exit,     /* CTRL_4 */
			t.hs_zero,     /* CTRL_5 */
			t.hs_prepare,  /* CTRL_6 */
			t.hs_trail,    /* CTRL_7 */
			t.hs_rqst,     /* CTRL_8 */
			t.ta_go,       /* CTRL_9 */
			t.ta_get,      /* CTRL_10 */
			0x00,          /* CTRL_11: trig3_cmd */
			t.clk_pre,     /* CTRL_12 */
			t.clk_post,    /* CTRL_13 */
		};
		for (int i = 0; i < 14; i++)
			write32(&active_phy->phy_cmn_timing_ctrl[i], calc_timings[i]);
	}
}

/* Top-level entry point */
static enum cb_err dsi_phy_init_instance(struct edid *edid, u32 num_of_lanes, u32 bpp)
{
	u32 pixel_clk_hz = edid->mode.pixel_clock * KHz;
	u32 desired_bitclk_hz;
	u32 pll_post_div, phy_post_div, div_ctrl;
	unsigned long vco_hz;
	enum cb_err ret;

	/* Bit clock = pixel_clock * bpp / num_lanes */
	desired_bitclk_hz = (u32)((u64)pixel_clk_hz * bpp / num_of_lanes);

	printk(BIOS_ERR, "DSI PHY: pixel_clk=%u Hz, bit_clk=%u Hz\n",
	       pixel_clk_hz, desired_bitclk_hz);

	/* Step 1: Wait for refgen */
	ret = dsi_phy_wait_refgen();
	if (ret != CB_SUCCESS)
		return ret;

	/* Step 2: Power-up */
	dsi_phy_power_up(desired_bitclk_hz);

	/* Step 3: Lane init (DLN0-3 + CLK) */
	for (int i = 0; i < 5; i++)
		dsi_phy_setup_lane(i);

	/* Step 4a: Calculate PLL dividers */
	vco_hz = dsi_phy_calc_vco(desired_bitclk_hz, bpp, num_of_lanes,
				  &pll_post_div, &phy_post_div, &div_ctrl);
	if (!vco_hz) {
		printk(BIOS_ERR, "DSI PHY: bit clock too low, unsupported\n");
		return CB_ERR;
	}

	printk(BIOS_ERR, "DSI PHY: VCO=%lu Hz, pll_post=%u, phy_post=%u\n",
	       vco_hz, pll_post_div, phy_post_div);

	/*
	 * Write CLK_CFG0: use panel override if provided, else use calculated div_ctrl.
	 * Panel driver sets clk_cfg0_override when HLOS uses a different divider
	 * than what coreboot calculates (e.g. NT36536: HLOS=0x51, calc=0x31).
	 */
	{
		u8 cfg0 = mdss_get_panel_ops()->clk_cfg0_override ?
			mdss_get_panel_ops()->clk_cfg0_override : (u8)div_ctrl;
		write32(&active_phy->phy_cmn_clk_cfg0, cfg0);
	}

	/*
	 * Set byte clock source in CLK_CFG1:
	 * DSICLK_SEL=1 for DSI0 (use DSI0 PLL output)
	 * DSICLK_SEL=5 for DSI1 (use DSI1 PLL output - BIT(2) selects DSI1 PLL)
	 * Confirmed from HLOS golden: DSI0 CLK_CFG1=0x21, DSI1 CLK_CFG1=0x25
	 */
	u32 dsiclk_sel = (active_phy == dsi1_phy) ? 5 : 1;
	write32(&active_phy->phy_cmn_clk_cfg1,
		(read32(&active_phy->phy_cmn_clk_cfg1) & ~0x7) | dsiclk_sel);

	/* PLL bias enable + PERF_OPTIMIZE */
	write32(&active_pll->pll_system_muxes, BIT(6) | BIT(7));
	write32(&active_pll->pll_perf_optimize, 0x22);

	/* Step 4b: Program PLL VCO */
	dsi_phy_pll_vco_270_set_rate(vco_hz, pll_post_div);

	/* Toggle DLN3 TX_DCTRL (required before PLL lock) */
	dsi_phy_toggle_dln3_tx_dctrl();

	/* Step 4c: PLL lock detect */
	ret = dsi_phy_pll_lock_detect();
	if (ret != CB_SUCCESS)
		return ret;

	/* Step 5: Post-lock sequence */
	dsi_phy_post_pll_lock();

	/* Step 6: Enable DSI clocks */
	ret = dsi_phy_enable_clocks(pixel_clk_hz, desired_bitclk_hz / 8);
	if (ret != CB_SUCCESS)
		return ret;

	/* Step 7: Timing parameters */
	dsi_phy_calc_and_commit_timings(desired_bitclk_hz);

	return CB_SUCCESS;
}

enum cb_err mdss_dsi_phy_270_init(struct edid *edid, u32 num_of_lanes, u32 bpp)
{
	active_phy = dsi0_phy;
	active_pll = phy_pll_qlink;
	return dsi_phy_init_instance(edid, num_of_lanes, bpp);
}

enum cb_err mdss_dsi1_phy_270_init(struct edid *edid, u32 num_of_lanes, u32 bpp)
{
	/*
	 * For DSI1: use DSI1 PHY/PLL but configure DSI1 clocks instead of DSI0.
	 * Override the clock configure function pointer via active_phy/pll,
	 * then call the shared init which calls dsi_phy_enable_clocks().
	 * dsi_phy_enable_clocks() calls mdss_dsi_clock_configure() - for DSI1
	 * we need mdss_dsi1_clock_configure() instead.
	 * Solution: call init_instance then reconfigure DSI1 clocks.
	 */
	active_phy = dsi1_phy;
	active_pll = phy_pll_qlink1;
	enum cb_err ret = dsi_phy_init_instance(edid, num_of_lanes, bpp);
	if (ret != CB_SUCCESS)
		return ret;

	/* Reconfigure DSI1 RCGs - dsi_phy_init_instance configured DSI0 RCGs */
	u32 pixel_clk_hz = edid->mode.pixel_clock * KHz;
	u32 bpp_val = 24;  /* RGB888 */
	u32 desired_bitclk_hz = (u32)((u64)pixel_clk_hz * bpp_val / 4);
	mdss_dsi1_clock_configure(pixel_clk_hz, desired_bitclk_hz / 8);

	/* Enable DSI1 CBCRs */
	if (mdss_clock_enable(DISP_CC_MDSS_ESC1_CBCR) != CB_SUCCESS)
		printk(BIOS_ERR, "DSI1: ESC1 clock enable failed\n");
	if (mdss_clock_enable(DISP_CC_MDSS_PCLK1_CBCR) != CB_SUCCESS)
		printk(BIOS_ERR, "DSI1: PCLK1 clock enable failed\n");
	if (mdss_clock_enable(DISP_CC_MDSS_BYTE1_CBCR) != CB_SUCCESS)
		printk(BIOS_ERR, "DSI1: BYTE1 clock enable failed\n");
	if (mdss_clock_enable(DISP_CC_MDSS_BYTE1_INTF_CBCR) != CB_SUCCESS)
		printk(BIOS_ERR, "DSI1: BYTE1_INTF clock enable failed\n");

	return CB_SUCCESS;
}
