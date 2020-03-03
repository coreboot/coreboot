/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <string.h>
#include <stdint.h>
#include <delay.h>
#include <edid.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>
#include <soc/display/dsi_phy_pll.h>

#define VCO_DELAY_USEC 1

/* Bit definition of SSC control registers */
#define SSC_CENTER		BIT(0)
#define SSC_EN			BIT(1)

struct dsi_pll_regs {
	u32 pll_prop_gain_rate;
	u32 decimal_div_start;
	u32 frac_div_start_low;
	u32 frac_div_start_mid;
	u32 frac_div_start_high;
	u32 pll_clock_inverters;
};

static void dsi_pll_init_val(void)
{
	write32(&phy_pll_qlink->pll_core_input_override, 0x10);
	write32(&phy_pll_qlink->pll_int_loop_settings, 0x3f);
	write32(&phy_pll_qlink->pll_int_loop_settings_two, 0x0);
	write32(&phy_pll_qlink->pll_analog_ctrls_four, 0x0);
	write32(&phy_pll_qlink->pll_int_loop_ctrls, 0x80);
	write32(&phy_pll_qlink->pll_freq_update_ctrl_overrides, 0x0);
	write32(&phy_pll_qlink->pll_band_sel_cal_timer_low, 0x0);
	write32(&phy_pll_qlink->pll_band_sel_cal_timer_high, 0x02);
	write32(&phy_pll_qlink->pll_band_sel_cal_settings, 0x82);
	write32(&phy_pll_qlink->pll_band_sel_min, 0x00);
	write32(&phy_pll_qlink->pll_band_sel_max, 0xff);
	write32(&phy_pll_qlink->pll_band_sel_pfilt, 0x00);
	write32(&phy_pll_qlink->pll_band_sel_ifilt, 0x00);
	write32(&phy_pll_qlink->pll_band_sel_cal_settings_two, 0x25);
	write32(&phy_pll_qlink->pll_band_sel_cal_settings_four, 0x4f);
	write32(&phy_pll_qlink->pll_band_sel_icode_high, 0x0a);
	write32(&phy_pll_qlink->pll_band_sel_icode_low, 0x0);
	write32(&phy_pll_qlink->pll_pll_gain, 0x42);
	write32(&phy_pll_qlink->pll_icode_low, 0x00);
	write32(&phy_pll_qlink->pll_icode_high, 0x00);
	write32(&phy_pll_qlink->pll_lockdet, 0x30);
	write32(&phy_pll_qlink->pll_fastlock_ctrl, 0x04);
	write32(&phy_pll_qlink->pll_pass_out_override_one, 0x00);
	write32(&phy_pll_qlink->pll_pass_out_override_two, 0x00);
	write32(&phy_pll_qlink->pll_rate_change, 0x01);
	write32(&phy_pll_qlink->pll_digital_timers, 0x08);
	write32(&phy_pll_qlink->pll_dec_frac_muxes, 0x00);
	write32(&phy_pll_qlink->pll_mash_ctrl, 0x03);
	write32(&phy_pll_qlink->pll_ssc_mux_ctrl, 0x0);
	write32(&phy_pll_qlink->pll_ssc_ctrl, 0x0);
	write32(&phy_pll_qlink->pll_pll_fastlock_en_band, 0x03);
	write32(&phy_pll_qlink->pll_freq_tune_accum_init_mux, 0x0);
	write32(&phy_pll_qlink->pll_lock_min_delay, 0x19);
	write32(&phy_pll_qlink->pll_spare_and_jpc_overrides, 0x0);
	write32(&phy_pll_qlink->pll_bias_ctrl_1, 0x40);
	write32(&phy_pll_qlink->pll_bias_ctrl_2, 0x20);
	write32(&phy_pll_qlink->pll_alog_obsv_bus_ctrl_1, 0x0);
}

static void dsi_pll_calc_dec_frac(struct dsi_pll_regs *regs,
				  unsigned long rate)
{
	u32 frac_bits = 18;
	u64 pll_freq;
	u64 divider;
	u64 dec, dec_multiple;
	u32 frac;
	u64 multiplier;

	pll_freq = rate;
	divider = SRC_XO_HZ * 2;

	multiplier = 1 << frac_bits;
	dec_multiple = (pll_freq * multiplier) / divider;
	frac = dec_multiple % multiplier;

	dec = dec_multiple / multiplier;
	if (pll_freq <= 1900UL * MHz)
		regs->pll_prop_gain_rate = 8;
	else if (pll_freq <= 3000UL * MHz)
		regs->pll_prop_gain_rate = 10;
	else
		regs->pll_prop_gain_rate = 12;

	if (pll_freq < 1100UL * MHz)
		regs->pll_clock_inverters = 8;
	else
		regs->pll_clock_inverters = 0;

	regs->decimal_div_start = dec;
	regs->frac_div_start_low = (frac & 0xff);
	regs->frac_div_start_mid = (frac & 0xff00) >> 8;
	regs->frac_div_start_high = (frac & 0x30000) >> 16;
}

static void dsi_pll_commit(struct dsi_pll_regs *reg)
{
	write32(&phy_pll_qlink->pll_core_input_override, 0x12);
	write32(&phy_pll_qlink->pll_decimal_div_start_1, reg->decimal_div_start);
	write32(&phy_pll_qlink->pll_frac_div_start_low1, reg->frac_div_start_low);
	write32(&phy_pll_qlink->pll_frac_div_start_mid1, reg->frac_div_start_mid);
	write32(&phy_pll_qlink->pll_frac_div_start_high1, reg->frac_div_start_high);
	write32(&phy_pll_qlink->pll_lockdet_rate[0], 0x40);
	write32(&phy_pll_qlink->pll_lock_delay, 0x06);
	write32(&phy_pll_qlink->pll_cmode, 0x10);
	write32(&phy_pll_qlink->pll_clock_inverters, reg->pll_clock_inverters);
}

static void dsi_pll_config_hzindep_reg(struct dsi_pll_regs *reg)
{
	write32(&phy_pll_qlink->pll_analog_ctrls_one, 0x80);
	write32(&phy_pll_qlink->pll_analog_ctrls_two, 0x03);
	write32(&phy_pll_qlink->pll_analog_ctrls_three, 0x00);
	write32(&phy_pll_qlink->pll_dsm_divider, 0x00);
	write32(&phy_pll_qlink->pll_feedback_divider, 0x4e);
	write32(&phy_pll_qlink->pll_cal_settings, 0x40);
	write32(&phy_pll_qlink->pll_band_sel_cal_settings_three, 0xba);
	write32(&phy_pll_qlink->pll_freq_detect_settings_one, 0x0c);
	write32(&phy_pll_qlink->pll_outdiv, 0x00);
	write32(&phy_pll_qlink->pll_core_override, 0x00);
	write32(&phy_pll_qlink->pll_digital_timers_two, 0x08);
	write32(&phy_pll_qlink->pll_prop_gain_rate[0], reg->pll_prop_gain_rate);
	write32(&phy_pll_qlink->pll_band_set_rate[0], 0xc0);
	write32(&phy_pll_qlink->pll_gain_ifilt_band[0], 0xfa);
	write32(&phy_pll_qlink->pll_fl_int_gain_pfilt_band[0], 0x4c);
	write32(&phy_pll_qlink->pll_lock_override, 0x80);
	write32(&phy_pll_qlink->pll_pfilt, 0x29);
	write32(&phy_pll_qlink->pll_ifilt, 0x3f);
}

void dsi_phy_pll_vco_10nm_set_rate(unsigned long rate)
{
	struct dsi_pll_regs regs;

	dsi_pll_init_val();
	dsi_pll_calc_dec_frac(&regs, rate);
	dsi_pll_commit(&regs);
	dsi_pll_config_hzindep_reg(&regs);
}
