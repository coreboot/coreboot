/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <timer.h>
#include <types.h>

#define DIV(div) (2 * div - 1)

struct clock_config qup_cfg[] = {
	{
		.hz = QUPV3_UART_SRC_HZ,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(1),
		.m = 384,
		.n = 15625,
		.d_2 = 15625,
	},
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = DIV(1),
	}
};

struct clock_config qspi_core_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = DIV(1),
	},
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(3),
	},
	{
		.hz = 150 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(2),
	},
	{
		.hz = GPLL0_EVEN_HZ,	/* 300MHz */
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(1),
	}
};

struct clock_config qup_wrap_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = DIV(1),
	},
	{
		.hz =  32 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(1),
		.m = 8,
		.n = 75,
		.d_2 = 75,
	},
	{
		.hz =  48 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(1),
		.m = 4,
		.n = 25,
		.d_2 = 25,
	},
	{
		.hz =  64 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(1),
		.m = 16,
		.n = 75,
		.d_2 = 75,
	},
	{
		.hz =  96 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(1),
		.m = 8,
		.n = 25,
		.d_2 = 25,
	},
	{
		.hz =  100 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(3),
	},
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = DIV(1),
	},
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = DIV(1),
	},
};

static struct sc7180_mnd_clock *mdss_clock[MDSS_CLK_COUNT] = {
	[MDSS_CLK_ESC0] = &mdss->esc0,
	[MDSS_CLK_PCLK0] = &mdss->pclk0,
	[MDSS_CLK_BYTE0] = &mdss->byte0,
	[MDSS_CLK_BYTE0_INTF] = &mdss->byte0,
};

static u32 *mdss_cbcr[MDSS_CLK_COUNT] = {
	[MDSS_CLK_ESC0] = &mdss->esc0_cbcr,
	[MDSS_CLK_PCLK0] = &mdss->pclk0_cbcr,
	[MDSS_CLK_BYTE0] = &mdss->byte0_cbcr,
	[MDSS_CLK_BYTE0_INTF] = &mdss->byte0_intf_cbcr,
};

static int clock_configure_gpll0(void)
{
	setbits32(&gcc->gpll0.user_ctl_u, 1 << SCALE_FREQ_SHFT);

	/* Keep existing GPLL0 configuration, in RUN mode @600Mhz. */
	setbits32(&gcc->gpll0.user_ctl,
			1 << CLK_CTL_GPLL_PLLOUT_EVEN_SHFT |
			1 << CLK_CTL_GPLL_PLLOUT_MAIN_SHFT |
			1 << CLK_CTL_GPLL_PLLOUT_ODD_SHFT);

	return 0;
}

static int clock_configure_mnd(struct sc7180_clock *clk, uint32_t m, uint32_t n,
				uint32_t d_2)
{
	struct sc7180_mnd_clock *mnd = (struct sc7180_mnd_clock *)clk;
	setbits32(&clk->rcg_cfg,
			RCG_MODE_DUAL_EDGE << CLK_CTL_CFG_MODE_SHFT);

	write32(&mnd->m, m & CLK_CTL_RCG_MND_BMSK);
	write32(&mnd->n, ~(n-m) & CLK_CTL_RCG_MND_BMSK);
	write32(&mnd->d_2, ~(d_2) & CLK_CTL_RCG_MND_BMSK);

	return 0;
}

static int clock_configure(struct sc7180_clock *clk,
				struct clock_config *clk_cfg,
				uint32_t hz, uint32_t num_perfs)
{
	uint32_t reg_val;
	uint32_t idx;

	for (idx = 0; idx < num_perfs; idx++)
		if (hz <= clk_cfg[idx].hz)
			break;

	assert(hz == clk_cfg[idx].hz);

	reg_val = (clk_cfg[idx].src << CLK_CTL_CFG_SRC_SEL_SHFT) |
			(clk_cfg[idx].div << CLK_CTL_CFG_SRC_DIV_SHFT);

	/* Set clock config */
	write32(&clk->rcg_cfg, reg_val);

	if (clk_cfg[idx].m != 0)
		clock_configure_mnd(clk, clk_cfg[idx].m, clk_cfg[idx].n,
				clk_cfg[idx].d_2);

	/* Commit config to RCG*/
	setbits32(&clk->rcg_cmd, BIT(CLK_CTL_CMD_UPDATE_SHFT));

	return 0;
}

static bool clock_is_off(u32 *cbcr_addr)
{
	return (read32(cbcr_addr) & CLK_CTL_CBC_CLK_OFF_BMSK);
}

static int clock_enable_vote(void *cbcr_addr, void *vote_addr,
				uint32_t vote_bit)
{
	/* Set clock vote bit */
	setbits32(vote_addr, BIT(vote_bit));

	/* Ensure clock is enabled */
	while (clock_is_off(cbcr_addr))
		;

	return 0;
}

static int clock_enable(void *cbcr_addr)
{
	/* Set clock enable bit */
	setbits32(cbcr_addr, BIT(CLK_CTL_CBC_CLK_EN_SHFT));

	/* Ensure clock is enabled */
	while (clock_is_off(cbcr_addr))
		;

	return 0;
}

void clock_reset_aop(void)
{
	/* Bring AOP out of RESET */
	clrbits32(&aoss->aoss_cc_apcs_misc, BIT(AOP_RESET_SHFT));
}

void clock_configure_qspi(uint32_t hz)
{
	clock_configure(&gcc->qspi_core,
			qspi_core_cfg, hz,
			ARRAY_SIZE(qspi_core_cfg));
	clock_enable(&gcc->qspi_cnoc_ahb_cbcr);
	clock_enable(&gcc->qspi_core_cbcr);
}

int clock_reset_bcr(void *bcr_addr, bool reset)
{
	struct sc7180_bcr *bcr = bcr_addr;

	if (reset)
		setbits32(bcr, BIT(CLK_CTL_BCR_BLK_ARES_SHFT));
	else
		clrbits32(bcr, BIT(CLK_CTL_BCR_BLK_ARES_SHFT));

	return 0;
}

void clock_configure_dfsr(int qup)
{
	int idx;
	int s = qup % QUP_WRAP1_S0;
	uint32_t reg_val;
	struct sc7180_qupv3_clock *qup_clk = qup < QUP_WRAP1_S0 ?
				&gcc->qup_wrap0_s[s] : &gcc->qup_wrap1_s[s];

	clrsetbits32(&qup_clk->dfsr_clk.cmd_dfsr,
					BIT(CLK_CTL_CMD_RCG_SW_CTL_SHFT),
					BIT(CLK_CTL_CMD_DFSR_SHFT));

	for (idx = 0; idx < ARRAY_SIZE(qup_wrap_cfg); idx++) {
		reg_val = (qup_wrap_cfg[idx].src << CLK_CTL_CFG_SRC_SEL_SHFT) |
			(qup_wrap_cfg[idx].div << CLK_CTL_CFG_SRC_DIV_SHFT);

		write32(&qup_clk->dfsr_clk.perf_dfsr[idx], reg_val);

		if (qup_wrap_cfg[idx].m == 0)
			continue;

		setbits32(&qup_clk->dfsr_clk.perf_dfsr[idx],
				RCG_MODE_DUAL_EDGE << CLK_CTL_CFG_MODE_SHFT);

		reg_val = qup_wrap_cfg[idx].m & CLK_CTL_RCG_MND_BMSK;
		write32(&qup_clk->dfsr_clk.perf_m_dfsr[idx], reg_val);

		reg_val = ~(qup_wrap_cfg[idx].n - qup_wrap_cfg[idx].m)
				& CLK_CTL_RCG_MND_BMSK;
		write32(&qup_clk->dfsr_clk.perf_n_dfsr[idx], reg_val);

		reg_val = ~(qup_wrap_cfg[idx].d_2) & CLK_CTL_RCG_MND_BMSK;
		write32(&qup_clk->dfsr_clk.perf_d_dfsr[idx], reg_val);
	}
}

void clock_configure_qup(int qup, uint32_t hz)
{
	int s = qup % QUP_WRAP1_S0;
	struct sc7180_qupv3_clock *qup_clk = qup < QUP_WRAP1_S0 ?
				&gcc->qup_wrap0_s[s] : &gcc->qup_wrap1_s[s];

	clock_configure(&qup_clk->mnd_clk.clock, qup_cfg, hz,
							ARRAY_SIZE(qup_cfg));
}

void clock_enable_qup(int qup)
{
	int s = qup % QUP_WRAP1_S0;
	int clk_en_off = qup < QUP_WRAP1_S0 ?
			QUPV3_WRAP0_CLK_ENA_S(s) : QUPV3_WRAP1_CLK_ENA_S(s);
	struct sc7180_qupv3_clock *qup_clk = qup < QUP_WRAP1_S0 ?
				&gcc->qup_wrap0_s[s] : &gcc->qup_wrap1_s[s];

	clock_enable_vote(&qup_clk->mnd_clk, &gcc->apcs_clk_br_en1,
							clk_en_off);
}

static int pll_init_and_set(struct sc7180_apss_clock *apss, u32 l_val)
{
	u32 gfmux_val;

	/* Configure and Enable PLL */
	write32(&apss->pll.config_ctl_lo, 0x0);
	setbits32(&apss->pll.config_ctl_lo, 0x2 << CTUNE_SHFT |
		    0x2 << K_I_SHFT | 0x5 << K_P_SHFT |
		    0x2 << PFA_MSB_SHFT | 0x2 << REF_CONT_SHFT);

	write32(&apss->pll.config_ctl_hi, 0x0);
	setbits32(&apss->pll.config_ctl_hi, 0x2 << CUR_ADJ_SHFT |
		    BIT(DMET_SHFT) | 0xF << RES_SHFT);

	write32(&apss->pll.config_ctl_u1, 0x0);
	write32(&apss->pll.l_val, l_val);

	setbits32(&apss->pll.mode, BIT(BYPASSNL_SHFT));
	udelay(5);
	setbits32(&apss->pll.mode, BIT(RESET_SHFT));

	setbits32(&apss->pll.opmode, RUN_MODE);

	if (!wait_us(100, read32(&apss->pll.mode) & LOCK_DET_BMSK)) {
		printk(BIOS_ERR, "ERROR: PLL did not lock!\n");
		return -1;
	}

	setbits32(&apss->pll.mode, BIT(OUTCTRL_SHFT));

	gfmux_val = read32(&apss->cfg_gfmux) & ~GFMUX_SRC_SEL_BMSK;
	gfmux_val |= APCS_SRC_EARLY;
	write32(&apss->cfg_gfmux, gfmux_val);

	return 0;
}

static void speed_up_boot_cpu(void)
{
	/* 1516.8 MHz */
	if (!pll_init_and_set(apss_silver, L_VAL_1516P8MHz))
		printk(BIOS_DEBUG, "Silver Frequency bumped to 1.5168(GHz)\n");

	/* 1209.6 MHz */
	if (!pll_init_and_set(apss_l3, L_VAL_1209P6MHz))
		printk(BIOS_DEBUG, "L3 Frequency bumped to 1.2096(GHz)\n");
}

int mdss_clock_configure(enum mdss_clock clk_type, uint32_t source,
				uint32_t half_divider, uint32_t m,
				uint32_t n, uint32_t d_2)
{
	struct clock_config mdss_clk_cfg;
	uint32_t reg_val;

	if (clk_type >= MDSS_CLK_COUNT)
		return -1;

	/* Initialize it with received arguments */
	mdss_clk_cfg.hz = 0;
	mdss_clk_cfg.src = source;

	/*
	 * client is expected to provide 2n divider value,
	 * as the divider value in register is in form "2n-1"
	 */
	mdss_clk_cfg.div = half_divider ? (half_divider - 1) : 0;
	mdss_clk_cfg.m = m;
	mdss_clk_cfg.n = n;
	mdss_clk_cfg.d_2 = d_2;

	/* configure and set the clock */
	reg_val = (mdss_clk_cfg.src << CLK_CTL_CFG_SRC_SEL_SHFT) |
			(mdss_clk_cfg.div << CLK_CTL_CFG_SRC_DIV_SHFT);

	write32(&mdss_clock[clk_type]->clock.rcg_cfg, reg_val);

	/* Set m/n/d values for a specific clock */
	if (mdss_clk_cfg.m != 0)
		clock_configure_mnd((struct sc7180_clock *)mdss_clock[clk_type],
			mdss_clk_cfg.m, mdss_clk_cfg.n, mdss_clk_cfg.d_2);

	/* Commit config to RCG */
	setbits32(&mdss_clock[clk_type]->clock.rcg_cmd,
						BIT(CLK_CTL_CMD_UPDATE_SHFT));

	return 0;
}

int mdss_clock_enable(enum mdss_clock clk_type)
{
	if (clk_type >= MDSS_CLK_COUNT)
		return -1;

	/* Enable clock*/
	clock_enable(mdss_cbcr[clk_type]);

	return 0;
}

void clock_init(void)
{
	clock_configure_gpll0();

	clock_enable_vote(&gcc->qup_wrap0_core_2x_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP0_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_core_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP0_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_m_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_0_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_s_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_0_S_AHB_CLK_ENA);

	clock_enable_vote(&gcc->qup_wrap1_core_2x_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP1_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_core_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP1_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_m_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_1_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_s_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_1_S_AHB_CLK_ENA);
	speed_up_boot_cpu();
}
