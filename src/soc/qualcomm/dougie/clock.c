/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <types.h>

static struct clock_freq_config qspi_core_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2MHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(6),
	},
	{
		.hz = 150 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(4),
	},
	{
		.hz = 200 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(3),
	},
	{
		.hz = 400 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(1.5),
	},
};

static struct clock_freq_config qupv3_wrap_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2MHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz = 32 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 8,
		.n = 75,
		.d_2 = 75,
	},
	{
		.hz = 48 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 4,
		.n = 25,
		.d_2 = 25,
	},
	{
		.hz = 64 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 16,
		.n = 75,
		.d_2 = 75,
	},
	{
		.hz = 96 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 8,
		.n = 25,
		.d_2 = 25,
	},
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(6),
	},
	{
		.hz = SRC_XO_HZ,	/* 19.2MHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz = SRC_XO_HZ,	/* 19.2MHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
};

static enum cb_err clock_configure_gpll0(void)
{
	struct alpha_pll_reg_val_config gpll0_cfg = {0};

	gpll0_cfg.reg_user_ctl = &gcc->gpll0.user_ctl;
	gpll0_cfg.user_ctl_val = (1 << PLL_POST_DIV_EVEN_SHFT |
				3 << PLL_POST_DIV_ODD_SHFT |
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

void clock_enable_qup(int qup)
{
	struct qupv3_clock *qup_clk;
	int s = qup % QUP_WRAP1_S0, clk_en_off;

	qup_clk = qup < QUP_WRAP1_S0 ?
				&gcc->qup_wrap0_s[s] : &gcc->qup_wrap1_s[s];

	if (qup < QUP_WRAP1_S6) {
		clk_en_off = qup < QUP_WRAP1_S0 ?
			QUPV3_WRAP0_CLK_ENA_S(s) : QUPV3_WRAP1_CLK_ENA_S(s);
		clock_enable_vote(&qup_clk->cbcr, &gcc->apcs_clk_br_en1,
							clk_en_off);
	} else {
		clk_en_off = QUPV3_WRAP1_CLK_ENA_1_S(s);
		clock_enable_vote(&qup_clk->cbcr, &gcc->apcs_clk_br_en,
								clk_en_off);
	}
}

void clock_configure_dfsr(int qup)
{
	clock_configure_dfsr_table(qup, qupv3_wrap_cfg,
					ARRAY_SIZE(qupv3_wrap_cfg));
}

static enum cb_err pll_init_and_set(struct dougie_apss_clock *apss, u32 l_val)
{
	struct alpha_pll_reg_val_config pll_cfg = {0};
	int ret;
	u32 gfmux_val, regval;

	pll_cfg.reg_l = &apss->pll.l;
	pll_cfg.l_val = l_val;

	pll_cfg.reg_config_ctl = &apss->pll.config_ctl_lo;
	pll_cfg.reg_config_ctl_hi = &apss->pll.config_ctl_hi;
	pll_cfg.reg_config_ctl_hi1 = &apss->pll.config_ctl_u1;

	regval = read32(&apss->pll.config_ctl_lo);
	pll_cfg.config_ctl_val = regval &
					(~(0x2 << K_P_SHFT | 0x2 << K_I_SHFT));

	regval = read32(&apss->pll.config_ctl_hi);
	pll_cfg.config_ctl_hi_val = (regval | (BIT(KLSB_SHFT) |
				BIT(RON_MODE_SHFT))) & (~(0x4 << KLSB_SHFT));

	regval = read32(&apss->pll.config_ctl_u1);
	pll_cfg.config_ctl_hi1_val = (regval | BIT(FAST_LOCK_LOW_L_SHFT)) &
					~BIT(DCO_BIAS_ADJ_SHFT);

	ret = clock_configure_enable_gpll(&pll_cfg, false, 0);
	if (ret != CB_SUCCESS)
		return CB_ERR;

	pll_cfg.reg_mode = &apss->pll.mode;
	pll_cfg.reg_opmode = &apss->pll.opmode;
	pll_cfg.reg_user_ctl = &apss->pll.user_ctl;

	ret = zonda_pll_enable(&pll_cfg);
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

	/* 1190.4 MHz */
	if (!pll_init_and_set(apss_l3, L_VAL_1190P4MHz))
		printk(BIOS_DEBUG, "L3 Frequency bumped to 1.1904(GHz)\n");
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
