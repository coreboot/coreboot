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
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(6),
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

static struct clock_freq_config sdcc1_core_cfg[] = {
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(3),
	},
	{
		.hz = 192 * MHz,
		.src = SRC_GPLL10_MAIN_384MHZ,
		.div = QCOM_CLOCK_DIV(2),
	},
	{
		.hz = 384 * MHz,
		.src = SRC_GPLL10_MAIN_384MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
};

static struct clock_freq_config sdcc2_core_cfg[] = {
	{
		.hz = 50 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(6),
	},
	{
		.hz = 202 * MHz,
		.src = SRC_GPLL9_MAIN_808MHZ,
		.div = QCOM_CLOCK_DIV(4),
	},
};

static struct pcie pcie_cfg[] = {
	[PCIE_1_GDSC] = {
		.gdscr = &gcc->pcie_1.gdscr,
	},
	[PCIE_1_SLV_Q2A_AXI_CLK] = {
		.clk = &gcc->pcie_1.slv_q2a_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = PCIE_1_SLV_Q2A_AXI_CLK_ENA,
	},
	[PCIE_1_SLV_AXI_CLK] = {
		.clk = &gcc->pcie_1.slv_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = PCIE_1_SLV_AXI_CLK_ENA,
	},
	[PCIE_1_MSTR_AXI_CLK] = {
		.clk = &gcc->pcie_1.mstr_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = PCIE_1_MSTR_AXI_CLK_ENA,
	},
	[PCIE_1_CFG_AHB_CLK] = {
		.clk = &gcc->pcie_1.cfg_ahb_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = PCIE_1_CFG_AHB_CLK_ENA,
	},
	[PCIE_1_AUX_CLK] = {
		.clk = &gcc->pcie_1.aux_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = PCIE_1_AUX_CLK_ENA,
	},
	[AGGRE_NOC_PCIE_TBU_CLK] = {
		.clk = &gcc->aggre_noc_pcie_tbu_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = AGGRE_NOC_PCIE_TBU_CLK_ENA,
	},
	[AGGRE_NOC_PCIE_1_AXI_CLK] = {
		.clk = &gcc->pcie_1.aggre_noc_pcie_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = AGGRE_NOC_PCIE_1_AXI_CLK_ENA,
	},
	[DDRSS_PCIE_SF_CLK] = {
		.clk = &gcc->pcie_1.ddrss_pcie_sf_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = DDRSS_PCIE_SF_CLK_ENA,
	},
	[PCIE1_PHY_RCHNG_CLK] = {
		.clk = &gcc->pcie_1.phy_rchng_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = PCIE1_PHY_RCHNG_CLK_ENA,
	},
	[AGGRE_NOC_PCIE_CENTER_SF_AXI_CLK] = {
		.clk = &gcc->pcie_1.aggre_noc_pcie_center_sf_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = AGGRE_NOC_PCIE_CENTER_SF_AXI_CLK_ENA,
	},
	[PCIE_1_PIPE_CLK] = {
		.clk = &gcc->pcie_1.pipe_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = PCIE_1_PIPE_CLK_ENA,
	},
	[PCIE_CLKREF_EN] = {
		.clk = &gcc->pcie_clkref_en,
		.vote_bit = NO_VOTE_BIT,
	},
	[GCC_PCIE_1_PIPE_MUXR] = {
		.clk = &gcc->pcie_1.pipe_muxr,
		.vote_bit = NO_VOTE_BIT,
	},
};

static struct clock_freq_config mdss_mdp_cfg[] = {
	{
		.hz = 200 * MHz,
		.src = SRC_GCC_DISP_GPLL0_CLK,
		.div = QCOM_CLOCK_DIV(3),
	},
	{
		.hz = 300 * MHz,
		.src = SRC_GCC_DISP_GPLL0_CLK,
		.div = QCOM_CLOCK_DIV(2),
	},
};

static struct clock_rcg *mdss_clock[MDSS_CLK_COUNT] = {
	[MDSS_CLK_MDP] = &mdss->mdp,
	[MDSS_CLK_VSYNC] = &mdss->vsync,
	[MDSS_CLK_ESC0] = &mdss->esc0,
	[MDSS_CLK_BYTE0] = &mdss->byte0,
	[MDSS_CLK_BYTE0_INTF] = &mdss->byte0,
	[MDSS_CLK_AHB] = &mdss->mdss_ahb,
	[MDSS_CLK_EDP_LINK] = &mdss->edp_link,
	[MDSS_CLK_EDP_LINK_INTF] = &mdss->edp_link,
	[MDSS_CLK_EDP_AUX] = &mdss->edp_aux,
};

static struct clock_rcg_mnd *mdss_clock_mnd[MDSS_CLK_COUNT] = {
	[MDSS_CLK_PCLK0] = &mdss->pclk0,
	[MDSS_CLK_EDP_PIXEL] = &mdss->edp_pixel,
};

static u32 *mdss_cbcr[MDSS_CLK_COUNT] = {
	[GCC_DISP_AHB] = &gcc->disp_ahb_cbcr,
	[GCC_DISP_HF_AXI] = &gcc->disp_hf_axi_cbcr,
	[GCC_DISP_SF_AXI] = &gcc->disp_sf_axi_cbcr,
	[GCC_EDP_CLKREF_EN] = &gcc->edp_clkref_en,
	[MDSS_CLK_PCLK0] = &mdss->pclk0_cbcr,
	[MDSS_CLK_MDP] = &mdss->mdp_cbcr,
	[MDSS_CLK_VSYNC] = &mdss->vsync_cbcr,
	[MDSS_CLK_BYTE0] = &mdss->byte0_cbcr,
	[MDSS_CLK_BYTE0_INTF] = &mdss->byte0_intf_cbcr,
	[MDSS_CLK_ESC0] = &mdss->esc0_cbcr,
	[MDSS_CLK_AHB] = &mdss->ahb_cbcr,
	[MDSS_CLK_EDP_PIXEL] = &mdss->edp_pixel_cbcr,
	[MDSS_CLK_EDP_LINK] = &mdss->edp_link_cbcr,
	[MDSS_CLK_EDP_LINK_INTF] = &mdss->edp_link_intf_cbcr,
	[MDSS_CLK_EDP_AUX] = &mdss->edp_aux_cbcr,
};

static u32 *gdsc[MAX_GDSC] = {
	[PCIE_1_GDSC] = &gcc->pcie_1.gdscr,
	[MDSS_CORE_GDSC] = &mdss->core_gdsc,
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

void clock_configure_sdcc(enum clk_sdcc sdcc, uint32_t hz)
{
	if (sdcc == SDCC1_CLK) {
		if (hz > CLK_100MHZ) {
			struct alpha_pll_reg_val_config gpll10_cfg = {0};
			gpll10_cfg.reg_mode = &gcc->gpll10.mode;
			gpll10_cfg.reg_opmode = &gcc->gpll10.opmode;
			gpll10_cfg.reg_l = &gcc->gpll10.l;
			gpll10_cfg.l_val = 0x14;
			gpll10_cfg.reg_cal_l = &gcc->gpll10.cal_l;
			gpll10_cfg.cal_l_val = 0x44;
			gpll10_cfg.fsm_enable = true;
			gpll10_cfg.reg_apcs_pll_br_en = &gcc->apcs_pll_br_en;
			clock_configure_enable_gpll(&gpll10_cfg, true, 9);
		}
		clock_configure((struct clock_rcg *)&gcc->sdcc1, sdcc1_core_cfg,
					hz, ARRAY_SIZE(sdcc1_core_cfg));
		clock_enable(&gcc->sdcc1_ahb_cbcr);
		clock_enable(&gcc->sdcc1_apps_cbcr);
	} else if (sdcc == SDCC2_CLK) {
		if (hz > CLK_100MHZ) {
			struct alpha_pll_reg_val_config gpll9_cfg = {0};
			gpll9_cfg.reg_mode = &gcc->gpll9.mode;
			gpll9_cfg.reg_opmode = &gcc->gpll9.opmode;
			gpll9_cfg.reg_alpha = &gcc->gpll9.alpha;
			gpll9_cfg.alpha_val = 0x1555;
			gpll9_cfg.reg_l = &gcc->gpll9.l;
			gpll9_cfg.l_val = 0x2A;
			gpll9_cfg.reg_cal_l = &gcc->gpll9.cal_l;
			gpll9_cfg.cal_l_val = 0x44;
			gpll9_cfg.fsm_enable = true;
			gpll9_cfg.reg_apcs_pll_br_en = &gcc->apcs_pll_br_en;
			clock_configure_enable_gpll(&gpll9_cfg, true, 8);
		}
		clock_configure((struct clock_rcg *)&gcc->sdcc2, sdcc2_core_cfg,
			       hz, ARRAY_SIZE(sdcc2_core_cfg));
		clock_enable(&gcc->sdcc2_ahb_cbcr);
		clock_enable(&gcc->sdcc2_apps_cbcr);
	}
}

void clock_configure_dfsr(int qup)
{
	clock_configure_dfsr_table(qup, qupv3_wrap_cfg,
					ARRAY_SIZE(qupv3_wrap_cfg));
}

static enum cb_err pll_init_and_set(struct sc7280_apss_clock *apss, u32 l_val)
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
	pll_cfg.config_ctl_val =  regval &
					(~(0x2 << K_P_SHFT | 0x2 << K_I_SHFT));

	regval = read32(&apss->pll.config_ctl_hi);
	pll_cfg.config_ctl_hi_val =  (regval | (BIT(KLSB_SHFT) |
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

	ret =  zonda_pll_enable(&pll_cfg);
	if (ret != CB_SUCCESS)
		return CB_ERR;

	gfmux_val = read32(&apss->cfg_gfmux) & ~GFMUX_SRC_SEL_BMSK;
	gfmux_val |= APCS_SRC_EARLY;
	write32(&apss->cfg_gfmux, gfmux_val);

	return CB_SUCCESS;
}

enum cb_err clock_enable_gdsc(enum clk_gdsc gdsc_type)
{
	if (gdsc_type > MAX_GDSC)
		return CB_ERR;

	return enable_and_poll_gdsc_status(gdsc[gdsc_type]);
}

enum cb_err mdss_clock_configure(enum clk_mdss clk_type, uint32_t hz,
				uint32_t source, uint32_t divider, uint32_t m,
				uint32_t n, uint32_t d_2)
{
	struct clock_freq_config mdss_clk_cfg;
	uint32_t idx;

	if (clk_type >= MDSS_CLK_COUNT)
		return CB_ERR;

	/* Initialize it with received arguments */
	mdss_clk_cfg.div = divider ?  QCOM_CLOCK_DIV(divider) : 0;

	if (clk_type == MDSS_CLK_MDP) {
		for (idx = 0; idx < ARRAY_SIZE(mdss_mdp_cfg); idx++) {
			if (hz <= mdss_mdp_cfg[idx].hz) {
				source = mdss_mdp_cfg[idx].src;
				mdss_clk_cfg.div = mdss_mdp_cfg[idx].div;
				m = 0;
				break;
			}
		}
	}
	mdss_clk_cfg.src = source;
	mdss_clk_cfg.m = m;
	mdss_clk_cfg.n = n;
	mdss_clk_cfg.d_2 = d_2;

	switch (clk_type) {
	case MDSS_CLK_EDP_PIXEL:
	case MDSS_CLK_PCLK0:
		return clock_configure((struct clock_rcg *)
				mdss_clock_mnd[clk_type],
				&mdss_clk_cfg, hz, 0);
	default:
		return clock_configure(mdss_clock[clk_type],
				&mdss_clk_cfg, hz, 0);
	}
}

enum cb_err mdss_clock_enable(enum clk_mdss clk_type)
{
	if (clk_type >= MDSS_CLK_COUNT)
		return CB_ERR;

	/* Enable clock */
	return clock_enable(mdss_cbcr[clk_type]);
}

enum cb_err clock_enable_pcie(enum clk_pcie clk_type)
{
	int clk_vote_bit;

	if (clk_type >= PCIE_CLK_COUNT)
		return CB_ERR;

	clk_vote_bit = pcie_cfg[clk_type].vote_bit;
	if (clk_vote_bit < 0)
		return clock_enable(pcie_cfg[clk_type].clk);

	clock_enable_vote(pcie_cfg[clk_type].clk,
			pcie_cfg[clk_type].clk_br_en,
			pcie_cfg[clk_type].vote_bit);

	return CB_SUCCESS;
}

enum cb_err clock_configure_mux(enum clk_pcie clk_type, u32 src_type)
{
	if (clk_type >= PCIE_CLK_COUNT)
		return CB_ERR;

	/* Set clock src */
	write32(pcie_cfg[clk_type].clk, src_type);

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
