/*
 * Copyright (c) 2013-2015, NVIDIA CORPORATION.  All rights reserved.
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <stdlib.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/clst_clk.h>
#include <soc/console_uart.h>
#include <soc/flow.h>
#include <soc/maincpu.h>
#include <soc/pmc.h>
#include <soc/sysctr.h>

static struct flow_ctlr *flow = (void *)TEGRA_FLOW_BASE;
static struct tegra_pmc_regs *pmc = (void *)TEGRA_PMC_BASE;
static struct sysctr_regs *sysctr = (void *)TEGRA_SYSCTR0_BASE;

enum {
	PLLX_INDEX,
	PLLC_INDEX,
	PLLU_INDEX,
	PLLDP_INDEX,
	PLLD_INDEX,
	PLL_MAX_INDEX,
};

struct pll_reg_info {
	u32	*base_reg;
	u32	*lock_enb_reg;
	u32	lock_enb_val;
	u32	*pll_lock_reg;
	u32	pll_lock_val;
	u32	*kcp_kvco_reg;
	u32	n_shift:5;	/* n bits location */
	u32	m_shift:5;	/* m bits location */
	u32	p_shift:5;	/* p bits location */
	u32	kcp_shift:5;	/* kcp bits location */
	u32	kvco_shift:5;	/* kvco bit location */
	u32	rsvd:7;
} static const pll_reg_table[] = {
	[PLLX_INDEX] = { .base_reg = CLK_RST_REG(pllx_base),
			 .lock_enb_reg = CLK_RST_REG(pllx_misc),
			 .lock_enb_val = PLLPAXS_MISC_LOCK_ENABLE,
			 .pll_lock_reg = CLK_RST_REG(pllx_base),
			 .pll_lock_val = PLL_BASE_LOCK,
			 .kcp_kvco_reg = CLK_RST_REG(pllx_misc3),
			 .n_shift = 8, .m_shift = 0, .p_shift = 20,
			 .kcp_shift = 1, .kvco_shift = 0, },
	[PLLC_INDEX] = { .base_reg = CLK_RST_REG(pllc_base),
			 .lock_enb_reg = CLK_RST_REG(pllc_misc),
			 .pll_lock_reg = CLK_RST_REG(pllc_base),
			 .pll_lock_val = PLL_BASE_LOCK,
			 .n_shift = 10, .m_shift = 0, .p_shift = 20, },
	[PLLU_INDEX] = { .base_reg = CLK_RST_REG(pllu_base),
			 .lock_enb_reg = CLK_RST_REG(pllu_misc),
			 .lock_enb_val = PLLU_MISC_LOCK_ENABLE,
			 .pll_lock_reg = CLK_RST_REG(pllu_base),
			 .pll_lock_val = PLL_BASE_LOCK,
			 .kcp_kvco_reg = CLK_RST_REG(pllu_misc),
			 .n_shift = 8, .m_shift = 0, .p_shift = 16,
			 .kcp_shift = 25, .kvco_shift = 24, },
	[PLLDP_INDEX] = { .base_reg = CLK_RST_REG(plldp_base),
			  .lock_enb_reg = CLK_RST_REG(plldp_misc),
			  .lock_enb_val = PLLDPD2_MISC_LOCK_ENABLE,
			  .pll_lock_reg = CLK_RST_REG(plldp_base),
			  .pll_lock_val = PLL_BASE_LOCK,
			  .kcp_kvco_reg = CLK_RST_REG(plldp_misc),
			  .n_shift = 8, .m_shift = 0, .p_shift = 19,
			  .kcp_shift = 25, .kvco_shift = 24, },
	[PLLD_INDEX] = { .base_reg = CLK_RST_REG(plld_base),
			 .lock_enb_reg = CLK_RST_REG(plld_misc),
			 .lock_enb_val = PLLD_MISC_LOCK_ENABLE | PLLD_MISC_CLK_ENABLE,
			 .pll_lock_reg = CLK_RST_REG(plld_base),
			 .pll_lock_val = PLL_BASE_LOCK,
			 .kcp_kvco_reg = CLK_RST_REG(plld_misc),
			 .n_shift = 11, .m_shift = 0, .p_shift = 20,
			 .kcp_shift = 23, .kvco_shift = 22, },
};

struct pll_fields {
	u32	n:8;		/* the feedback divider bits width */
	u32	m:8;		/* the input divider bits width */
	u32	p:5;		/* the post divider bits witch */
	u32	kcp:2;		/* charge pump gain control */
	u32	kvco:1;	/* vco gain */
	u32	rsvd:8;
};

#define PLL_HAS_KCP_KVCO(_n, _m, _p, _kcp, _kvco)	\
	{.n = _n, .m = _m, .p = _p, .kcp = _kcp, .kvco = _kvco,}
#define PLL_NO_KCP_KVCO(_n, _m, _p)			\
	{.n = _n, .m = _m, .p = _p,}

#define PLLX(_n, _m, _p, _kcp, _kvco)					\
	[PLLX_INDEX] = PLL_HAS_KCP_KVCO(_n, _m, _p, _kcp, _kvco)
#define PLLC(_n, _m, _p)						\
	[PLLC_INDEX] = PLL_NO_KCP_KVCO(_n, _m, _p)
#define PLLU(_n, _m, _p, _kcp, _kvco)					\
	[PLLU_INDEX] = PLL_HAS_KCP_KVCO(_n, _m, _p, _kcp, _kvco)
#define PLLDP(_n, _m, _p, _kcp, _kvco)					\
	[PLLDP_INDEX] = PLL_HAS_KCP_KVCO(_n, _m, _p, _kcp, _kvco)
#define PLLD(_n, _m, _p, _kcp, _kvco)					\
	[PLLD_INDEX] = PLL_HAS_KCP_KVCO(_n, _m, _p, _kcp, _kvco)

/* This table defines the frequency dividers for every PLL to turn the external
 * OSC clock into the frequencies defined by TEGRA_PLL*_KHZ in soc/clock.h.
 * All PLLs have three dividers (n, m and p), with the governing formula for
 * the output frequency being CF = (IN / m), VCO = CF * n and OUT = VCO / (2^p).
 * All divisor configurations must meet the PLL's constraints for VCO and CF:
 * PLLX:  12 MHz < CF < 50 MHz, 700 MHz < VCO < 3000 MHz
 * PLLC:  12 MHz < CF   < 50 MHz, 600 MHz < VCO < 1400 MHz
 * PLLM:  12 MHz < CF < 50 MHz, 400 MHz < VCO < 1066 MHz
 * PLLP:   1 MHz < CF <  6 MHz, 200 MHz < VCO <  700 MHz
 * PLLD:   1 MHz < CF <  6 MHz, 500 MHz < VCO < 1000 MHz
 * PLLU:   1 MHz < CF <  6 MHz, 480 MHz < VCO <  960 MHz
 * PLLDP: 12 MHz < CF < 38 MHz, 600 MHz < VCO < 1200 MHz
 * (values taken from Linux' drivers/clk/tegra/clk-tegra124.c).
 * Target Frequencies:
 * PLLX = CONFIG_PLLX_KHZ
 * PLLC = 600 MHz
 * PLLU = 240 MHz (As per TRM, m and n should be programmed to generate 480MHz
 * VCO, and p should be programmed to do div-by-2.)
 * PLLDP = 270 MHz (PLLDP treats p differently (OUT = VCO / (p + 1) for p < 6)).
 * PLLM is set up dynamically by clock_sdram().
 * PLLP is hardwired to 408 MHz in HW (unless we set BASE_OVRD).
 */
struct {
	int			khz;
	struct pll_fields	plls[PLL_MAX_INDEX];
} static osc_table[16] = {
	[OSC_FREQ_12] = {
		.khz = 12000,
		.plls = {
			PLLX(TEGRA_PLLX_KHZ / 12000, 1, 0, 0, 0),
			PLLC(50, 1, 0), /* 600 MHz */
			PLLU(40, 1, 1, 0, 0), /* 240 MHz */
			PLLDP(90, 1, 2, 0, 0), /* 270 MHz */
		},
	},
	[OSC_FREQ_13] = {
		.khz = 13000,
		.plls = {
			PLLX(TEGRA_PLLX_KHZ / 13000, 1, 0, 0, 0),
			PLLC(46, 1, 0), /* 598.0 MHz */
			PLLU(74, 2, 1, 0, 0), /* 240.5 MHz */
			PLLDP(83, 1, 3, 0, 0), /* 269.8 MHz */
		},
	},
	[OSC_FREQ_16P8] = {
		.khz = 16800,
		.plls = {
			PLLX(TEGRA_PLLX_KHZ / 16800, 1, 0, 0, 0),
			PLLC(71, 1, 1), /* 596.4 MHz */
			PLLU(115, 4, 1, 0, 0), /* 241.5 MHz */
			PLLDP(64, 1, 2, 0, 0), /* 268.8 MHz */
		},
	},
	[OSC_FREQ_19P2] = {
		.khz = 19200,
		.plls = {
			PLLX(TEGRA_PLLX_KHZ / 19200, 1, 0, 0, 0),
			PLLC(62, 1, 1), /* 595.2 MHz */
			PLLU(25, 1, 1, 0, 0), /* 240.0 MHz */
			PLLDP(56, 1, 2, 0, 0), /* 268.8 MHz */
		},
	},
	[OSC_FREQ_26] = {
		.khz = 26000,
		.plls = {
			PLLX(TEGRA_PLLX_KHZ / 26000, 1, 0, 0, 0),
			PLLC(23, 1, 0), /* 598.0 MHz */
			PLLU(37, 2, 1, 0, 0), /* 240.5 MHz */
			PLLDP(83, 2, 2, 0, 0), /* 269.8 MHz */
		},
	},
	[OSC_FREQ_38P4] = {
		.khz = 38400,
		.plls = {
			PLLX(TEGRA_PLLX_KHZ / 38400, 1, 0, 0, 0),
			PLLC(62, 2, 1), /* 595.2 MHz */
			PLLU(25, 2, 1, 0, 0), /* 240 MHz */
			PLLDP(56, 2, 2, 0, 0), /* 268.8 MHz */
		},
	},
	[OSC_FREQ_48] = {
		.khz = 48000,
		.plls = {
			PLLX(TEGRA_PLLX_KHZ / 48000, 1, 0, 0, 0),
			PLLC(50, 2, 1), /* 600 MHz */
			PLLU(40, 4, 1, 0, 0), /* 240 MHz */
			PLLDP(90, 2, 3, 0, 0), /* 270 MHz */
		},
	},
};

/* Get the oscillator frequency, from the corresponding hardware
 * configuration field. This is actually a per-soc thing. Avoid the
 * temptation to make it common.
 */
static u32 clock_get_osc_bits(void)
{
	return (read32(CLK_RST_REG(osc_ctrl)) & OSC_FREQ_MASK) >> OSC_FREQ_SHIFT;
}

int clock_get_osc_khz(void)
{
	return osc_table[clock_get_osc_bits()].khz;
}

int clock_get_pll_input_khz(void)
{
	u32 osc_ctrl = read32(CLK_RST_REG(osc_ctrl));
	u32 osc_bits = (osc_ctrl & OSC_FREQ_MASK) >> OSC_FREQ_SHIFT;
	u32 pll_ref_div = (osc_ctrl & OSC_PREDIV_MASK) >> OSC_PREDIV_SHIFT;
	return osc_table[osc_bits].khz >> pll_ref_div;
}

void clock_init_arm_generic_timer(void)
{
	uint32_t freq = TEGRA_CLK_M_KHZ * 1000;

	// Record the system timer frequency.
	write32(&sysctr->cntfid0, freq);
	// Enable the system counter.
	uint32_t cntcr = read32(&sysctr->cntcr);
	cntcr |= SYSCTR_CNTCR_EN | SYSCTR_CNTCR_HDBG;
	write32(&sysctr->cntcr, cntcr);
}

#define SOR0_CLK_SEL0			(1 << 14)
#define SOR0_CLK_SEL1			(1 << 15)

void sor_clock_stop(void)
{
	/* The Serial Output Resource clock has to be off
	 * before we start the plldp. Learned the hard way.
	 * FIXME: this has to be cleaned up a bit more.
	 * Waiting on some new info from Nvidia.
	 */
	clrbits_le32(CLK_RST_REG(clk_src_sor), SOR0_CLK_SEL0 | SOR0_CLK_SEL1);
}

void sor_clock_start(void)
{
	/* uses PLLP, has a non-standard bit layout. */
	setbits_le32(CLK_RST_REG(clk_src_sor), SOR0_CLK_SEL0);
}

static void init_pll(u32 index, u32 osc)
{
	assert(index <= PLL_MAX_INDEX);

	struct pll_fields *pll = &osc_table[osc].plls[index];
	const struct pll_reg_info *pll_reg = &pll_reg_table[index];

	u32 dividers =  pll->n << pll_reg->n_shift |
			pll->m << pll_reg->m_shift |
			pll->p << pll_reg->p_shift;

	/* Write dividers but BYPASS the PLL while we're messing with it. */
	write32(pll_reg->base_reg, dividers | PLL_BASE_BYPASS);

	/* Set Lock bit if needed. */
	if (pll_reg->lock_enb_val)
		setbits_le32(pll_reg->lock_enb_reg, pll_reg->lock_enb_val);

	/* Set KCP/KVCO if needed. */
	if (pll_reg->kcp_kvco_reg)
		setbits_le32(pll_reg->kcp_kvco_reg,
			     pll->kcp << pll_reg->kcp_shift |
			     pll->kvco << pll_reg->kvco_shift);

	/* Enable PLL and take it back out of BYPASS */
	write32(pll_reg->base_reg, dividers | PLL_BASE_ENABLE);

	/* Wait for lock ready */
	if (pll_reg->lock_enb_val)
		while (!(read32(pll_reg->pll_lock_reg) & pll_reg->pll_lock_val))
			;
}

static void init_pllc(u32 osc)
{
	/* Clear PLLC reset */
	clrbits_le32(CLK_RST_REG(pllc_misc), PLLC_MISC_RESET);

	/* Clear PLLC IDDQ */
	clrbits_le32(CLK_RST_REG(pllc_misc_1), PLLC_MISC_1_IDDQ);

	/* Max out the AVP clock before everything else (need PLLC for that). */
	init_pll(PLLC_INDEX, osc);

	/* wait for pllc_lock (not the normal bit 27) */
	while (!(read32(CLK_RST_REG(pllc_base)) & PLLC_BASE_LOCK))
		;
}

static void init_pllu(u32 osc)
{
	/* Clear PLLU IDDQ */
	clrbits_le32(CLK_RST_REG(pllu_misc), PLLU_MISC_IDDQ);

	/* Wait 5 us */
	udelay(5);

	init_pll(PLLU_INDEX, osc);
}

static void init_utmip_pll(void)
{
	int khz = clock_get_pll_input_khz();

	/* CFG1 */
	u32 pllu_enb_ct = 0;
	u32 phy_stb_ct = div_round_up(khz, 300);  /* phy_stb_ct = 128 */
	write32(CLK_RST_REG(utmip_pll_cfg1),
		pllu_enb_ct << UTMIP_CFG1_PLLU_ENABLE_DLY_COUNT_SHIFT |
		UTMIP_CFG1_FORCE_PLLU_POWERDOWN_ENABLE |
		UTMIP_CFG1_FORCE_PLL_ENABLE_POWERDOWN_DISABLE |
		UTMIP_CFG1_FORCE_PLL_ACTIVE_POWERDOWN_DISABLE |
		UTMIP_CFG1_FORCE_PLL_ENABLE_POWERUP_ENABLE |
		phy_stb_ct << UTMIP_CFG1_XTAL_FREQ_COUNT_SHIFT);

	/* CFG2 */
	u32 pllu_stb_ct = 0;
	u32 phy_act_ct = div_round_up(khz, 6400); /* phy_act_ct = 6 */
	write32(CLK_RST_REG(utmip_pll_cfg2),
		phy_act_ct << UTMIP_CFG2_PLL_ACTIVE_DLY_COUNT_SHIFT |
		pllu_stb_ct << UTMIP_CFG2_PLLU_STABLE_COUNT_SHIFT |
		UTMIP_CFG2_FORCE_PD_SAMP_D_POWERDOWN_DISABLE |
		UTMIP_CFG2_FORCE_PD_SAMP_C_POWERDOWN_DISABLE |
		UTMIP_CFG2_FORCE_PD_SAMP_B_POWERDOWN_DISABLE |
		UTMIP_CFG2_FORCE_PD_SAMP_A_POWERDOWN_DISABLE |
		UTMIP_CFG2_FORCE_PD_SAMP_D_POWERUP_ENABLE |
		UTMIP_CFG2_FORCE_PD_SAMP_C_POWERUP_ENABLE |
		UTMIP_CFG2_FORCE_PD_SAMP_B_POWERUP_ENABLE |
		UTMIP_CFG2_FORCE_PD_SAMP_A_POWERUP_ENABLE);

	printk(BIOS_DEBUG, "%s: UTMIPLL_HW_PWRDN_CFG0:0x%08x\n",
		__func__, read32(CLK_RST_REG(utmipll_hw_pwrdn_cfg0)));
	printk(BIOS_DEBUG, "%s: UTMIP_PLL_CFG0:0x%08x\n",
		__func__, read32(CLK_RST_REG(utmip_pll_cfg0)));
	printk(BIOS_DEBUG, "%s: UTMIP_PLL_CFG1:0x%08x\n",
		__func__, read32(CLK_RST_REG(utmip_pll_cfg1)));
	printk(BIOS_DEBUG, "%s: UTMIP_PLL_CFG2:0x%08x\n",
		__func__, read32(CLK_RST_REG(utmip_pll_cfg2)));
}

/* Graphics just has to be different. There's a few more bits we
 * need to set in here, but it makes sense just to restrict all the
 * special bits to this one function.
 */
static void graphics_pll(void)
{
	int osc = clock_get_osc_bits();
	u32 *cfg = CLK_RST_REG(plldp_ss_cfg);
	/* the vendor code sets the dither bit (28)
	 * an undocumented bit (24)
	 * and clamp while we mess with it (22)
	 * Dither is pretty important to display port
	 * so we really do need to handle these bits.
	 * I'm not willing to not clamp it, even if
	 * it might "mostly work" with it not set,
	 * I don't want to find out in a few months
	 * that it is needed.
	 */
	u32 scfg = (1<<28) | (1<<24) | (1<<22);
	write32(cfg, scfg);
	init_pll(PLLDP_INDEX, osc);
	/* leave dither and undoc bits set, release clamp */
	scfg = (1<<28) | (1<<24);
	write32(cfg, scfg);
}

/*
 * Init PLLD clock source.
 *
 * @frequency: the requested plld frequency
 *
 * Return the plld frequency if success, otherwise return 0.
 */
u32 clock_configure_plld(u32 frequency)
{
	/**
	 * plld (fo) = vco >> p, where 500MHz < vco < 1000MHz
	 *           = (cf * n) >> p, where 1MHz < cf < 6MHz
	 *           = ((ref / m) * n) >> p
	 *
	 * Iterate the possible values of p (3 bits, 2^7) to find out a minimum
	 * safe vco, then find best (m, n). since m has only 5 bits, we can
	 * iterate all possible values.  Note Tegra1xx supports 11 bits for n,
	 * but our pll_fields has only 10 bits for n.
	 *
	 * Note values undershoot or overshoot target output frequency may not
	 * work if the values are not in "safe" range by panel specification.
	 */
	struct pll_fields *plld;
	u32 ref = clock_get_pll_input_khz() * 1000, m, n, p = 0;
	u32 cf, vco, rounded_rate = frequency;
	u32 diff, best_diff;
	const u32 max_m = 1 << 8, max_n = 1 << 8, max_p = 1 << 3,
		  mhz = 1000 * 1000, min_vco = 500 * mhz, max_vco = 1000 * mhz,
		  min_cf = 1 * mhz, max_cf = 6 * mhz;
	u32 osc = clock_get_osc_bits();

	plld = &osc_table[osc].plls[PLLD_INDEX];

	for (vco = frequency; vco < min_vco && p < max_p; p++)
		vco <<= 1;

	if (vco < min_vco || vco > max_vco) {
		printk(BIOS_ERR, "%s: Cannot find out a supported VCO"
			" for Frequency (%u).\n", __func__, frequency);
		return 0;
	}

	plld->p = p;
	best_diff = vco;

	for (m = 1; m < max_m && best_diff; m++) {
		cf = ref / m;
		if (cf < min_cf)
			break;
		if (cf > max_cf)
			continue;

		n = vco / cf;
		if (n >= max_n)
			continue;

		diff = vco - n * cf;
		if (n + 1 < max_n && diff > cf / 2) {
			n++;
			diff = cf - diff;
		}

		if (diff >= best_diff)
			continue;

		best_diff = diff;
		plld->m = m;
		plld->n = n;
	}

	if (best_diff) {
		printk(BIOS_WARNING, "%s: Failed to match output frequency %u, "
		       "best difference is %u.\n", __func__, frequency,
		       best_diff);
		rounded_rate = (ref / plld->m * plld->n) >> plld->p;
	}

	printk(BIOS_DEBUG, "%s: PLLD=%u ref=%u, m/n/p=%u/%u/%u\n",
	       __func__, rounded_rate, ref, plld->m, plld->n, plld->p);

	/* Write misc1 and misc */
	write32(CLK_RST_REG(plld_misc1), PLLD_MISC1_SETUP);
	write32(CLK_RST_REG(plld_misc), (PLLD_MISC_EN_SDM | PLLD_MISC_SDM_DIN));

	/* configure PLLD */
	init_pll(PLLD_INDEX, osc);

	if (rounded_rate != frequency)
		printk(BIOS_DEBUG, "PLLD rate: %u vs %u\n", rounded_rate,
			frequency);

	return rounded_rate;
}

/*
 * Initialize the UART and use PLLP as clock source. PLLP is hardwired to 408
 * MHz in HW (unless we set BASE_OVRD). We override the 16.0 UART divider with
 * the 15.1 CLK_SOURCE divider to get more precision. The 1843(KHZ) is
 * calculated thru BAUD_RATE*16/1000, ie, 115200*16/1000.
 */
void clock_early_uart(void)
{
	if (console_uart_get_id() == UART_ID_NONE)
		return;

	write32(console_uart_clk_rst_reg(),
		console_uart_clk_src_dev_id() << CLK_SOURCE_SHIFT |
		CLK_UART_DIV_OVERRIDE |
		CLK_DIVIDER(TEGRA_PLLP_KHZ, 1843));

	console_uart_clock_enable_clear_reset();
}

/* Enable output clock (CLK1~3) for external peripherals. */
void clock_external_output(int clk_id)
{
	switch (clk_id) {
	case 1:
		setbits_le32(&pmc->clk_out_cntrl, 1 << 2);
		break;
	case 2:
		setbits_le32(&pmc->clk_out_cntrl, 1 << 10);
		break;
	case 3:
		setbits_le32(&pmc->clk_out_cntrl, 1 << 18);
		break;
	default:
		printk(BIOS_CRIT, "ERROR: Unknown output clock id %d\n",
		       clk_id);
		break;
	}
}

/* Start PLLM for SDRAM. */
void clock_sdram(u32 m, u32 n, u32 p, u32 setup, u32 kvco, u32 kcp,
		 u32 stable_time, u32 emc_source, u32 same_freq)
{
	u32 misc1 = ((setup << PLLM_MISC1_SETUP_SHIFT)),
	    misc2 = ((kvco << PLLM_MISC2_KVCO_SHIFT) |
		     (kcp << PLLM_MISC2_KCP_SHIFT) |
		     PLLM_EN_LCKDET),
	    base;

	if (same_freq)
		emc_source |= CLK_SOURCE_EMC_MC_EMC_SAME_FREQ;
	else
		emc_source &= ~CLK_SOURCE_EMC_MC_EMC_SAME_FREQ;

	/*
	 * Note PLLM_BASE.PLLM_OUT1_RSTN must be in RESET_ENABLE mode, and
	 * PLLM_BASE.ENABLE must be in DISABLE state (both are the default
	 * values after coldboot reset).
	 */

	write32(CLK_RST_REG(pllm_misc1), misc1);
	write32(CLK_RST_REG(pllm_misc2), misc2);

	/* PLLM.BASE needs BYPASS=0, different from general init_pll */
	base = read32(CLK_RST_REG(pllm_base));
	base &= ~(PLLCMX_BASE_DIVN_MASK | PLLCMX_BASE_DIVM_MASK |
		  PLLM_BASE_DIVP_MASK | PLL_BASE_BYPASS);
	base |= ((m << PLL_BASE_DIVM_SHIFT) | (n << PLL_BASE_DIVN_SHIFT) |
		 (p << PLL_BASE_DIVP_SHIFT));
	write32(CLK_RST_REG(pllm_base), base);

	setbits_le32(CLK_RST_REG(pllm_base), PLL_BASE_ENABLE);
	/* stable_time is required, before we can start to check lock. */
	udelay(stable_time);

	while (!(read32(CLK_RST_REG(pllm_base)) & PLL_BASE_LOCK))
		udelay(1);

	/*
	 * After PLLM reports being locked, we have to delay 10us before
	 * enabling PLLM_OUT.
	 */
	udelay(10);

	/* Enable and start MEM(MC) and EMC. */
	clock_enable_clear_reset(0, CLK_H_MEM | CLK_H_EMC, 0, 0, 0, 0, 0);
	write32(CLK_RST_REG(clk_src_emc), emc_source);
	udelay(IO_STABILIZATION_DELAY);
}

void clock_halt_avp(void)
{
	for (;;) {
		write32(&flow->halt_cop_events,
			FLOW_EVENT_JTAG | FLOW_EVENT_LIC_IRQ |
			FLOW_EVENT_GIC_IRQ | FLOW_MODE_WAITEVENT);
	}
}

void clock_init(void)
{
	u32 osc = clock_get_osc_bits();
	/* clk_m = osc/2 */
	clrsetbits_le32(CLK_RST_REG(spare_reg0), CLK_M_DIVISOR_MASK,
			CLK_M_DIVISOR_BY_2);

	/* TIMERUS needs to be adjusted for new 19.2MHz CLK_M rate */
	write32((void *)TEGRA_TMRUS_BASE + TIMERUS_USEC_CFG,
		TIMERUS_USEC_CFG_19P2_CLK_M);

	init_pllc(osc);

	/* Typical ratios are 1:2:2 or 1:2:3 sclk:hclk:pclk (See: APB DMA
	 * features section in the TRM). */
	write32(CLK_RST_REG(clk_sys_rate),	/* pclk = hclk = sclk/2 */
		1 << HCLK_DIVISOR_SHIFT | 0 << PCLK_DIVISOR_SHIFT);
	write32(CLK_RST_REG(pllc_out),
		CLK_DIVIDER(TEGRA_PLLC_KHZ, 300000) << PLL_OUT_RATIO_SHIFT |
		PLL_OUT_CLKEN | PLL_OUT_RSTN);
	write32(CLK_RST_REG(sclk_brst_pol),		/* sclk = 300 MHz */
		SCLK_SYS_STATE_RUN << SCLK_SYS_STATE_SHIFT |
		SCLK_SOURCE_PLLC_OUT1 << SCLK_RUN_SHIFT);

	/* Change the oscillator drive strength (from U-Boot -- why?) */
	clrsetbits_le32(CLK_RST_REG(osc_ctrl), OSC_XOFS_MASK,
			OSC_DRIVE_STRENGTH << OSC_XOFS_SHIFT);

	/*
	 * Ambiguous quote from u-boot. TODO: what's this mean?
	 * "should update same value in PMC_OSC_EDPD_OVER XOFS
	 * field for warmboot "
	 */
	clrsetbits_le32(&pmc->osc_edpd_over, PMC_OSC_EDPD_OVER_XOFS_MASK,
			OSC_DRIVE_STRENGTH << PMC_OSC_EDPD_OVER_XOFS_SHIFT);

	/* Disable IDDQ for PLLX before we set it up (from U-Boot -- why?) */
	clrbits_le32(CLK_RST_REG(pllx_misc3), PLLX_IDDQ_MASK);

	/* Set up PLLP_OUT(1|2|3|4) divisor to generate (9.6|48|102|204)MHz */
	write32(CLK_RST_REG(pllp_outa),
		(CLK_DIVIDER(TEGRA_PLLP_KHZ, 9600) << PLL_OUT_RATIO_SHIFT |
		PLL_OUT_OVR | PLL_OUT_CLKEN | PLL_OUT_RSTN) << PLL_OUT1_SHIFT |
		(CLK_DIVIDER(TEGRA_PLLP_KHZ, 48000) << PLL_OUT_RATIO_SHIFT |
		PLL_OUT_OVR | PLL_OUT_CLKEN | PLL_OUT_RSTN) << PLL_OUT2_SHIFT);
	write32(CLK_RST_REG(pllp_outb),
		(CLK_DIVIDER(TEGRA_PLLP_KHZ, TEGRA_PLLP_OUT3_KHZ) <<
			PLL_OUT_RATIO_SHIFT |
		PLL_OUT_OVR | PLL_OUT_CLKEN | PLL_OUT_RSTN) << PLL_OUT3_SHIFT |
		(CLK_DIVIDER(TEGRA_PLLP_KHZ, 204000) << PLL_OUT_RATIO_SHIFT |
		PLL_OUT_OVR | PLL_OUT_CLKEN | PLL_OUT_RSTN) << PLL_OUT4_SHIFT);

	/* init pllx */
	init_pll(PLLX_INDEX, osc);
	write32(CLK_RST_REG(cclk_brst_pol), CCLK_BURST_POLICY_VAL);

	/* init pllu */
	init_pllu(osc);

	init_utmip_pll();
	graphics_pll();
}

void clock_grp_enable_clear_reset(u32 val, u32* clk_enb_set_reg,
				  u32 *rst_dev_clr_reg)
{
	write32(clk_enb_set_reg, val);
	udelay(IO_STABILIZATION_DELAY);
	write32(rst_dev_clr_reg, val);
}

static u32 * const clk_enb_set_arr[DEV_CONFIG_BLOCKS] = {
	CLK_RST_REG(clk_enb_l_set),
	CLK_RST_REG(clk_enb_h_set),
	CLK_RST_REG(clk_enb_u_set),
	CLK_RST_REG(clk_enb_v_set),
	CLK_RST_REG(clk_enb_w_set),
	CLK_RST_REG(clk_enb_x_set),
	CLK_RST_REG(clk_enb_y_set),
};

static u32 * const clk_enb_clr_arr[DEV_CONFIG_BLOCKS] = {
	CLK_RST_REG(clk_enb_l_clr),
	CLK_RST_REG(clk_enb_h_clr),
	CLK_RST_REG(clk_enb_u_clr),
	CLK_RST_REG(clk_enb_v_clr),
	CLK_RST_REG(clk_enb_w_clr),
	CLK_RST_REG(clk_enb_x_clr),
	CLK_RST_REG(clk_enb_y_clr),
};

static u32 * const rst_dev_set_arr[DEV_CONFIG_BLOCKS] = {
	CLK_RST_REG(rst_dev_l_set),
	CLK_RST_REG(rst_dev_h_set),
	CLK_RST_REG(rst_dev_u_set),
	CLK_RST_REG(rst_dev_v_set),
	CLK_RST_REG(rst_dev_w_set),
	CLK_RST_REG(rst_dev_x_set),
	CLK_RST_REG(rst_dev_y_set),
};

static u32 * const rst_dev_clr_arr[DEV_CONFIG_BLOCKS] = {
	CLK_RST_REG(rst_dev_l_clr),
	CLK_RST_REG(rst_dev_h_clr),
	CLK_RST_REG(rst_dev_u_clr),
	CLK_RST_REG(rst_dev_v_clr),
	CLK_RST_REG(rst_dev_w_clr),
	CLK_RST_REG(rst_dev_x_clr),
	CLK_RST_REG(rst_dev_y_clr),
};

static void clock_write_regs(u32 * const regs[DEV_CONFIG_BLOCKS],
			     u32 bits[DEV_CONFIG_BLOCKS])
{
	int i = 0;

	for (; i < DEV_CONFIG_BLOCKS; i++)
		if (bits[i])
			write32(regs[i], bits[i]);
}

void clock_enable_regs(u32 bits[DEV_CONFIG_BLOCKS])
{
	clock_write_regs(clk_enb_set_arr, bits);
}

void clock_disable_regs(u32 bits[DEV_CONFIG_BLOCKS])
{
	clock_write_regs(clk_enb_clr_arr, bits);
}

void clock_set_reset_regs(u32 bits[DEV_CONFIG_BLOCKS])
{
	clock_write_regs(rst_dev_set_arr, bits);
}

void clock_clr_reset_regs(u32 bits[DEV_CONFIG_BLOCKS])
{
	clock_write_regs(rst_dev_clr_arr, bits);
}

void clock_enable_clear_reset(u32 l, u32 h, u32 u, u32 v, u32 w, u32 x, u32 y)
{
	clock_enable(l, h, u, v, w, x, y);

	/* Give clocks time to stabilize. */
	udelay(IO_STABILIZATION_DELAY);

	clock_clr_reset(l, h, u, v, w, x, y);
}

static void clock_reset_dev(u32 *setaddr, u32 *clraddr, u32 bit)
{
	write32(setaddr, bit);
	udelay(LOGIC_STABILIZATION_DELAY);
	write32(clraddr, bit);
}

void clock_reset_l(u32 bit)
{
	clock_reset_dev(CLK_RST_REG(rst_dev_l_set), CLK_RST_REG(rst_dev_l_clr),
			bit);
}

void clock_reset_h(u32 bit)
{
	clock_reset_dev(CLK_RST_REG(rst_dev_h_set), CLK_RST_REG(rst_dev_h_clr),
			bit);
}

void clock_reset_u(u32 bit)
{
	clock_reset_dev(CLK_RST_REG(rst_dev_u_set), CLK_RST_REG(rst_dev_u_clr),
			bit);
}

void clock_reset_v(u32 bit)
{
	clock_reset_dev(CLK_RST_REG(rst_dev_v_set), CLK_RST_REG(rst_dev_v_clr),
			bit);
}

void clock_reset_w(u32 bit)
{
	clock_reset_dev(CLK_RST_REG(rst_dev_w_set), CLK_RST_REG(rst_dev_w_clr),
			bit);
}

void clock_reset_x(u32 bit)
{
	clock_reset_dev(CLK_RST_REG(rst_dev_x_set), CLK_RST_REG(rst_dev_x_clr),
			bit);
}

void clock_reset_y(u32 bit)
{
	clock_reset_dev(CLK_RST_REG(rst_dev_y_set), CLK_RST_REG(rst_dev_y_clr),
			bit);
}

/* Enable/unreset all audio toys under AHUB */
void clock_enable_audio(void)
{
	/*
	 * As per NVIDIA hardware team, we need to take ALL audio devices
	 * connected to AHUB (AHUB, APB2APE, I2S, SPDIF, etc.) out of reset
	 * and clock-enabled, otherwise reading AHUB devices (in our case,
	 * I2S/APBIF/AUDIO<XBAR>) will hang.
	 */
	clock_enable_clear_reset(CLK_L_I2S1 | CLK_L_I2S2 | CLK_L_I2S3 | CLK_L_SPDIF,
				 0, 0,
				 CLK_V_I2S4 | CLK_V_I2S5 | CLK_V_AHUB | CLK_V_APB2APE |
				 CLK_V_EXTPERIPH1,
				 0, 0, 0);
}
