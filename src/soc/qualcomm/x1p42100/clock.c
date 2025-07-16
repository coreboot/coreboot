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
};

static u32 *gdsc[MAX_GDSC] = {
	[PCIE_6A_GDSC] = &gcc->pcie_6a.gdscr,
	[PCIE_6_PHY_GDSC] = &gcc->pcie_6_phy_gdscr,
};

struct pcie pcie_cfg[] = {
	[PCIE_6A_GDSC] = {
		.gdscr = &gcc->pcie_6a.gdscr,
	},
	[ANOC_PCIE_AT_CBCR] = {
		.clk = &gcc->pcie_noc.anoc_pcie_at_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = ANOC_PCIE_AT_CLK_ENA,
	},
	[ANOC_PCIE_QOSGEN_EXTREF_CBCR] = {
		.clk = &gcc->pcie_noc.anoc_pcie_qosgen_extref_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = ANOC_PCIE_QOSGEN_EXTREF_CLK_ENA,
	},
	[DDRSS_PCIE_SF_QTB_CBCR] = {
		.clk = &gcc->pcie_noc.ddrss_pcie_sf_qtb_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = DDRSS_PCIE_SF_QTB_CLK_ENA,
	},
	[CNOC_PCIE_NORTH_SF_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.cnoc_pcie_north_sf_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = CNOC_PCIE_NORTH_SF_AXI_CLK_ENA,
	},
	[CNOC_PCIE_SOUTH_SF_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.cnoc_pcie_south_sf_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = CNOC_PCIE_SOUTH_SF_AXI_CLK_ENA,
	},
	[NOC_PCIE_DCD_XO_CBCR] = {
		.clk = &gcc->pcie_noc.noc_pcie_dcd_xo_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = NOC_PCIE_DCD_XO_CLK_ENA,
	},
	[NOC_PCIE_SOUTH_DCD_XO_CBCR] = {
		.clk = &gcc->pcie_noc.noc_pcie_south_dcd_xo_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = NOC_PCIE_SOUTH_DCD_XO_CLK_ENA,
	},
	[NOC_PCIE_NORTH_DCD_XO_CBCR] = {
		.clk = &gcc->pcie_noc.noc_pcie_north_dcd_xo_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = NOC_PCIE_NORTH_DCD_XO_CLK_ENA,
	},
	[CFG_NOC_PCIE_ANOC_AHB_CBCR] = {
		.clk = &gcc->pcie_noc.cfg_noc_pcie_anoc_ahb_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = CFG_NOC_PCIE_ANOC_AHB_CLK_ENA,
	},
	[CFG_NOC_PCIE_ANOC_NORTH_AHB_CBCR] = {
		.clk = &gcc->pcie_noc.cfg_noc_pcie_anoc_north_ahb_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = CFG_NOC_PCIE_ANOC_NORTH_AHB_CLK_ENA,
	},
	[CFG_NOC_PCIE_ANOC_SOUTH_AHB_CBCR] = {
		.clk = &gcc->pcie_noc.cfg_noc_pcie_anoc_south_ahb_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = CFG_NOC_PCIE_ANOC_SOUTH_AHB_CLK_ENA,
	},
	[AGGRE_NOC_PCIE_NORTH_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.aggre_noc_pcie_north_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = AGGRE_NOC_PCIE_NORTH_AXI_CLK_ENA,
	},
	[ANOC_PCIE_PWRCTL_CBCR] = {
		.clk = &gcc->pcie_noc.anoc_pcie_pwrctl_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = ANOC_PCIE_PWRCTL_CLK_ENA,
	},
	[AGGRE_NOC_PCIE_SOUTH_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.aggre_noc_pcie_south_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = AGGRE_NOC_PCIE_SOUTH_AXI_CLK_ENA,
	},
	[AGGRE_NOC_PCIE_HS_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.aggre_noc_pcie_hs_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = AGGRE_NOC_PCIE_HS_AXI_CLK_ENA,
	},
	[AGGRE_NOC_PCIE_HS_NORTH_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.aggre_noc_pcie_hs_north_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = AGGRE_NOC_PCIE_HS_NORTH_AXI_CLK_ENA,
	},
	[AGGRE_NOC_PCIE_HS_SOUTH_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.aggre_noc_pcie_hs_south_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en5,
		.vote_bit = AGGRE_NOC_PCIE_HS_SOUTH_AXI_CLK_ENA,
	},
	[CNOC_PCIE_NORTH_SF_TUNNEL_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.cnoc_pcie_north_sf_tunnel_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = CNOC_PCIE_NORTH_SF_TUNNEL_AXI_CLK_ENA,
	},
	[CNOC_PCIE_SOUTH_SF_TUNNEL_AXI_CBCR] = {
		.clk = &gcc->pcie_noc.cnoc_pcie_south_sf_tunnel_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = CNOC_PCIE_SOUTH_SF_TUNNEL_AXI_CLK_ENA,
	},
	[PCIE_6A_SLV_Q2A_AXI_CLK] = {
		.clk = &gcc->pcie_6a.slv_q2a_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_SLV_Q2A_AXI_CLK_ENA,
	},
	[PCIE_6A_SLV_AXI_CLK] = {
		.clk = &gcc->pcie_6a.slv_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_SLV_AXI_CLK_ENA,
	},
	[PCIE_6A_MSTR_AXI_CLK] = {
		.clk = &gcc->pcie_6a.mstr_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_MSTR_AXI_CLK_ENA,
	},
	[PCIE_6A_CFG_AHB_CLK] = {
		.clk = &gcc->pcie_6a.cfg_ahb_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_CFG_AHB_CLK_ENA,
	},
	[PCIE_6A_AUX_CLK] = {
		.clk = &gcc->pcie_6a.aux_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_AUX_CLK_ENA,
	},
	[PCIE_6A_PHY_AUX_CLK] = {
		.clk = &gcc->pcie_6a.phy_aux_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_PHY_AUX_CLK_ENA,
	},
	[PCIE_6A_PHY_RCHNG_CLK] = {
		.clk = &gcc->pcie_6a.phy_rchng_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_PHY_RCHNG_CLK_ENA,
	},
	[PCIE_6A_PIPE_CLK] = {
		.clk = &gcc->pcie_6a.pipe_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_PIPE_CLK_ENA,
	},
	[PCIE_6A_PIPEDIV2_CLK] = {
		.clk = &gcc->pcie_6a.pipediv2_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en3,
		.vote_bit = PCIE_6A_PIPEDIV2_CLK_ENA,
	},
	[PCIE_6A_PIPE_MUXR] = {
		.clk = &gcc->pcie_6a.pipe_muxr,
		.vote_bit = NO_VOTE_BIT,
	},
};

static struct clock_freq_config pcie_core_cfg[] = {
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(6),
	},
};

void clock_configure_qspi(uint32_t hz)
{
	clock_configure(&gcc->qspi_core, qspi_core_cfg, hz, ARRAY_SIZE(qspi_core_cfg));
	clock_enable(&gcc->qspi_cnoc_ahb_cbcr);
	clock_enable(&gcc->qspi_core_cbcr);
}

void clock_enable_qup(int qup)
{
	struct qupv3_clock *qup_clk;
	int s = qup % QUP_WRAP1_S0, clk_en_off;
	void *clk_br_en_ptr = NULL;	// Pointer to the correct apcs_clk_br_enX
	qup_clk = qup < QUP_WRAP1_S0 ? &gcc->qup_wrap0_s[s] : qup < QUP_WRAP2_S0 ?
		&gcc->qup_wrap1_s[s] : &gcc->qup_wrap2_s[s];

	if (qup <= QUP_WRAP0_S7) {
		clk_en_off = QUPV3_WRAP0_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en4;
	} else if (qup >= QUP_WRAP1_S0 && qup <= QUP_WRAP1_S6) {
		clk_en_off = QUPV3_WRAP1_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en1;
	} else if (qup >= QUP_WRAP2_S0 && qup <= QUP_WRAP2_S6) {
		clk_en_off = QUPV3_WRAP2_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en2;
	} else if (qup == QUP_WRAP1_S7 || qup == QUP_WRAP2_S7) {
		clk_en_off = qup == QUP_WRAP1_S7 ? QUPV3_WRAP1_SE7_CLK_ENA : QUPV3_WRAP2_SE7_CLK_ENA;
		clk_br_en_ptr = &gcc->apcs_clk_br_en2;
	}

	/* Only call if a valid pointer was assigned */
	if (clk_br_en_ptr)
		clock_enable_vote(&qup_clk->cbcr, clk_br_en_ptr, clk_en_off);
}

void clock_configure_dfsr(int qup)
{
	clock_configure_dfsr_table(qup, qupv3_wrap_cfg,
						ARRAY_SIZE(qupv3_wrap_cfg));
}
static enum cb_err clock_configure_gpll0(void)
{
	struct alpha_pll_reg_val_config gpll0_cfg = {0};

	gpll0_cfg.reg_user_ctl = &gcc->gpll0.user_ctl;

	gpll0_cfg.user_ctl_val = (read32(gpll0_cfg.reg_user_ctl) |
				1 << PLL_POST_DIV_EVEN_SHFT_X1P42100 |
				2 << PLL_POST_DIV_ODD_SHFT_X1P42100 |
				1 << PLL_PLLOUT_EVEN_SHFT_X1P42100 |
				1 << PLL_PLLOUT_MAIN_SHFT_X1P42100 |
				1 << PLL_PLLOUT_ODD_SHFT_X1P42100);

	return clock_configure_enable_gpll(&gpll0_cfg, false, 0);
}


enum cb_err clock_enable_gdsc(enum clk_gdsc gdsc_type)
{
	if (gdsc_type > MAX_GDSC)
		return CB_ERR;

	return enable_and_poll_gdsc_status(gdsc[gdsc_type]);
}

enum cb_err clock_enable_pcie(enum clk_pcie clk_type)
{
	int clk_vote_bit;

	if (clk_type >= PCIE_CLK_COUNT)
		return CB_ERR;

	clk_vote_bit = pcie_cfg[clk_type].vote_bit;
	if (clk_vote_bit < 0)
		return clock_enable(pcie_cfg[clk_type].clk);

	return clock_enable_vote(pcie_cfg[clk_type].clk,
			pcie_cfg[clk_type].clk_br_en, pcie_cfg[clk_type].vote_bit);

}

enum cb_err clock_configure_mux(enum clk_pcie clk_type, u32 src_type)
{
	if (clk_type >= PCIE_CLK_COUNT)
		return CB_ERR;

	/* Set clock src */
	write32(pcie_cfg[clk_type].clk, src_type);

	return CB_SUCCESS;
}

void clock_configure_pcie(void)
{
	 clock_configure(&gcc->pcie_6a.phy_rchng_rcg,
			pcie_core_cfg, PCIE_PHY_RCHNG_FREQ, ARRAY_SIZE(pcie_core_cfg));
}

static void speed_up_boot_cpu(void)
{
	/* Placeholder */
}

void clock_init(void)
{
	clock_configure_gpll0();
	clock_enable_vote(&gcc->qup_wrap0_core_2x_cbcr,
				&gcc->apcs_clk_br_en4,
				QUPV3_WRAP0_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_core_cbcr,
				&gcc->apcs_clk_br_en4,
				QUPV3_WRAP0_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_m_ahb_cbcr,
				&gcc->apcs_clk_br_en4,
				QUPV3_WRAP_0_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_s_ahb_cbcr,
				&gcc->apcs_clk_br_en4,
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

	clock_enable_vote(&gcc->qup_wrap2_core_2x_cbcr,
				&gcc->apcs_clk_br_en2,
				QUPV3_WRAP2_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_core_cbcr,
				&gcc->apcs_clk_br_en2,
				QUPV3_WRAP2_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_m_ahb_cbcr,
				&gcc->apcs_clk_br_en2,
				QUPV3_WRAP_2_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_s_ahb_cbcr,
				&gcc->apcs_clk_br_en2,
				QUPV3_WRAP_2_S_AHB_CLK_ENA);
	speed_up_boot_cpu();

}
