/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <timer.h>
#include <types.h>

static struct clock_freq_config qspi_core_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(3),
	},
	{
		.hz = 150 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(2),
	},
	{
		.hz = GPLL0_EVEN_HZ,	/* 300MHz */
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
	}
};

static struct clock_freq_config qupv3_wrap_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz =  32 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 8,
		.n = 75,
		.d_2 = 75,
	},
	{
		.hz =  48 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 4,
		.n = 25,
		.d_2 = 25,
	},
	{
		.hz =  64 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 16,
		.n = 75,
		.d_2 = 75,
	},
	{
		.hz =  96 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 8,
		.n = 25,
		.d_2 = 25,
	},
	{
		.hz =  100 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(3),
	},
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
};

static struct clock_rcg_mnd *mdss_clock[MDSS_CLK_COUNT] = {
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
	struct alpha_pll_reg_val_config gpll0_cfg = {0};

	gpll0_cfg.reg_user_ctl_hi = &gcc->gpll0.user_ctl_u;
	gpll0_cfg.user_ctl_hi_val = 1 << SCALE_FREQ_SHFT;

	gpll0_cfg.reg_user_ctl = &gcc->gpll0.user_ctl;
	gpll0_cfg.user_ctl_val = (1 << PLL_POST_DIV_EVEN_SHFT |
				1 << PLL_PLLOUT_EVEN_SHFT |
				1 << PLL_PLLOUT_MAIN_SHFT |
				1 << PLL_PLLOUT_ODD_SHFT);

	return clock_configure_enable_gpll(&gpll0_cfg, false, 0);
}

void clock_configure_qspi(uint32_t hz)
{
	clock_configure(&gcc->qspi_core,
			qspi_core_cfg, hz,
			ARRAY_SIZE(qspi_core_cfg));
	clock_enable(&gcc->qspi_cnoc_ahb_cbcr);
	clock_enable(&gcc->qspi_core_cbcr);
}

void clock_configure_dfsr(int qup)
{
	clock_configure_dfsr_table(qup, qupv3_wrap_cfg,
					ARRAY_SIZE(qupv3_wrap_cfg));
}

void clock_enable_qup(int qup)
{
	int s = qup % QUP_WRAP1_S0;
	int clk_en_off = qup < QUP_WRAP1_S0 ?
			QUPV3_WRAP0_CLK_ENA_S(s) : QUPV3_WRAP1_CLK_ENA_S(s);
	struct qupv3_clock *qup_clk = qup < QUP_WRAP1_S0 ?
				&gcc->qup_wrap0_s[s] : &gcc->qup_wrap1_s[s];

	clock_enable_vote(&qup_clk->cbcr, &gcc->apcs_clk_br_en1,
				clk_en_off);
}

static enum cb_err pll_init_and_set(struct sc7180_apss_clock *apss, u32 l_val)
{
	struct alpha_pll_reg_val_config pll_cfg = {0};
	int ret;
	u32 gfmux_val;

	pll_cfg.reg_config_ctl = &apss->pll.config_ctl_lo;
	pll_cfg.reg_config_ctl_hi = &apss->pll.config_ctl_hi;
	pll_cfg.reg_config_ctl_hi1 = &apss->pll.config_ctl_u1;

	pll_cfg.config_ctl_val = (0x2 << CTUNE_SHFT | 0x2 << K_I_SHFT |
				 0x5 << K_P_SHFT | 0x2 << PFA_MSB_SHFT |
				 0x2 << REF_CONT_SHFT);
	pll_cfg.config_ctl_hi_val = (0x2 << CUR_ADJ_SHFT | BIT(DMET_SHFT) |
					0xF << RES_SHFT);

	write32(&apss->pll.config_ctl_u1, 0x0);
	pll_cfg.reg_l = &apss->pll.l;
	pll_cfg.l_val = l_val;

	ret = clock_configure_enable_gpll(&pll_cfg, false, 0);
	if (ret != CB_SUCCESS)
		return CB_ERR;

	pll_cfg.reg_mode = &apss->pll.mode;
	ret = agera_pll_enable(&pll_cfg);
	if (ret != CB_SUCCESS)
		return CB_ERR;

	gfmux_val = read32(&apss->cfg_gfmux) & ~GFMUX_SRC_SEL_BMSK;
	gfmux_val |= APCS_SRC_EARLY;
	write32(&apss->cfg_gfmux, gfmux_val);

	return CB_SUCCESS;
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

enum cb_err mdss_clock_configure(enum mdss_clock clk_type, uint32_t source,
				uint32_t divider, uint32_t m,
				uint32_t n, uint32_t d_2)
{
	struct clock_freq_config mdss_clk_cfg;

	if (clk_type >= MDSS_CLK_COUNT)
		return CB_ERR;

	/* Initialize it with received arguments */
	mdss_clk_cfg.hz = 0;
	mdss_clk_cfg.src = source;

	/*
	 * Update half_divider passed from display, this is to accommodate
	 * the transition to common clock driver.
	 *
	 * client is expected to provide 2n divider value,
	 * as the divider value in register is in form "2n-1"
	 */
	mdss_clk_cfg.div = divider ? ((divider * 2) - 1) : 0;
	mdss_clk_cfg.m = m;
	mdss_clk_cfg.n = n;
	mdss_clk_cfg.d_2 = d_2;

	return clock_configure((struct clock_rcg *)mdss_clock[clk_type],
			       &mdss_clk_cfg, 0, 1);
}

int mdss_clock_enable(enum mdss_clock clk_type)
{
	if (clk_type >= MDSS_CLK_COUNT)
		return CB_ERR;

	/* Enable clock */
	return clock_enable(mdss_cbcr[clk_type]);
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
