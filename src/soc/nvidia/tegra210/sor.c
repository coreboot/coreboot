/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * drivers/video/tegra/dc/sor.c
 */

#include <console/console.h>
#include <stdint.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <device/device.h>
#include <boot/tables.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/nvidia/tegra/types.h>
#include <soc/sor.h>
#include <soc/nvidia/tegra/displayport.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include "chip.h"
#include <soc/display.h>

#define DEBUG_SOR 0

#define APBDEV_PMC_DPD_SAMPLE				(0x20)
#define APBDEV_PMC_DPD_SAMPLE_ON_DISABLE		(0)
#define APBDEV_PMC_DPD_SAMPLE_ON_ENABLE			(1)
#define APBDEV_PMC_SEL_DPD_TIM				(0x1c8)
#define APBDEV_PMC_SEL_DPD_TIM_SEL_DPD_TIM_DEFAULT	(0x7f)
#define APBDEV_PMC_IO_DPD2_REQ				(0x1c0)
#define APBDEV_PMC_IO_DPD2_REQ_LVDS_SHIFT		(25)
#define APBDEV_PMC_IO_DPD2_REQ_LVDS_OFF			(0 << 25)
#define APBDEV_PMC_IO_DPD2_REQ_LVDS_ON			(1 << 25)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_SHIFT               (30)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_DEFAULT_MASK        (0x3 << 30)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_IDLE                (0 << 30)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_DPD_OFF             (1 << 30)
#define APBDEV_PMC_IO_DPD2_REQ_CODE_DPD_ON              (2 << 30)
#define APBDEV_PMC_IO_DPD2_STATUS			(0x1c4)
#define APBDEV_PMC_IO_DPD2_STATUS_LVDS_SHIFT		(25)
#define APBDEV_PMC_IO_DPD2_STATUS_LVDS_OFF		(0 << 25)
#define APBDEV_PMC_IO_DPD2_STATUS_LVDS_ON		(1 << 25)

#define DC_N_WINDOWS 5

static inline u32 tegra_sor_readl(struct tegra_dc_sor_data *sor, u32 reg)
{
	void *addr = sor->base + (u32)(reg << 2);
	u32 reg_val = READL(addr);
	return reg_val;
}

static inline void tegra_sor_writel(struct tegra_dc_sor_data *sor,
	u32 reg, u32 val)
{
	void *addr = sor->base + (u32)(reg << 2);
	WRITEL(val, addr);
}

static inline void tegra_sor_write_field(struct tegra_dc_sor_data *sor,
	u32 reg, u32 mask, u32 val)
{
	u32 reg_val = tegra_sor_readl(sor, reg);
	reg_val &= ~mask;
	reg_val |= val;
	tegra_sor_writel(sor, reg, reg_val);
}

void tegra_dp_disable_tx_pu(struct tegra_dc_sor_data *sor)
{
	tegra_sor_write_field(sor,
			NV_SOR_DP_PADCTL(sor->portnum),
			NV_SOR_DP_PADCTL_TX_PU_MASK,
			NV_SOR_DP_PADCTL_TX_PU_DISABLE);
}

void tegra_dp_set_pe_vs_pc(struct tegra_dc_sor_data *sor, u32 mask,
			u32 pe_reg, u32 vs_reg, u32 pc_reg, u8 pc_supported)
{
	tegra_sor_write_field(sor, NV_SOR_PR(sor->portnum),
					mask, pe_reg);
	tegra_sor_write_field(sor, NV_SOR_DC(sor->portnum),
					mask, vs_reg);
	if (pc_supported) {
		tegra_sor_write_field(
				sor, NV_SOR_POSTCURSOR(sor->portnum),
				mask, pc_reg);
	}
}

static u32 tegra_dc_sor_poll_register(struct tegra_dc_sor_data *sor,
	u32 reg, u32 mask, u32 exp_val, u32 poll_interval_us, u32 timeout_us)
{
	u32 temp = timeout_us;
	u32 reg_val = 0;

	do {
		udelay(poll_interval_us);
		reg_val = tegra_sor_readl(sor, reg);
		if (timeout_us > poll_interval_us)
			timeout_us -= poll_interval_us;
		else
			break;
	} while ((reg_val & mask) != exp_val);

	if ((reg_val & mask) == exp_val)
		return 0;	/* success */
	printk(BIOS_ERR,
		"sor_poll_register 0x%x: timeout, "
		"(reg_val)0x%08x & (mask)0x%08x != (exp_val)0x%08x\n",
		reg, reg_val, mask, exp_val);

	return temp;
}

int tegra_dc_sor_set_power_state(struct tegra_dc_sor_data *sor, int pu_pd)
{
	u32 reg_val;
	u32 orig_val;

	orig_val = tegra_sor_readl(sor, NV_SOR_PWR);

	reg_val = pu_pd ? NV_SOR_PWR_NORMAL_STATE_PU :
		NV_SOR_PWR_NORMAL_STATE_PD; /* normal state only */

	if (reg_val == orig_val)
		return 0;	/* No update needed */

	reg_val |= NV_SOR_PWR_SETTING_NEW_TRIGGER;
	tegra_sor_writel(sor, NV_SOR_PWR, reg_val);

	/* Poll to confirm it is done */
	if (tegra_dc_sor_poll_register(sor, NV_SOR_PWR,
			NV_SOR_PWR_SETTING_NEW_DEFAULT_MASK,
			NV_SOR_PWR_SETTING_NEW_DONE,
			100, TEGRA_SOR_TIMEOUT_MS * 1000)) {
		printk(BIOS_ERR,
			"dc timeout waiting for SOR_PWR = NEW_DONE\n");
		return -EFAULT;
	}
	return 0;
}

void tegra_dc_sor_set_dp_linkctl(struct tegra_dc_sor_data *sor, int ena,
	u8 training_pattern, const struct tegra_dc_dp_link_config *link_cfg)
{
	u32 reg_val;

	reg_val = tegra_sor_readl(sor, NV_SOR_DP_LINKCTL(sor->portnum));

	if (ena)
		reg_val |= NV_SOR_DP_LINKCTL_ENABLE_YES;
	else
		reg_val &= NV_SOR_DP_LINKCTL_ENABLE_NO;

	reg_val &= ~NV_SOR_DP_LINKCTL_TUSIZE_MASK;
	reg_val |= (link_cfg->tu_size << NV_SOR_DP_LINKCTL_TUSIZE_SHIFT);

	if (link_cfg->enhanced_framing)
		reg_val |= NV_SOR_DP_LINKCTL_ENHANCEDFRAME_ENABLE;

	tegra_sor_writel(sor, NV_SOR_DP_LINKCTL(sor->portnum), reg_val);

	switch (training_pattern) {
	case training_pattern_1:
		tegra_sor_writel(sor, NV_SOR_DP_TPG, 0x41414141);
		break;
	case training_pattern_2:
	case training_pattern_3:
		reg_val = (link_cfg->link_bw == SOR_LINK_SPEED_G5_4) ?
			0x43434343 : 0x42424242;
		tegra_sor_writel(sor, NV_SOR_DP_TPG, reg_val);
		break;
	default:
		tegra_sor_writel(sor, NV_SOR_DP_TPG, 0x50505050);
		break;
	}
}

static int tegra_dc_sor_enable_lane_sequencer(struct tegra_dc_sor_data *sor,
	int pu, int is_lvds)
{
	u32 reg_val;

	/* SOR lane sequencer */
	if (pu)
		reg_val = NV_SOR_LANE_SEQ_CTL_SETTING_NEW_TRIGGER |
			NV_SOR_LANE_SEQ_CTL_SEQUENCE_DOWN |
			NV_SOR_LANE_SEQ_CTL_NEW_POWER_STATE_PU;
	else
		reg_val = NV_SOR_LANE_SEQ_CTL_SETTING_NEW_TRIGGER |
			NV_SOR_LANE_SEQ_CTL_SEQUENCE_UP |
			NV_SOR_LANE_SEQ_CTL_NEW_POWER_STATE_PD;

	if (is_lvds)
		reg_val |= 15 << NV_SOR_LANE_SEQ_CTL_DELAY_SHIFT;
	else
		reg_val |= 1 << NV_SOR_LANE_SEQ_CTL_DELAY_SHIFT;

	tegra_sor_writel(sor, NV_SOR_LANE_SEQ_CTL, reg_val);

	if (tegra_dc_sor_poll_register(sor, NV_SOR_LANE_SEQ_CTL,
			NV_SOR_LANE_SEQ_CTL_SETTING_MASK,
			NV_SOR_LANE_SEQ_CTL_SETTING_NEW_DONE,
			100, TEGRA_SOR_TIMEOUT_MS*1000)) {
		printk(BIOS_ERR,
			"dp: timeout while waiting for SOR lane sequencer "
			"to power down langes\n");
		return -1;
	}
	return 0;
}

static int tegra_dc_sor_power_dplanes(struct tegra_dc_sor_data *sor,
	u32 lane_count, int pu)
{
	u32 reg_val;

	reg_val = tegra_sor_readl(sor, NV_SOR_DP_PADCTL(sor->portnum));

	if (pu) {
		switch (lane_count) {
		case 4:
			reg_val |= (NV_SOR_DP_PADCTL_PD_TXD_3_NO |
				NV_SOR_DP_PADCTL_PD_TXD_2_NO);
			fallthrough;
		case 2:
			reg_val |= NV_SOR_DP_PADCTL_PD_TXD_1_NO;
			fallthrough;
		case 1:
			reg_val |= NV_SOR_DP_PADCTL_PD_TXD_0_NO;
			break;
		default:
			printk(BIOS_ERR,
				"dp: invalid lane number %d\n", lane_count);
			return -1;
		}

		tegra_sor_writel(sor, NV_SOR_DP_PADCTL(sor->portnum), reg_val);
		tegra_dc_sor_set_lane_count(sor, lane_count);
	}
	return tegra_dc_sor_enable_lane_sequencer(sor, pu, 0);
}

void tegra_dc_sor_set_panel_power(struct tegra_dc_sor_data *sor,
	int power_up)
{
	u32 reg_val;

	reg_val = tegra_sor_readl(sor, NV_SOR_DP_PADCTL(sor->portnum));

	if (power_up)
		reg_val &= ~NV_SOR_DP_PADCTL_PAD_CAL_PD_POWERDOWN;
	else
		reg_val |= NV_SOR_DP_PADCTL_PAD_CAL_PD_POWERDOWN;

	tegra_sor_writel(sor, NV_SOR_DP_PADCTL(sor->portnum), reg_val);
}

static void tegra_dc_sor_config_pwm(struct tegra_dc_sor_data *sor, u32 pwm_div,
	u32 pwm_dutycycle)
{
	tegra_sor_writel(sor, NV_SOR_PWM_DIV, pwm_div);
	tegra_sor_writel(sor, NV_SOR_PWM_CTL,
		(pwm_dutycycle & NV_SOR_PWM_CTL_DUTY_CYCLE_MASK) |
		NV_SOR_PWM_CTL_SETTING_NEW_TRIGGER);

	if (tegra_dc_sor_poll_register(sor, NV_SOR_PWM_CTL,
			NV_SOR_PWM_CTL_SETTING_NEW_SHIFT,
			NV_SOR_PWM_CTL_SETTING_NEW_DONE,
			100, TEGRA_SOR_TIMEOUT_MS * 1000)) {
		printk(BIOS_ERR,
			"dp: timeout while waiting for SOR PWM setting\n");
	}
}

static void tegra_dc_sor_set_dp_mode(struct tegra_dc_sor_data *sor,
	const struct tegra_dc_dp_link_config *link_cfg)
{
	u32 reg_val;

	tegra_dc_sor_set_link_bandwidth(sor, link_cfg->link_bw);

	tegra_dc_sor_set_dp_linkctl(sor, 1, training_pattern_none, link_cfg);
	reg_val = tegra_sor_readl(sor, NV_SOR_DP_CONFIG(sor->portnum));
	reg_val &= ~NV_SOR_DP_CONFIG_WATERMARK_MASK;
	reg_val |= link_cfg->watermark;
	reg_val &= ~NV_SOR_DP_CONFIG_ACTIVESYM_COUNT_MASK;
	reg_val |= (link_cfg->active_count <<
		NV_SOR_DP_CONFIG_ACTIVESYM_COUNT_SHIFT);
	reg_val &= ~NV_SOR_DP_CONFIG_ACTIVESYM_FRAC_MASK;
	reg_val |= (link_cfg->active_frac <<
		NV_SOR_DP_CONFIG_ACTIVESYM_FRAC_SHIFT);
	if (link_cfg->activepolarity)
		reg_val |= NV_SOR_DP_CONFIG_ACTIVESYM_POLARITY_POSITIVE;
	else
		reg_val &= ~NV_SOR_DP_CONFIG_ACTIVESYM_POLARITY_POSITIVE;
	reg_val |= (NV_SOR_DP_CONFIG_ACTIVESYM_CNTL_ENABLE |
		NV_SOR_DP_CONFIG_RD_RESET_VAL_NEGATIVE);

	tegra_sor_writel(sor, NV_SOR_DP_CONFIG(sor->portnum), reg_val);

	/* program h/vblank sym */
	tegra_sor_write_field(sor, NV_SOR_DP_AUDIO_HBLANK_SYMBOLS,
		NV_SOR_DP_AUDIO_HBLANK_SYMBOLS_MASK, link_cfg->hblank_sym);

	tegra_sor_write_field(sor, NV_SOR_DP_AUDIO_VBLANK_SYMBOLS,
		NV_SOR_DP_AUDIO_VBLANK_SYMBOLS_MASK, link_cfg->vblank_sym);
}

static inline void tegra_dc_sor_super_update(struct tegra_dc_sor_data *sor)
{
	tegra_sor_writel(sor, NV_SOR_SUPER_STATE0, 0);
	tegra_sor_writel(sor, NV_SOR_SUPER_STATE0, 1);
	tegra_sor_writel(sor, NV_SOR_SUPER_STATE0, 0);
}

static inline void tegra_dc_sor_update(struct tegra_dc_sor_data *sor)
{
	tegra_sor_writel(sor, NV_SOR_STATE0, 0);
	tegra_sor_writel(sor, NV_SOR_STATE0, 1);
	tegra_sor_writel(sor, NV_SOR_STATE0, 0);
}

static void tegra_dc_sor_io_set_dpd(struct tegra_dc_sor_data *sor, int up)
{
	u32 reg_val;
	void *pmc_base = sor->pmc_base;

	if (up) {
		WRITEL(APBDEV_PMC_DPD_SAMPLE_ON_ENABLE,
			pmc_base + APBDEV_PMC_DPD_SAMPLE);
		WRITEL(10, pmc_base + APBDEV_PMC_SEL_DPD_TIM);
	}

	reg_val = READL(pmc_base + APBDEV_PMC_IO_DPD2_REQ);
	reg_val &= ~(APBDEV_PMC_IO_DPD2_REQ_LVDS_ON ||
		APBDEV_PMC_IO_DPD2_REQ_CODE_DEFAULT_MASK);

	reg_val = up ? APBDEV_PMC_IO_DPD2_REQ_LVDS_ON |
		APBDEV_PMC_IO_DPD2_REQ_CODE_DPD_OFF :
		APBDEV_PMC_IO_DPD2_REQ_LVDS_OFF |
		APBDEV_PMC_IO_DPD2_REQ_CODE_DPD_ON;

	WRITEL(reg_val, pmc_base + APBDEV_PMC_IO_DPD2_REQ);

	/* Polling */
	u32 temp = 10*1000;
	do {
		udelay(20);
		reg_val = READL(pmc_base + APBDEV_PMC_IO_DPD2_STATUS);
		if (temp > 20)
			temp -= 20;
		else
			break;
	} while ((reg_val & APBDEV_PMC_IO_DPD2_STATUS_LVDS_ON) != 0);

	if ((reg_val & APBDEV_PMC_IO_DPD2_STATUS_LVDS_ON) != 0)
		printk(BIOS_ERR,
			"PMC_IO_DPD2 polling failed (0x%x)\n", reg_val);

	if (up)
		WRITEL(APBDEV_PMC_DPD_SAMPLE_ON_DISABLE,
			pmc_base + APBDEV_PMC_DPD_SAMPLE);
}

void tegra_dc_sor_set_internal_panel(struct tegra_dc_sor_data *sor, int is_int)
{
	u32 reg_val;

	reg_val = tegra_sor_readl(sor, NV_SOR_DP_SPARE(sor->portnum));
	if (is_int)
		reg_val |= NV_SOR_DP_SPARE_PANEL_INTERNAL;
	else
		reg_val &= ~NV_SOR_DP_SPARE_PANEL_INTERNAL;

	reg_val |= NV_SOR_DP_SPARE_SOR_CLK_SEL_MACRO_SORCLK |
		NV_SOR_DP_SPARE_SEQ_ENABLE_YES;
	tegra_sor_writel(sor, NV_SOR_DP_SPARE(sor->portnum), reg_val);
}

void tegra_dc_sor_read_link_config(struct tegra_dc_sor_data *sor, u8 *link_bw,
				   u8 *lane_count)
{
	u32 reg_val;

	reg_val = tegra_sor_readl(sor, NV_SOR_CLK_CNTRL);
	*link_bw = (reg_val & NV_SOR_CLK_CNTRL_DP_LINK_SPEED_MASK)
		>> NV_SOR_CLK_CNTRL_DP_LINK_SPEED_SHIFT;
	reg_val = tegra_sor_readl(sor,
		NV_SOR_DP_LINKCTL(sor->portnum));

	switch (reg_val & NV_SOR_DP_LINKCTL_LANECOUNT_MASK) {
	case NV_SOR_DP_LINKCTL_LANECOUNT_ZERO:
		*lane_count = 0;
		break;
	case NV_SOR_DP_LINKCTL_LANECOUNT_ONE:
		*lane_count = 1;
		break;
	case NV_SOR_DP_LINKCTL_LANECOUNT_TWO:
		*lane_count = 2;
		break;
	case NV_SOR_DP_LINKCTL_LANECOUNT_FOUR:
		*lane_count = 4;
		break;
	default:
		printk(BIOS_ERR, "Unknown lane count\n");
	}
}

void tegra_dc_sor_set_link_bandwidth(struct tegra_dc_sor_data *sor, u8 link_bw)
{
	tegra_sor_write_field(sor, NV_SOR_CLK_CNTRL,
		NV_SOR_CLK_CNTRL_DP_LINK_SPEED_MASK,
		link_bw << NV_SOR_CLK_CNTRL_DP_LINK_SPEED_SHIFT);
}

void tegra_dc_sor_set_lane_count(struct tegra_dc_sor_data *sor, u8 lane_count)
{
	u32 reg_val;

	reg_val = tegra_sor_readl(sor, NV_SOR_DP_LINKCTL(sor->portnum));
	reg_val &= ~NV_SOR_DP_LINKCTL_LANECOUNT_MASK;
	switch (lane_count) {
	case 0:
		break;
	case 1:
		reg_val |= NV_SOR_DP_LINKCTL_LANECOUNT_ONE;
		break;
	case 2:
		reg_val |= NV_SOR_DP_LINKCTL_LANECOUNT_TWO;
		break;
	case 4:
		reg_val |= NV_SOR_DP_LINKCTL_LANECOUNT_FOUR;
		break;
	default:
		/* 0 should be handled earlier. */
		printk(BIOS_ERR, "dp: Invalid lane count %d\n",
			lane_count);
		return;
	}
	tegra_sor_writel(sor, NV_SOR_DP_LINKCTL(sor->portnum), reg_val);
}

static void tegra_sor_enable_edp_clock(struct tegra_dc_sor_data *sor)
{
	sor_clock_start();
}

/* The SOR power sequencer does not work for t1xx so SW has to
   go through the power sequence manually */
/* Power up steps from spec: */
/* STEP	PDPORT	PDPLL	PDBG	PLLVCOD	PLLCAPD	E_DPD	PDCAL */
/* 1	1	1	1	1	1	1	1 */
/* 2	1	1	1	1	1	0	1 */
/* 3	1	1	0	1	1	0	1 */
/* 4	1	0	0	0	0	0	1 */
/* 5	0	0	0	0	0	0	1 */
static void tegra_dc_sor_power_up(struct tegra_dc_sor_data *sor,
				int is_lvds)
{
	if (sor->power_is_up)
		return;

	/* Set link bw */
	tegra_dc_sor_set_link_bandwidth(sor,
		is_lvds ? NV_SOR_CLK_CNTRL_DP_LINK_SPEED_LVDS :
		NV_SOR_CLK_CNTRL_DP_LINK_SPEED_G1_62);

	/* step 1 */
	tegra_sor_write_field(sor, NV_SOR_PLL2,
		NV_SOR_PLL2_AUX7_PORT_POWERDOWN_MASK | /* PDPORT */
		NV_SOR_PLL2_AUX6_BANDGAP_POWERDOWN_MASK | /* PDBG */
		NV_SOR_PLL2_AUX8_SEQ_PLLCAPPD_ENFORCE_MASK, /* PLLCAPD */
		NV_SOR_PLL2_AUX7_PORT_POWERDOWN_ENABLE |
		NV_SOR_PLL2_AUX6_BANDGAP_POWERDOWN_ENABLE |
		NV_SOR_PLL2_AUX8_SEQ_PLLCAPPD_ENFORCE_ENABLE);
	tegra_sor_write_field(sor, NV_SOR_PLL0,
		NV_SOR_PLL0_PWR_MASK | /* PDPLL */
		NV_SOR_PLL0_VCOPD_MASK, /* PLLVCOPD */
		NV_SOR_PLL0_PWR_OFF |
		NV_SOR_PLL0_VCOPD_ASSERT);
	tegra_sor_write_field(sor, NV_SOR_DP_PADCTL(sor->portnum),
		NV_SOR_DP_PADCTL_PAD_CAL_PD_POWERDOWN, /* PDCAL */
		NV_SOR_DP_PADCTL_PAD_CAL_PD_POWERUP);

	/* step 2 */
	tegra_dc_sor_io_set_dpd(sor, 1);
	udelay(15);

	/* step 3 */
	tegra_sor_write_field(sor, NV_SOR_PLL2,
		NV_SOR_PLL2_AUX6_BANDGAP_POWERDOWN_MASK,
		NV_SOR_PLL2_AUX6_BANDGAP_POWERDOWN_DISABLE);
	udelay(25);

	/* step 4 */
	tegra_sor_write_field(sor, NV_SOR_PLL0,
		NV_SOR_PLL0_PWR_MASK | /* PDPLL */
		NV_SOR_PLL0_VCOPD_MASK, /* PLLVCOPD */
		NV_SOR_PLL0_PWR_ON | NV_SOR_PLL0_VCOPD_RESCIND);
	tegra_sor_write_field(sor, NV_SOR_PLL2,
		NV_SOR_PLL2_AUX8_SEQ_PLLCAPPD_ENFORCE_MASK, /* PLLCAPD */
		NV_SOR_PLL2_AUX8_SEQ_PLLCAPPD_ENFORCE_DISABLE);
	udelay(225);

	/* step 5 */
	tegra_sor_write_field(sor, NV_SOR_PLL2,
		NV_SOR_PLL2_AUX7_PORT_POWERDOWN_MASK, /* PDPORT */
		NV_SOR_PLL2_AUX7_PORT_POWERDOWN_DISABLE);

	sor->power_is_up = 1;
}

#if DEBUG_SOR
static void dump_sor_reg(struct tegra_dc_sor_data *sor)
{
#define DUMP_REG(a) printk(BIOS_INFO, "%-32s  %03x  %08x\n",		\
		#a, a, tegra_sor_readl(sor, a))

	DUMP_REG(NV_SOR_SUPER_STATE0);
	DUMP_REG(NV_SOR_SUPER_STATE1);
	DUMP_REG(NV_SOR_STATE0);
	DUMP_REG(NV_SOR_STATE1);
	DUMP_REG(NV_HEAD_STATE0(0));
	DUMP_REG(NV_HEAD_STATE0(1));
	DUMP_REG(NV_HEAD_STATE1(0));
	DUMP_REG(NV_HEAD_STATE1(1));
	DUMP_REG(NV_HEAD_STATE2(0));
	DUMP_REG(NV_HEAD_STATE2(1));
	DUMP_REG(NV_HEAD_STATE3(0));
	DUMP_REG(NV_HEAD_STATE3(1));
	DUMP_REG(NV_HEAD_STATE4(0));
	DUMP_REG(NV_HEAD_STATE4(1));
	DUMP_REG(NV_HEAD_STATE5(0));
	DUMP_REG(NV_HEAD_STATE5(1));
	DUMP_REG(NV_SOR_CRC_CNTRL);
	DUMP_REG(NV_SOR_CLK_CNTRL);
	DUMP_REG(NV_SOR_CAP);
	DUMP_REG(NV_SOR_PWR);
	DUMP_REG(NV_SOR_TEST);
	DUMP_REG(NV_SOR_PLL0);
	DUMP_REG(NV_SOR_PLL1);
	DUMP_REG(NV_SOR_PLL2);
	DUMP_REG(NV_SOR_PLL3);
	DUMP_REG(NV_SOR_CSTM);
	DUMP_REG(NV_SOR_LVDS);
	DUMP_REG(NV_SOR_CRCA);
	DUMP_REG(NV_SOR_CRCB);
	DUMP_REG(NV_SOR_SEQ_CTL);
	DUMP_REG(NV_SOR_LANE_SEQ_CTL);
	DUMP_REG(NV_SOR_SEQ_INST(0));
	DUMP_REG(NV_SOR_SEQ_INST(1));
	DUMP_REG(NV_SOR_SEQ_INST(2));
	DUMP_REG(NV_SOR_SEQ_INST(3));
	DUMP_REG(NV_SOR_SEQ_INST(4));
	DUMP_REG(NV_SOR_SEQ_INST(5));
	DUMP_REG(NV_SOR_SEQ_INST(6));
	DUMP_REG(NV_SOR_SEQ_INST(7));
	DUMP_REG(NV_SOR_SEQ_INST(8));
	DUMP_REG(NV_SOR_PWM_DIV);
	DUMP_REG(NV_SOR_PWM_CTL);
	DUMP_REG(NV_SOR_MSCHECK);
	DUMP_REG(NV_SOR_XBAR_CTRL);
	DUMP_REG(NV_SOR_DP_LINKCTL(0));
	DUMP_REG(NV_SOR_DP_LINKCTL(1));
	DUMP_REG(NV_SOR_DC(0));
	DUMP_REG(NV_SOR_DC(1));
	DUMP_REG(NV_SOR_LANE_DRIVE_CURRENT(0));
	DUMP_REG(NV_SOR_PR(0));
	DUMP_REG(NV_SOR_LANE4_PREEMPHASIS(0));
	DUMP_REG(NV_SOR_POSTCURSOR(0));
	DUMP_REG(NV_SOR_DP_CONFIG(0));
	DUMP_REG(NV_SOR_DP_CONFIG(1));
	DUMP_REG(NV_SOR_DP_MN(0));
	DUMP_REG(NV_SOR_DP_MN(1));
	DUMP_REG(NV_SOR_DP_PADCTL(0));
	DUMP_REG(NV_SOR_DP_PADCTL(1));
	DUMP_REG(NV_SOR_DP_DEBUG(0));
	DUMP_REG(NV_SOR_DP_DEBUG(1));
	DUMP_REG(NV_SOR_DP_SPARE(0));
	DUMP_REG(NV_SOR_DP_SPARE(1));
	DUMP_REG(NV_SOR_DP_TPG);
}
#endif

static void tegra_dc_sor_config_panel(struct tegra_dc_sor_data *sor,
	int is_lvds)
{
	const struct tegra_dc *dc = sor->dc;
	const struct tegra_dc_dp_data *dp = dc->out;
	const struct tegra_dc_dp_link_config *link_cfg = &dp->link_cfg;
	const struct soc_nvidia_tegra210_config *config = dc->config;

	const int	head_num = 0;
	u32		reg_val	 = NV_SOR_STATE1_ASY_OWNER_HEAD0 << head_num;
	u32		vsync_end, hsync_end;
	u32		vblank_end, hblank_end;
	u32		vblank_start, hblank_start;

	reg_val |= is_lvds ? NV_SOR_STATE1_ASY_PROTOCOL_LVDS_CUSTOM :
		NV_SOR_STATE1_ASY_PROTOCOL_DP_A;
	reg_val |= NV_SOR_STATE1_ASY_SUBOWNER_NONE |
		NV_SOR_STATE1_ASY_CRCMODE_COMPLETE_RASTER;

	reg_val |= NV_SOR_STATE1_ASY_HSYNCPOL_NEGATIVE_TRUE;
	reg_val |= NV_SOR_STATE1_ASY_VSYNCPOL_NEGATIVE_TRUE;
	reg_val |= (link_cfg->bits_per_pixel > 18) ?
		NV_SOR_STATE1_ASY_PIXELDEPTH_BPP_24_444 :
		NV_SOR_STATE1_ASY_PIXELDEPTH_BPP_18_444;

	tegra_sor_writel(sor, NV_SOR_STATE1, reg_val);

	/* Skipping programming NV_HEAD_STATE0, assuming:
	   interlacing: PROGRESSIVE, dynamic range: VESA, colorspace: RGB */

	tegra_sor_writel(sor, NV_HEAD_STATE1(head_num),
		vtotal(config) << NV_HEAD_STATE1_VTOTAL_SHIFT |
		htotal(config) << NV_HEAD_STATE1_HTOTAL_SHIFT);

	vsync_end = config->vsync_width - 1;
	hsync_end = config->hsync_width - 1;
	tegra_sor_writel(sor, NV_HEAD_STATE2(head_num),
		vsync_end << NV_HEAD_STATE2_VSYNC_END_SHIFT |
		hsync_end << NV_HEAD_STATE2_HSYNC_END_SHIFT);

	vblank_end = vsync_end + config->vback_porch;
	hblank_end = hsync_end + config->hback_porch;
	tegra_sor_writel(sor, NV_HEAD_STATE3(head_num),
		vblank_end << NV_HEAD_STATE3_VBLANK_END_SHIFT |
		hblank_end << NV_HEAD_STATE3_HBLANK_END_SHIFT);

	vblank_start = vblank_end + config->yres;
	hblank_start = hblank_end + config->xres;
	tegra_sor_writel(sor, NV_HEAD_STATE4(head_num),
		vblank_start << NV_HEAD_STATE4_VBLANK_START_SHIFT |
		hblank_start << NV_HEAD_STATE4_HBLANK_START_SHIFT);

	/* TODO: adding interlace mode support */
	tegra_sor_writel(sor, NV_HEAD_STATE5(head_num), 0x1);

	tegra_sor_write_field(sor, NV_SOR_CSTM,
		NV_SOR_CSTM_ROTCLK_DEFAULT_MASK |
		NV_SOR_CSTM_LVDS_EN_ENABLE,
		2 << NV_SOR_CSTM_ROTCLK_SHIFT |
		is_lvds ? NV_SOR_CSTM_LVDS_EN_ENABLE :
		NV_SOR_CSTM_LVDS_EN_DISABLE);

	 tegra_dc_sor_config_pwm(sor, 1024, 1024);
}

static void tegra_dc_sor_enable_dc(struct tegra_dc_sor_data *sor)
{
	struct tegra_dc		*dc   = sor->dc;
	struct display_controller *disp_ctrl = (void *)dc->base;

	u32	reg_val = READL(&disp_ctrl->cmd.state_access);

	WRITEL(reg_val | WRITE_MUX_ACTIVE, &disp_ctrl->cmd.state_access);
	WRITEL(VSYNC_H_POSITION(1), &disp_ctrl->disp.disp_timing_opt);

	/* Enable DC now - otherwise pure text console may not show. */
	WRITEL(DISP_CTRL_MODE_C_DISPLAY, &disp_ctrl->cmd.disp_cmd);
	WRITEL(reg_val, &disp_ctrl->cmd.state_access);
}

void tegra_dc_sor_enable_dp(struct tegra_dc_sor_data *sor)
{
	const struct tegra_dc_dp_link_config *link_cfg = sor->link_cfg;

	tegra_sor_write_field(sor, NV_SOR_CLK_CNTRL,
		NV_SOR_CLK_CNTRL_DP_CLK_SEL_MASK,
		NV_SOR_CLK_CNTRL_DP_CLK_SEL_SINGLE_DPCLK);

	tegra_sor_write_field(sor, NV_SOR_PLL2,
		NV_SOR_PLL2_AUX6_BANDGAP_POWERDOWN_MASK,
		NV_SOR_PLL2_AUX6_BANDGAP_POWERDOWN_DISABLE);
	udelay(25);

	tegra_sor_write_field(sor, NV_SOR_PLL3,
		NV_SOR_PLL3_PLLVDD_MODE_MASK,
		NV_SOR_PLL3_PLLVDD_MODE_V3_3);
	tegra_sor_writel(sor, NV_SOR_PLL0,
		0xf << NV_SOR_PLL0_ICHPMP_SHFIT |
		0x3 << NV_SOR_PLL0_VCOCAP_SHIFT |
		NV_SOR_PLL0_PLLREG_LEVEL_V45 |
		NV_SOR_PLL0_RESISTORSEL_EXT |
		NV_SOR_PLL0_PWR_ON | NV_SOR_PLL0_VCOPD_RESCIND);
	tegra_sor_write_field(sor, NV_SOR_PLL2,
		NV_SOR_PLL2_AUX1_SEQ_MASK | NV_SOR_PLL2_AUX9_LVDSEN_OVERRIDE |
		NV_SOR_PLL2_AUX8_SEQ_PLLCAPPD_ENFORCE_MASK,
		NV_SOR_PLL2_AUX1_SEQ_PLLCAPPD_OVERRIDE |
		NV_SOR_PLL2_AUX9_LVDSEN_OVERRIDE |
		NV_SOR_PLL2_AUX8_SEQ_PLLCAPPD_ENFORCE_DISABLE);
	tegra_sor_writel(sor, NV_SOR_PLL1,
		NV_SOR_PLL1_TERM_COMPOUT_HIGH | NV_SOR_PLL1_TMDS_TERM_ENABLE);

	if (tegra_dc_sor_poll_register(sor, NV_SOR_PLL2,
			NV_SOR_PLL2_AUX8_SEQ_PLLCAPPD_ENFORCE_MASK,
			NV_SOR_PLL2_AUX8_SEQ_PLLCAPPD_ENFORCE_DISABLE,
			100, TEGRA_SOR_TIMEOUT_MS * 1000)) {
		printk(BIOS_ERR, "DP failed to lock PLL\n");
		return;
	}

	tegra_sor_write_field(sor, NV_SOR_PLL2,
		NV_SOR_PLL2_AUX2_MASK | NV_SOR_PLL2_AUX7_PORT_POWERDOWN_MASK,
		NV_SOR_PLL2_AUX2_OVERRIDE_POWERDOWN |
		NV_SOR_PLL2_AUX7_PORT_POWERDOWN_DISABLE);

	tegra_dc_sor_power_up(sor, 0);

	/* re-enable SOR clock */
	tegra_sor_enable_edp_clock(sor); /* select pll_dp as clock source */

	/* Power up lanes */
	tegra_dc_sor_power_dplanes(sor, link_cfg->lane_count, 1);

	tegra_dc_sor_set_dp_mode(sor, link_cfg);

}

void tegra_dc_sor_attach(struct tegra_dc_sor_data *sor)
{
	u32 reg_val;
	struct display_controller *disp_ctrl = (void *)sor->dc->base;

	tegra_dc_sor_enable_dc(sor);
	tegra_dc_sor_config_panel(sor, 0);

	WRITEL(0x9f00, &disp_ctrl->cmd.state_ctrl);
	WRITEL(0x9f, &disp_ctrl->cmd.state_ctrl);

	WRITEL(PW0_ENABLE | PW1_ENABLE | PW2_ENABLE |
		PW3_ENABLE | PW4_ENABLE | PM0_ENABLE | PM1_ENABLE,
		&disp_ctrl->cmd.disp_pow_ctrl);

	reg_val = tegra_sor_readl(sor, NV_SOR_TEST);
	if (reg_val & NV_SOR_TEST_ATTACHED_TRUE)
		return;

	tegra_sor_writel(sor, NV_SOR_SUPER_STATE1,
			NV_SOR_SUPER_STATE1_ATTACHED_NO);

	/*
	 * Enable display2sor clock at least 2 cycles before DC start,
	 * to clear sor internal valid signal.
	 */

	/* Stop dc for 3 cycles */
	WRITEL(0, &disp_ctrl->disp.disp_win_opt);
	WRITEL(GENERAL_ACT_REQ, &disp_ctrl->cmd.state_ctrl);
	udelay(FRAME_IN_MS * 1000 * 3);

	/* Attach head */
	tegra_dc_sor_update(sor);
	tegra_sor_writel(sor, NV_SOR_SUPER_STATE1,
			NV_SOR_SUPER_STATE1_ATTACHED_YES);
	tegra_sor_writel(sor, NV_SOR_SUPER_STATE1,
		NV_SOR_SUPER_STATE1_ATTACHED_YES |
		NV_SOR_SUPER_STATE1_ASY_HEAD_OP_AWAKE |
		NV_SOR_SUPER_STATE1_ASY_ORMODE_NORMAL);
	tegra_dc_sor_super_update(sor);

	/* wait for another 5 cycles */
	udelay(FRAME_IN_MS * 1000 * 5);

	/* Re-enable dc */
	WRITEL(READ_MUX_ACTIVE | WRITE_MUX_ACTIVE,
		&disp_ctrl->cmd.state_access);
	WRITEL(SOR_ENABLE, &disp_ctrl->disp.disp_win_opt);

	WRITEL(DISP_CTRL_MODE_C_DISPLAY, &disp_ctrl->cmd.disp_cmd);
	WRITEL(GENERAL_ACT_REQ, &disp_ctrl->cmd.state_ctrl);

	if (tegra_dc_sor_poll_register(sor, NV_SOR_TEST,
			NV_SOR_TEST_ACT_HEAD_OPMODE_DEFAULT_MASK,
			NV_SOR_TEST_ACT_HEAD_OPMODE_AWAKE,
			100, TEGRA_SOR_ATTACH_TIMEOUT_MS * 1000))
		printk(BIOS_ERR, "dc timeout waiting for OPMOD = AWAKE\n");
	else
		printk(BIOS_INFO, "%s: sor is attached\n", __func__);

#if DEBUG_SOR
	dump_sor_reg(sor);
#endif
}

void tegra_dc_sor_set_lane_parm(struct tegra_dc_sor_data *sor,
	const struct tegra_dc_dp_link_config *link_cfg)
{
	tegra_sor_writel(sor, NV_SOR_LANE_DRIVE_CURRENT(sor->portnum),
		link_cfg->drive_current);
	tegra_sor_writel(sor, NV_SOR_PR(sor->portnum),
		link_cfg->preemphasis);
	tegra_sor_writel(sor, NV_SOR_POSTCURSOR(sor->portnum),
		link_cfg->postcursor);
	tegra_sor_writel(sor, NV_SOR_LVDS, 0);

	tegra_dc_sor_set_link_bandwidth(sor, link_cfg->link_bw);
	tegra_dc_sor_set_lane_count(sor, link_cfg->lane_count);

	tegra_sor_write_field(sor, NV_SOR_DP_PADCTL(sor->portnum),
		NV_SOR_DP_PADCTL_TX_PU_ENABLE |
		NV_SOR_DP_PADCTL_TX_PU_VALUE_DEFAULT_MASK,
		NV_SOR_DP_PADCTL_TX_PU_ENABLE |
		2 << NV_SOR_DP_PADCTL_TX_PU_VALUE_SHIFT);

	/* Precharge */
	tegra_sor_write_field(sor, NV_SOR_DP_PADCTL(sor->portnum),
				0xf0, 0xf0);
	udelay(20);

	tegra_sor_write_field(sor, NV_SOR_DP_PADCTL(sor->portnum),
				0xf0, 0x0);
}

void tegra_dc_sor_set_voltage_swing(struct tegra_dc_sor_data *sor)
{
	u32 drive_current = 0;
	u32 pre_emphasis = 0;

	/* Set to a known-good pre-calibrated setting */
	switch (sor->link_cfg->link_bw) {
	case SOR_LINK_SPEED_G1_62:
	case SOR_LINK_SPEED_G2_7:
		drive_current = 0x13131313;
		pre_emphasis = 0;
		break;
	case SOR_LINK_SPEED_G5_4:
		printk(BIOS_WARNING, "T1xx does not support 5.4G link"
			" clock.\n");
	default:
		printk(BIOS_WARNING, "Invalid sor link bandwidth: %d\n",
			sor->link_cfg->link_bw);
		return;
	}

	tegra_sor_writel(sor, NV_SOR_LANE_DRIVE_CURRENT(sor->portnum),
				drive_current);
	tegra_sor_writel(sor, NV_SOR_PR(sor->portnum), pre_emphasis);
}

void tegra_dc_sor_power_down_unused_lanes(struct tegra_dc_sor_data *sor)
{
	u32 pad_ctrl = 0;
	int err = 0;

	switch (sor->link_cfg->lane_count) {
	case 4:
		pad_ctrl = (NV_SOR_DP_PADCTL_PD_TXD_0_NO |
			NV_SOR_DP_PADCTL_PD_TXD_1_NO |
			NV_SOR_DP_PADCTL_PD_TXD_2_NO |
			NV_SOR_DP_PADCTL_PD_TXD_3_NO);
		break;
	case 2:
		pad_ctrl = (NV_SOR_DP_PADCTL_PD_TXD_0_NO |
			NV_SOR_DP_PADCTL_PD_TXD_1_NO |
			NV_SOR_DP_PADCTL_PD_TXD_2_YES |
			NV_SOR_DP_PADCTL_PD_TXD_3_YES);
		break;
	case 1:
		pad_ctrl = (NV_SOR_DP_PADCTL_PD_TXD_0_NO |
			NV_SOR_DP_PADCTL_PD_TXD_1_YES |
			NV_SOR_DP_PADCTL_PD_TXD_2_YES |
			NV_SOR_DP_PADCTL_PD_TXD_3_YES);
		break;
	default:
		printk(BIOS_ERR, "Invalid sor lane count: %u\n",
			sor->link_cfg->lane_count);
		return;
	}

	pad_ctrl |= NV_SOR_DP_PADCTL_PAD_CAL_PD_POWERDOWN;
	tegra_sor_writel(sor, NV_SOR_DP_PADCTL(sor->portnum), pad_ctrl);

	err = tegra_dc_sor_enable_lane_sequencer(sor, 0, 0);
	if (err) {
		printk(BIOS_ERR,
			"Wait for lane power down failed: %d\n", err);
		return;
	}
}

void tegra_sor_precharge_lanes(struct tegra_dc_sor_data *sor)
{
	const struct tegra_dc_dp_link_config *cfg = sor->link_cfg;
	u32 val = 0;

	switch (cfg->lane_count) {
	case 4:
		val |= (NV_SOR_DP_PADCTL_PD_TXD_3_NO |
			NV_SOR_DP_PADCTL_PD_TXD_2_NO);
		fallthrough;
	case 2:
		val |= NV_SOR_DP_PADCTL_PD_TXD_1_NO;
		fallthrough;
	case 1:
		val |= NV_SOR_DP_PADCTL_PD_TXD_0_NO;
		break;
	default:
		printk(BIOS_ERR,
			"dp: invalid lane number %d\n", cfg->lane_count);
		return;
	}

	tegra_sor_write_field(sor, NV_SOR_DP_PADCTL(sor->portnum),
		(0xf << NV_SOR_DP_PADCTL_COMODE_TXD_0_DP_TXD_2_SHIFT),
		(val << NV_SOR_DP_PADCTL_COMODE_TXD_0_DP_TXD_2_SHIFT));
	udelay(100);
	tegra_sor_write_field(sor, NV_SOR_DP_PADCTL(sor->portnum),
		(0xf << NV_SOR_DP_PADCTL_COMODE_TXD_0_DP_TXD_2_SHIFT), 0);
}

static u32 tegra_dc_poll_register(void *reg,
	u32 mask, u32 exp_val, u32 poll_interval_us, u32 timeout_us)
{
	u32 temp = timeout_us;
	u32 reg_val = 0;

	do {
		udelay(poll_interval_us);
		reg_val = READL(reg);
		if (timeout_us > poll_interval_us)
			timeout_us -= poll_interval_us;
		else
			break;
	} while ((reg_val & mask) != exp_val);

	if ((reg_val & mask) == exp_val)
		return 0;	/* success */

	return temp;
}

static void tegra_dc_sor_general_act(struct display_controller *disp_ctrl)
{
	WRITEL(GENERAL_ACT_REQ, &disp_ctrl->cmd.state_ctrl);

	if (tegra_dc_poll_register(&disp_ctrl->cmd.state_ctrl,
		GENERAL_ACT_REQ, 0, 100,
		TEGRA_DC_POLL_TIMEOUT_MS*1000))
		printk(BIOS_ERR,
			"dc timeout waiting for DC to stop\n");
}

static struct tegra_dc_mode min_mode = {
	.h_ref_to_sync = 0,
	.v_ref_to_sync = 1,
	.h_sync_width = 1,
	.v_sync_width = 1,
	.h_back_porch = 20,
	.v_back_porch = 0,
	.h_active = 16,
	.v_active = 16,
	.h_front_porch = 1,
	.v_front_porch = 2,
};

/* Disable windows and set minimum raster timings */
static void
tegra_dc_sor_disable_win_short_raster(struct display_controller *disp_ctrl,
					int *dc_reg_ctx)
{
	int selected_windows, i;

	selected_windows = READL(&disp_ctrl->cmd.disp_win_header);

	/* Store and clear window options */
	for (i = 0; i < DC_N_WINDOWS; ++i) {
		WRITEL(WINDOW_A_SELECT << i, &disp_ctrl->cmd.disp_win_header);
		dc_reg_ctx[i] = READL(&disp_ctrl->win.win_opt);
		WRITEL(0, &disp_ctrl->win.win_opt);
		WRITEL(WIN_A_ACT_REQ << i, &disp_ctrl->cmd.state_ctrl);
	}

	WRITEL(selected_windows, &disp_ctrl->cmd.disp_win_header);

	/* Store current raster timings and set minimum timings */
	dc_reg_ctx[i++] = READL(&disp_ctrl->disp.ref_to_sync);
	WRITEL(min_mode.h_ref_to_sync | (min_mode.v_ref_to_sync << 16),
		&disp_ctrl->disp.ref_to_sync);

	dc_reg_ctx[i++] = READL(&disp_ctrl->disp.sync_width);
	WRITEL(min_mode.h_sync_width | (min_mode.v_sync_width << 16),
		&disp_ctrl->disp.sync_width);

	dc_reg_ctx[i++] = READL(&disp_ctrl->disp.back_porch);
	WRITEL(min_mode.h_back_porch |
		min_mode.v_back_porch << 16,
		&disp_ctrl->disp.back_porch);

	dc_reg_ctx[i++] = READL(&disp_ctrl->disp.front_porch);
	WRITEL(min_mode.h_front_porch |
		min_mode.v_front_porch << 16,
		&disp_ctrl->disp.front_porch);

	dc_reg_ctx[i++] = READL(&disp_ctrl->disp.disp_active);
	WRITEL(min_mode.h_active | (min_mode.v_active << 16),
		&disp_ctrl->disp.disp_active);

	WRITEL(GENERAL_ACT_REQ, &disp_ctrl->cmd.state_ctrl);
}

/* Restore previous windows status and raster timings */
static void
tegra_dc_sor_restore_win_and_raster(struct display_controller *disp_ctrl,
					int *dc_reg_ctx)
{
	int selected_windows, i;

	selected_windows = READL(&disp_ctrl->cmd.disp_win_header);

	for (i = 0; i < DC_N_WINDOWS; ++i) {
		WRITEL(WINDOW_A_SELECT << i, &disp_ctrl->cmd.disp_win_header);
		WRITEL(dc_reg_ctx[i], &disp_ctrl->win.win_opt);
		WRITEL(WIN_A_ACT_REQ << i, &disp_ctrl->cmd.state_ctrl);
	}

	WRITEL(selected_windows, &disp_ctrl->cmd.disp_win_header);

	WRITEL(dc_reg_ctx[i++], &disp_ctrl->disp.ref_to_sync);
	WRITEL(dc_reg_ctx[i++], &disp_ctrl->disp.sync_width);
	WRITEL(dc_reg_ctx[i++], &disp_ctrl->disp.back_porch);
	WRITEL(dc_reg_ctx[i++], &disp_ctrl->disp.front_porch);
	WRITEL(dc_reg_ctx[i++], &disp_ctrl->disp.disp_active);

	WRITEL(GENERAL_UPDATE, &disp_ctrl->cmd.state_ctrl);
}

static void tegra_dc_sor_enable_sor(struct tegra_dc_sor_data *sor, int enable)
{
	struct display_controller *disp_ctrl = (void *)sor->dc->base;
	u32 reg_val = READL(&disp_ctrl->disp.disp_win_opt);

	reg_val = enable ? reg_val | SOR_ENABLE : reg_val & ~SOR_ENABLE;
	WRITEL(reg_val, &disp_ctrl->disp.disp_win_opt);
}

void tegra_dc_detach(struct tegra_dc_sor_data *sor)
{
	struct display_controller *disp_ctrl = (void *)sor->dc->base;
	int dc_reg_ctx[DC_N_WINDOWS + 5];
	unsigned long dc_int_mask;

	/* Sleep mode */
	tegra_sor_writel(sor, NV_SOR_SUPER_STATE1,
		NV_SOR_SUPER_STATE1_ASY_HEAD_OP_SLEEP |
		NV_SOR_SUPER_STATE1_ASY_ORMODE_SAFE |
		NV_SOR_SUPER_STATE1_ATTACHED_YES);
	tegra_dc_sor_super_update(sor);

	tegra_dc_sor_disable_win_short_raster(disp_ctrl, dc_reg_ctx);

	if (tegra_dc_sor_poll_register(sor, NV_SOR_TEST,
		NV_SOR_TEST_ACT_HEAD_OPMODE_DEFAULT_MASK,
		NV_SOR_TEST_ACT_HEAD_OPMODE_SLEEP,
		100, TEGRA_SOR_ATTACH_TIMEOUT_MS*1000)) {
		printk(BIOS_ERR,
			"dc timeout waiting for OPMOD = SLEEP\n");
	}

	tegra_sor_writel(sor, NV_SOR_SUPER_STATE1,
		NV_SOR_SUPER_STATE1_ASY_HEAD_OP_SLEEP |
		NV_SOR_SUPER_STATE1_ASY_ORMODE_SAFE |
		NV_SOR_SUPER_STATE1_ATTACHED_NO);

	/* Mask DC interrupts during the 2 dummy frames required for detach */
	dc_int_mask = READL(&disp_ctrl->cmd.int_mask);
	WRITEL(0, &disp_ctrl->cmd.int_mask);

	/* Stop DC->SOR path */
	tegra_dc_sor_enable_sor(sor, 0);
	tegra_dc_sor_general_act(disp_ctrl);

	/* Stop DC */
	WRITEL(DISP_CTRL_MODE_STOP, &disp_ctrl->cmd.disp_cmd);
	tegra_dc_sor_general_act(disp_ctrl);

	tegra_dc_sor_restore_win_and_raster(disp_ctrl, dc_reg_ctx);

	WRITEL(dc_int_mask, &disp_ctrl->cmd.int_mask);
}
