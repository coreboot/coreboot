/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSI PHY 2.7.0 PLL (QDIG PLL) VCO rate programming for purwa (x1p42100).
 *
 * Ported from UEFI HALdsi_Phy_2_7_0.c (HAL_DSI_Phy_CommitPll).
 * Reference: QcomPkg/Library/HALDSILib/HALdsi_Phy_2_7_0.c
 *
 * Key differences from sc7180 10nm PLL:
 *  - FRAC_DIV_START_LOW_1 uses bits [3:0] only (not [7:0])
 *  - CLOCK_INVERTERS thresholds: 1000/2500/3500 MHz (not 1100 MHz)
 *  - CORE_INPUT_OVERRIDE: write 0x10 then 0x12 (not just 0x10)
 *  - OUTDIV_RATE: two fields PLL_OUTDIV_1 and PLL_OUTDIV_2 (bits [1:0] and [5:4])
 *  - Many new frequency-independent init registers (see below)
 *  - All math is integer-only (no float)
 */

#include <device/mmio.h>
#include <lib.h>
#include <soc/display/dsi_phy_pll.h>
#include <soc/display/mdssreg.h>
#include <stdint.h>
#include <types.h>

/* Reference clock after prescaler: 19.2 MHz * 2 = 38.4 MHz */
#define DSI_PLL_REF_FREQ_HZ        38400000UL
#define DSI_PLL_FRAC_BITS          18
#define DSI_PLL_LOCK_TIMER         0x40
#define DSI_PLL_LOCK_DELAY         0x06

/* Clock inverter thresholds (VCO freq in MHz) and values */
#define DSI_PLL_CLK_INV_THRESH_0   1000
#define DSI_PLL_CLK_INV_THRESH_1   2500
#define DSI_PLL_CLK_INV_THRESH_2   3500
#define DSI_PLL_CLK_INV_VAL_0      0xA0  /* VCO < 1000 MHz */
#define DSI_PLL_CLK_INV_VAL_1      0x20  /* VCO 1000..2499 MHz */
#define DSI_PLL_CLK_INV_VAL_2      0x00  /* VCO 2500..3499 MHz */
#define DSI_PLL_CLK_INV_VAL_3      0x40  /* VCO >= 3500 MHz */

struct dsi_pll_regs_270 {
	u32 dec_div_start;
	u32 frac_div_low;    /* bits [3:0] only - PHY 2.7.0 difference */
	u32 frac_div_mid;    /* bits [7:0] */
	u32 frac_div_high;   /* bits [1:0] */
	u32 prop_gain_rate;
	u32 clock_inverters;
	u32 analog_controls_five; /* 0x1 if VCO < 3100 MHz, else 0x3 */
	u32 vco_config;           /* 0x00 if VCO > 1557 MHz, else 0x08 */
	u32 outdiv_val;           /* log2(pll_post_div) */
};

static void dsi_pll_calc_regs(struct dsi_pll_regs_270 *regs,
			      unsigned long vco_rate_hz,
			      u32 pll_post_div)
{
	u64 multiplier = (1ULL << DSI_PLL_FRAC_BITS);
	u64 dec_frac   = ((u64)vco_rate_hz * multiplier) / DSI_PLL_REF_FREQ_HZ;
	u32 dec        = (u32)(dec_frac / multiplier);
	u32 frac       = (u32)(dec_frac % multiplier);
	u32 vco_mhz    = vco_rate_hz / 1000000;

	/* Prop gain rate */
	if (vco_mhz <= 1900)
		regs->prop_gain_rate = 8;
	else if (vco_mhz <= 3000)
		regs->prop_gain_rate = 10;
	else
		regs->prop_gain_rate = 12;

	/* Clock inverters */
	if (vco_mhz < DSI_PLL_CLK_INV_THRESH_0)
		regs->clock_inverters = DSI_PLL_CLK_INV_VAL_0;
	else if (vco_mhz < DSI_PLL_CLK_INV_THRESH_1)
		regs->clock_inverters = DSI_PLL_CLK_INV_VAL_1;
	else if (vco_mhz < DSI_PLL_CLK_INV_THRESH_2)
		regs->clock_inverters = DSI_PLL_CLK_INV_VAL_2;
	else
		regs->clock_inverters = DSI_PLL_CLK_INV_VAL_3;

	/* ANALOG_CONTROLS_FIVE: 0x1 if < 3100 MHz, 0x3 if >= 3100 MHz */
	regs->analog_controls_five = (vco_mhz < 3100) ? 0x1 : 0x3;

	/* VCO_CONFIG: 0x00 if > 1557 MHz, 0x08 if <= 1557 MHz */
	regs->vco_config = (vco_mhz > 1557) ? 0x00 : 0x08;

	regs->dec_div_start = dec;
	/* FRAC_DIV_LOW_1: bits [3:0] only (PHY 2.7.0 - not [7:0] like 10nm) */
	regs->frac_div_low  = frac & 0xF;
	regs->frac_div_mid  = (frac >> 8) & 0xFF;
	regs->frac_div_high = (frac >> 16) & 0x3;

	/* log2 of post-div (pll_post_div is always a power of 2) */
	regs->outdiv_val = log2(pll_post_div);
}

void dsi_phy_pll_vco_270_set_rate(unsigned long vco_rate_hz, u32 pll_post_div)
{
	struct dsi_pll_regs_270 regs = {0};

	dsi_pll_calc_regs(&regs, vco_rate_hz, pll_post_div);

	/* Step 1: PLL core input override - reset then enable */
	write32(&phy_pll_qlink->pll_core_input_override, 0x10);
	write32(&phy_pll_qlink->pll_core_input_override, 0x12);

	/* Step 2: Decimal/fractional divider (bank 1) */
	write32(&phy_pll_qlink->pll_decimal_div_start_1,   regs.dec_div_start);
	write32(&phy_pll_qlink->pll_frac_div_start_low_1,  regs.frac_div_low);
	write32(&phy_pll_qlink->pll_frac_div_start_mid_1,  regs.frac_div_mid);
	write32(&phy_pll_qlink->pll_frac_div_start_high_1, regs.frac_div_high);

	/* Step 3: PLL bias enable (SYSTEM_MUXES: CM_BIAS_EN_MUX=bit6, CM_BIAS_EN=bit7) */
	write32(&phy_pll_qlink->pll_system_muxes, BIT(6) | BIT(7));

	/* Step 4: Lock detect rate */
	write32(&phy_pll_qlink->pll_lockdet_rate_1, DSI_PLL_LOCK_TIMER);

	/*
	 * Step 5: Output divider - write log2(post_div) to both PLL_OUTDIV_1[1:0]
	 * and PLL_OUTDIV_2[5:4] fields in PLL_OUTDIV_RATE register
	 */
	write32(&phy_pll_qlink->pll_outdiv_rate,
		(regs.outdiv_val & 0x3) | ((regs.outdiv_val & 0x3) << 4));

	/* Step 6: Lock delay */
	write32(&phy_pll_qlink->pll_lock_delay, DSI_PLL_LOCK_DELAY);

	/* Step 7: Frequency-dependent registers */
	write32(&phy_pll_qlink->pll_analog_controls_five_1, regs.analog_controls_five);
	write32(&phy_pll_qlink->pll_analog_controls_five,   regs.analog_controls_five);
	write32(&phy_pll_qlink->pll_vco_config_1,           regs.vco_config);
	write32(&phy_pll_qlink->pll_clock_inverters_1,      regs.clock_inverters);

	/* Step 8: D-PHY cmode (disable clk90 level shifter) */
	write32(&phy_pll_qlink->pll_cmode_1, 0x10);

	/* Step 9: Frequency-independent init registers (from UEFI HAL_DSI_Phy_CommitPll) */
	write32(&phy_pll_qlink->pll_lock_min_delay,              0x00);
	write32(&phy_pll_qlink->pll_rst_controls,                0x11);
	write32(&phy_pll_qlink->pll_fastlock_control,            0x3F);
	write32(&phy_pll_qlink->pll_analog_controls_two,         0x03);
	write32(&phy_pll_qlink->pll_analog_controls_three,       0x00);
	write32(&phy_pll_qlink->pll_dsm_divider,                 0x00);
	write32(&phy_pll_qlink->pll_feedback_divider,            0x4E);
	write32(&phy_pll_qlink->pll_calibration_settings,        0x40);
	write32(&phy_pll_qlink->pll_band_sel_cal_settings_three, 0xBA);
	write32(&phy_pll_qlink->pll_freq_detect_settings_one,    0x0C);
	write32(&phy_pll_qlink->pll_outdiv,                      0x00);
	write32(&phy_pll_qlink->pll_core_override,               0x00);
	write32(&phy_pll_qlink->pll_digital_timers_two,          0x08);
	write32(&phy_pll_qlink->pll_prop_gain_rate_1,            0x0A);
	write32(&phy_pll_qlink->pll_band_sel_rate_1,             0xC0);
	/* Write twice as per UEFI sequence */
	write32(&phy_pll_qlink->pll_int_gain_ifilt_band_1,       0x84);
	write32(&phy_pll_qlink->pll_int_gain_ifilt_band_1,       0x82);
	write32(&phy_pll_qlink->pll_fl_int_gain_pfilt_band_1,    0x4C);
	write32(&phy_pll_qlink->pll_lock_override,               0x80);
	/* Write twice as per UEFI sequence */
	write32(&phy_pll_qlink->pll_pfilt,                       0x29);
	write32(&phy_pll_qlink->pll_pfilt,                       0x2F);
	write32(&phy_pll_qlink->pll_ifilt,                       0x2A);
	write32(&phy_pll_qlink->pll_ifilt,                       0x3F);
}
