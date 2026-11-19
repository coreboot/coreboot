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

/* DISP_CC SRC_SEL values from Purwa UEFI MDSSPlatform_9xx.h. */
enum dsi_clock_source {
	DSI0_PLL_PIXEL_CLOCK = 1,
	DSI0_PLL_BYTE_CLOCK = 2,
	DSI1_PLL_PIXEL_CLOCK = 3,
	DSI1_PLL_BYTE_CLOCK = 4,
};

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

static const struct pll_div_lut_entry cphy_pll_div_lut[] = {
	{ 2, 5 }, { 8, 1 }, { 2, 3 }, { 1, 5 },
	{ 4, 1 }, { 1, 3 }, { 2, 1 }, { 1, 1 },
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
static void dsi_phy_power_up(u32 desired_bitclk_hz, u32 num_of_lanes, bool cphy_mode)
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

	if (cphy_mode) {
		/* UEFI HAL_DSI_Phy_PowerUp() C-PHY initialization sequence. */
		write32(&active_phy->phy_cmn_vreg_ctrl_0, 0x45);
		write32(&active_phy->phy_cmn_vreg_ctrl_1, 0x41);
		write32(&active_phy->phy_cmn_ctrl3, 0x00);
		write32(&active_phy->phy_cmn_glbl_str_swi_cal_sel_ctrl, 0x00);
		write32(&active_phy->phy_cmn_glbl_hstx_str_ctrl_0, 0x88);
		write32(&active_phy->phy_cmn_ctrl2, 0x40);
		write32(&active_phy->phy_cmn_glbl_pemph_ctrl_0, 0x11);
		write32(&active_phy->phy_cmn_glbl_pemph_ctrl_1, 0x01);
		write32(&active_phy->phy_cmn_glbl_rescode_offset_top_ctrl, 0x00);
		write32(&active_phy->phy_cmn_glbl_rescode_offset_mid_ctrl, 0x00);
		write32(&active_phy->phy_cmn_glbl_rescode_offset_bot_ctrl, 0x00);
		write32(&active_phy->phy_cmn_glbl_lptx_str_ctrl, 0x55);
		write32(&active_phy->phy_cmn_ctrl0, 0x7f);
		write32(&active_phy->phy_cmn_dsi_lane_ctrl0,
			(num_of_lanes >= 1 ? BIT(0) : 0) |
			(num_of_lanes >= 2 ? BIT(1) : 0) |
			(num_of_lanes >= 3 ? BIT(2) : 0) |
			(num_of_lanes >= 4 ? BIT(3) : 0) | BIT(4));
		return;
	}

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
static void dsi_phy_setup_lane(int lane_idx, bool cphy_mode)
{
	/* Use the phy_ln_regs array directly - no offsetof/typeof needed */
	int is_clk  = (lane_idx == 4);
	int is_dln0 = (lane_idx == 0);
	int is_dln3 = (lane_idx == 3);
	u32 cfg2_val, tx_dctrl;
	if (cphy_mode) {
		if (lane_idx == 3) {
			write32(&active_phy->phy_ln_regs[lane_idx].dln0_tx_dctrl, 0x06);
			return;
		}

		write32(&active_phy->phy_ln_regs[lane_idx].dln0_lprx_ctrl, 0x00);
		write32(&active_phy->phy_ln_regs[lane_idx].dln0_lprx_ctrl,
			lane_idx == 0 ? 0x03 : 0x00);
		write32(&active_phy->phy_ln_regs[lane_idx].dln0_pin_swap, 0x00);
		write32(&active_phy->phy_ln_regs[lane_idx].dln0_cfg0, 0x00);
		write32(&active_phy->phy_ln_regs[lane_idx].dln0_cfg1, 0x00);
		write32(&active_phy->phy_ln_regs[lane_idx].dln0_cfg2, 0x0a);
		write32(&active_phy->phy_ln_regs[lane_idx].dln0_tx_dctrl, 0x40);
		return;
	}

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
				      bool cphy_mode,
				      u32 *pll_post_div_out,
				      u32 *phy_post_div_out,
				      u32 *div_ctrl_out)
{
	u64 min_vco_hz = 600000000ULL; /* 600 MHz minimum */
	const struct pll_div_lut_entry *lut = cphy_mode ? cphy_pll_div_lut : pll_div_lut;
	int lut_len = cphy_mode ? ARRAY_SIZE(cphy_pll_div_lut) : ARRAY_SIZE(pll_div_lut);
	int i;

	for (i = lut_len - 1; i >= 0; i--) {
		u64 total_div = (u64)lut[i].pll_post_div * lut[i].phy_post_div;
		u64 vco = (u64)desired_bitclk_hz * total_div;

		if (vco >= min_vco_hz) {
			u32 pll_post = lut[i].pll_post_div;
			u32 phy_post = lut[i].phy_post_div;
			/* UEFI C-PHY RGB888 / three-trio divider simplifies to phy_post. */
			u32 div_ctrl = cphy_mode ? phy_post :
				(bpp * pll_post) / (num_lanes * 2);

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
	const struct mdss_panel_ops *panel = mdss_get_panel_ops();
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
	 * Preserve the legacy CLK_EN=1, CLK_EN_SEL=0 default. A panel override
	 * supplies the complete post-lock CLK_CFG1, including clock enable bits.
	 */
	clk_cfg1 = read32(&active_phy->phy_cmn_clk_cfg1);
	clk_cfg1 = panel->clk_cfg1_override ? panel->clk_cfg1_override :
		(clk_cfg1 & ~BIT(4)) | BIT(5);
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
			.src = DSI0_PLL_PIXEL_CLOCK,
			.div = 0,  /* SRC_DIV=0 = bypass */
		},
	};
	struct clock_freq_config byte0_cfg[] = {
		{
			.hz  = byte_hz,
			.src = DSI0_PLL_BYTE_CLOCK,
			.div = QCOM_CLOCK_DIV(1),
		},
	};

	clock_configure(&disp_cc->mdss_pclk0_rcg, pclk0_cfg, pclk_hz,
			ARRAY_SIZE(pclk0_cfg));
	clock_configure(&disp_cc->mdss_byte0_rcg, byte0_cfg, byte_hz,
			ARRAY_SIZE(byte0_cfg));

	/* byte0_intf shares the same RCG as byte0 - only CDIVR needed. */
	write32(&disp_cc->mdss_byte0_div, mdss_get_panel_ops()->config->cphy_mode ? 0 : 1);
}

/*
 * Single DSI1 uses PLL1: sDSI1ExtClocks_9xx in Purwa UEFI.
 * Dual DSI shares PLL0: sDSI1SharedSourceExtClocks_9xx.
 */
static void mdss_dsi1_clock_configure(u32 pclk_hz, u32 byte_hz)
{
	bool shared_pll = mdss_get_panel_ops()->config->dsi_count > 1;
	struct clock_freq_config pclk1_cfg[] = {
		{
			.hz  = pclk_hz,
			.src = shared_pll ? DSI0_PLL_PIXEL_CLOCK : DSI1_PLL_PIXEL_CLOCK,
			.div = 0,
		},
	};
	struct clock_freq_config byte1_cfg[] = {
		{
			.hz  = byte_hz,
			.src = shared_pll ? DSI0_PLL_BYTE_CLOCK : DSI1_PLL_BYTE_CLOCK,
			.div = QCOM_CLOCK_DIV(1),
		},
	};

	clock_configure(&disp_cc->mdss_pclk1_rcg, pclk1_cfg, pclk_hz,
			ARRAY_SIZE(pclk1_cfg));
	clock_configure(&disp_cc->mdss_byte1_rcg, byte1_cfg, byte_hz,
			ARRAY_SIZE(byte1_cfg));
	write32(&disp_cc->mdss_byte1_div, mdss_get_panel_ops()->config->cphy_mode ? 0 : 1);
}

/*
 * Step 6: Enable DSI clocks
 * UEFI: MDSSPlatform_9xx DSI0Clocks_9xx + sDSI0ExtClocks_9xx
 */
static enum cb_err dsi_phy_enable_clocks(u32 pclk_hz, u32 byte_hz)
{
	if (active_phy == dsi1_phy) {
		mdss_dsi1_clock_configure(pclk_hz, byte_hz);
		if (mdss_clock_enable(DISP_CC_MDSS_ESC1_CBCR) != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI1: ESC1 clock enable failed\n");
			return CB_ERR;
		}
		if (mdss_clock_enable(DISP_CC_MDSS_BYTE1_CBCR) != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI1: BYTE1 clock enable failed\n");
			return CB_ERR;
		}
		if (mdss_clock_enable(DISP_CC_MDSS_BYTE1_INTF_CBCR) != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI1: BYTE1_INTF clock enable failed\n");
			return CB_ERR;
		}
		if (mdss_clock_enable(DISP_CC_MDSS_PCLK1_CBCR) != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI1: PCLK1 clock enable failed\n");
			return CB_ERR;
		}
	} else {
		mdss_dsi_clock_configure(pclk_hz, byte_hz);
		if (mdss_clock_enable(DISP_CC_MDSS_ESC0_CBCR) != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI: ESC0 clock enable failed\n");
			return CB_ERR;
		}
		if (mdss_clock_enable(DISP_CC_MDSS_BYTE0_CBCR) != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI: BYTE0 clock enable failed\n");
			return CB_ERR;
		}
		if (mdss_clock_enable(DISP_CC_MDSS_BYTE0_INTF_CBCR) != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI: BYTE0_INTF clock enable failed\n");
			return CB_ERR;
		}
		if (mdss_clock_enable(DISP_CC_MDSS_PCLK0_CBCR) != CB_SUCCESS) {
			printk(BIOS_ERR, "DSI: PCLK0 clock enable failed\n");
			return CB_ERR;
		}
	}

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
	const struct mdss_panel_ops *panel = mdss_get_panel_ops();
	const u8 *panel_timings = panel->phy_timings;
	if (panel_timings) {
		/* Use DTSI-provided timings - validated against HLOS golden */
		u32 count = panel->phy_timings_size ? panel->phy_timings_size :
			DSI_PHY_TIMING_SIZE;
		for (u32 i = 0; i < MIN(count, (u32)DSI_PHY_TIMING_SIZE); i++)
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

static u32 div_round_up_u32(u64 numerator, u32 denominator)
{
	return (u32)((numerator + denominator - 1) / denominator);
}

static u32 cphy_timing_margin(u32 min, u32 max, u32 percent)
{
	if (max <= min)
		return min;
	return min + div_round_up_u32((u64)(max - min) * percent, 100);
}

/* C-PHY timing fields from UEFI HAL_DSI_Phy_CalculateTimingParams_CPhy(). */
static void dsi_phy_calc_and_commit_cphy_timings(u32 bitclk_hz)
{
	const struct mdss_panel_ops *panel = mdss_get_panel_ops();
	u32 ui, half_byte_clk_en;
	u32 hs_exit_min, hs_prebegin_min, hs_prepare_min, hs_prepare_max;
	u32 hs_rqst_min;
	s32 hs_rqst_numerator;
	u32 hs_prebegin_min_ns;

	if (!bitclk_hz)
		return;

	write32(&active_phy->phy_cmn_glbl_ctrl, BIT(6));
	write32(&active_phy->phy_cmn_vreg_ctrl_0, 0x45);
	write32(&active_phy->phy_cmn_vreg_ctrl_1, 0x41);
	write32(&active_phy->phy_cmn_timing_ctrl[11], 0x00);
	if (panel->phy_timings) {
		u32 count = panel->phy_timings_size ? panel->phy_timings_size :
			DSI_PHY_TIMING_SIZE;
		for (u32 i = 0; i < MIN(count, (u32)DSI_PHY_TIMING_SIZE); i++)
			write32(&active_phy->phy_cmn_timing_ctrl[i], panel->phy_timings[i]);
		return;
	}

	/* UI in thousandths of a nanosecond keeps this arithmetic integral. */
	ui = div_round_up_u32(1000000000000ULL, bitclk_hz);
	half_byte_clk_en = bitclk_hz < 80000000U;
	hs_exit_min = div_round_up_u32(100000, ui * 7) - 1;
	hs_prebegin_min_ns = MIN(262000 - 7 * ui, 448 * ui);
	hs_prebegin_min = div_round_up_u32(hs_prebegin_min_ns, ui * 7) - 1;
	hs_prepare_min = div_round_up_u32(38000 - 3 * half_byte_clk_en * ui, ui * 7);
	hs_prepare_max = (95000 - 3 * half_byte_clk_en * ui) / (ui * 7);
	hs_rqst_numerator = 50000 + (4 * (s32)half_byte_clk_en - 7) * (s32)ui;
	hs_rqst_min = div_round_up_u32(hs_rqst_numerator > 0 ? hs_rqst_numerator : 0,
				       ui * 7);

	write32(&active_phy->phy_cmn_timing_ctrl[0], half_byte_clk_en);
	write32(&active_phy->phy_cmn_timing_ctrl[4], cphy_timing_margin(hs_exit_min, 255, 10));
	write32(&active_phy->phy_cmn_timing_ctrl[5], cphy_timing_margin(hs_prebegin_min, 63, 20));
	write32(&active_phy->phy_cmn_timing_ctrl[6],
		cphy_timing_margin(hs_prepare_min, hs_prepare_max, 50));
	write32(&active_phy->phy_cmn_timing_ctrl[7], cphy_timing_margin(0, 31, 80));
	/* UEFI uses a 1% margin at >= 500 MHz, otherwise 0.05%. */
	write32(&active_phy->phy_cmn_timing_ctrl[8], bitclk_hz >= 500000000U ?
		cphy_timing_margin(hs_rqst_min, 255, 1) :
		hs_rqst_min + div_round_up_u32(255 - hs_rqst_min, 2000));
	write32(&active_phy->phy_cmn_timing_ctrl[9], 0x02);
	write32(&active_phy->phy_cmn_timing_ctrl[10], 0x04);
}

/* Top-level entry point */
static enum cb_err dsi_phy_init_instance(struct edid *edid, u32 num_of_lanes, u32 bpp)
{
	u32 pixel_clk_hz = edid->mode.pixel_clock * KHz;
	u32 dsi_pclk_hz;
	u32 desired_bitclk_hz, phy_bitclk_hz, byteclk_hz;
	u32 pll_post_div, phy_post_div, div_ctrl;
	unsigned long vco_hz;
	enum cb_err ret;
	const struct mdss_panel_ops *panel = mdss_get_panel_ops();
	const struct mdss_panel_config *cfg = panel->config;
	bool cphy_mode = cfg->cphy_mode;

	/* UEFI calculates DSC link bandwidth from compressed active width. */
	if ((cfg->command_mode || cphy_mode) && cfg->dsc_ratio_x1000) {
		u32 compressed_width = mdss_dsc_compressed_ha(edid->mode.ha / cfg->dsi_count,
							     cfg->dsc_ratio_x1000);
		pixel_clk_hz = (u32)((u64)(compressed_width + edid->mode.hbl) *
				     (edid->mode.va + edid->mode.vbl) *
				     edid->mode.refresh);
	}

	/* Bit clock = pixel_clock * bpp / num_lanes */
	desired_bitclk_hz = (u32)((u64)pixel_clk_hz * bpp / num_of_lanes);
	phy_bitclk_hz = cphy_mode ? (desired_bitclk_hz / 16) * 7 : desired_bitclk_hz;
	byteclk_hz = cphy_mode ? phy_bitclk_hz / 7 : phy_bitclk_hz / 8;
	dsi_pclk_hz = cphy_mode ? (u32)((u64)byteclk_hz * num_of_lanes * 2 / (bpp / 8)) :
		pixel_clk_hz;

	printk(BIOS_DEBUG, "DSI PHY: pixel_clk=%u Hz, bit_clk=%u Hz, phy_clk=%u Hz, cphy=%u\n",
	       dsi_pclk_hz, desired_bitclk_hz, phy_bitclk_hz, cphy_mode);

	/* Step 1: Wait for refgen */
	ret = dsi_phy_wait_refgen();
	if (ret != CB_SUCCESS)
		return ret;

	/* Step 2: Power-up */
	dsi_phy_power_up(desired_bitclk_hz, num_of_lanes, cphy_mode);

	/* C-PHY has three data trios and no separate clock lane. */
	for (int i = 0; i < (cphy_mode ? 4 : 5); i++)
		dsi_phy_setup_lane(i, cphy_mode);

	/* Step 4a: Calculate PLL dividers */
	vco_hz = dsi_phy_calc_vco(phy_bitclk_hz, bpp, num_of_lanes, cphy_mode,
				  &pll_post_div, &phy_post_div, &div_ctrl);
	if (!vco_hz) {
		printk(BIOS_ERR, "DSI PHY: bit clock too low, unsupported\n");
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "DSI PHY: VCO=%lu Hz, pll_post=%u, phy_post=%u\n",
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
	 * DSICLK_SEL[1:0]=3 for C-PHY RGB888, 1 for D-PHY.
	 * BITCLK_SEL[3:2]=0 uses this PHY's PLL. Only split-source DSI1
	 * uses BITCLK_SEL=1 to receive the other PHY's clock.
	 */
	u32 dsiclk_sel = cphy_mode ? 3 :
		((active_phy == dsi1_phy && cfg->dsi_count > 1) ? 5 : 1);
	write32(&active_phy->phy_cmn_clk_cfg1,
		(read32(&active_phy->phy_cmn_clk_cfg1) & ~0xf) | dsiclk_sel);

	/* PLL bias enable + PERF_OPTIMIZE */
	write32(&active_pll->pll_system_muxes, BIT(6) | BIT(7));
	write32(&active_pll->pll_perf_optimize, 0x22);

	/* Step 4b: Program PLL VCO */
	dsi_phy_pll_vco_270_set_rate(active_pll, vco_hz, pll_post_div, cphy_mode);

	/* Toggle DLN3 TX_DCTRL (required before PLL lock) */
	dsi_phy_toggle_dln3_tx_dctrl();

	/* Step 4c: PLL lock detect */
	ret = dsi_phy_pll_lock_detect();
	if (ret != CB_SUCCESS)
		return ret;

	/* Step 5: Post-lock sequence */
	dsi_phy_post_pll_lock();

	/* UEFI sets C-PHY mode and timings before enabling the DISP_CC clocks. */
	if (cphy_mode)
		dsi_phy_calc_and_commit_cphy_timings(phy_bitclk_hz);

	/* Step 6: Enable DSI clocks */
	ret = dsi_phy_enable_clocks(dsi_pclk_hz, byteclk_hz);
	if (ret != CB_SUCCESS) {
		bool dsi1_active = active_phy == dsi1_phy;
		struct clock_rcg *pclk = dsi1_active ?
			&disp_cc->mdss_pclk1_rcg : &disp_cc->mdss_pclk0_rcg;
		struct clock_rcg *byteclk = dsi1_active ?
			&disp_cc->mdss_byte1_rcg : &disp_cc->mdss_byte0_rcg;
		printk(BIOS_ERR, "DSI PHY: PLL_STATUS=%#x CLK_CFG0=%#x CLK_CFG1=%#x "
			"GLBL_CTRL=%#x\n",
			read32(&active_pll->pll_common_status_one),
			read32(&active_phy->phy_cmn_clk_cfg0),
			read32(&active_phy->phy_cmn_clk_cfg1),
			read32(&active_phy->phy_cmn_glbl_ctrl));
		printk(BIOS_ERR, "DSI clocks: PCLK_CMD=%#x PCLK_CFG=%#x "
			"BYTE_CMD=%#x BYTE_CFG=%#x\n",
			read32(&pclk->rcg_cmd), read32(&pclk->rcg_cfg),
			read32(&byteclk->rcg_cmd), read32(&byteclk->rcg_cfg));
		return ret;
	}

	/* Step 7: D-PHY timing parameters */
	if (!cphy_mode)
		dsi_phy_calc_and_commit_timings(phy_bitclk_hz);

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
	active_phy = dsi1_phy;
	active_pll = phy_pll_qlink1;
	return dsi_phy_init_instance(edid, num_of_lanes, bpp);
}
