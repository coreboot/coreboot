/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
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
		.hz = 300 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(2),
	},
	{
		.hz = 400 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(1.5),
	},
};

void clock_configure_qspi(uint32_t hz)
{
	clock_configure(&gcc->qspi_core, qspi_core_cfg, hz,
			ARRAY_SIZE(qspi_core_cfg));
	clock_enable(&gcc->qspi_cnoc_ahb_cbcr);
	clock_enable(&gcc->qspi_core_cbcr);
}

/* Helper function to get SE index within a wrap */
static inline int get_qup_se_index(int qup)
{
	return qup % QUP_WRAP1_S0;
}

/* Helper function to get pointer to QUP SE clock structure */
static void *get_qup_se_clk(int qup, int s)
{
	struct calypso_qupv3_wrap *wrap = NULL;
	void *se_clk_array[8];

	/* Determine which wrap based on qup value */
	if (qup <= QUP_WRAP0_S7)
		wrap = qup_wrap0_clk;
	else if (qup <= QUP_WRAP1_S7)
		wrap = qup_wrap1_clk;
	else if (qup <= QUP_WRAP2_S7)
		wrap = qup_wrap2_clk;
	else if (qup <= QUP_WRAP3_S1)
		wrap = qup_oob_clk;

	if (!wrap) {
		printk(BIOS_ERR, "%s: invalid QUP index %d\n", __func__, qup);
		return NULL;
	}

	/* Build array of pointers to SE clock structures */
	se_clk_array[0] = &wrap->qupv3_s0;
	se_clk_array[1] = &wrap->qupv3_s1;
	se_clk_array[2] = &wrap->qupv3_s2;
	se_clk_array[3] = &wrap->qupv3_s3;
	se_clk_array[4] = &wrap->qupv3_s4;
	se_clk_array[5] = &wrap->qupv3_s5;
	se_clk_array[6] = &wrap->qupv3_s6;
	se_clk_array[7] = &wrap->qupv3_s7;
	return se_clk_array[s];
}

void clock_enable_qup(int qup)
{
	int s = get_qup_se_index(qup), clk_en_off;
	void *clk_br_en_ptr = NULL;
	void *se_clk;
	u32 *cbcr;

	if (qup <= QUP_WRAP0_S7) {
		clk_en_off = QUPV3_WRAP0_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en4;
	} else if (qup <= QUP_WRAP1_S7) {
		clk_en_off = QUPV3_WRAP1_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en3;
	} else if (qup <= QUP_WRAP2_S7) {
		if (qup <= QUP_WRAP2_S1) {
			clk_en_off = (qup == QUP_WRAP2_S0) ?
				QUPV3_WRAP2_SE0_CLK_ENA :
				QUPV3_WRAP2_SE1_CLK_ENA;
			clk_br_en_ptr = &gcc->apcs_clk_br_en3;
		} else {
			clk_en_off = QUPV3_WRAP2_CLK_ENA_S(s);
			clk_br_en_ptr = &gcc->apcs_clk_br_en4;
		}
	} else if (qup <= QUP_WRAP3_S1) {
		clk_en_off = QUPV3_OOB_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en3;
	}

	se_clk = get_qup_se_clk(qup, s);
	if (!se_clk)
		return;

	if (s == 2 || s == 3)
		cbcr = &((struct qupv3_clock_v2 *)se_clk)->cbcr;
	else
		cbcr = &((struct qupv3_clock *)se_clk)->cbcr;

	/* Only call if a valid pointer was assigned */
	if (clk_br_en_ptr)
		clock_enable_vote(cbcr, clk_br_en_ptr, clk_en_off);
}

void clock_configure_dfsr(int qup)
{
	/* placeholder */
}

enum cb_err pll_init_and_set(struct calypso_ncc0_clock *ncc0, u32 l_val)
{
	int ret;
	struct alpha_pll_reg_val_config ncc0_pll_cfg = {0};

	setbits64p(NCC0_NCC_CMU_NCC_CLK_CFG,
		   BIT(PLLSWCTL) | BIT(OVRCKMUXPLLFASTCLK));

	setbits64p(NCC0_NCC_CMU_NCC_PLL_CFG,
		   ((uint64_t)(LOCKTMOUTCNT_VAL & LOCKTMOUTCNT_BMSK)
		    << LOCKTMOUTCNT) |
		   ((uint64_t)(LOCKDEASSERTTMOUTCNT_VAL & LOCKDEASSERTTMOUTCNT_BMSK)
		    << LOCKDEASSERTTMOUTCNT));

	ncc0_pll_cfg.reg_config_ctl = &ncc0->pll0_config_ctl;
	ncc0_pll_cfg.config_ctl_val = read32(ncc0_pll_cfg.reg_config_ctl) |
				      PFA_MSB_VAL << PFA_MSB |
				      RON_DEGEN_MULTIPLY_VAL << RON_DEGEN_MULTIPLY |
				      FBC_ALPHA_CAL_VAL << FBC_ALPHA_CAL |
				      PLL_COUNTER_ENABLE_VAL << PLL_COUNTER_ENABLE;

	ncc0_pll_cfg.reg_config_ctl_hi = &ncc0->pll0_config_ctl_u;
	ncc0_pll_cfg.config_ctl_hi_val = read32(ncc0_pll_cfg.reg_config_ctl_hi) |
					 CHP_REF_CUR_TRIM_VAL << CHP_REF_CUR_TRIM |
					 ADC_KLSB_VALUE_VAL << ADC_KLSB_VALUE |
					 ADC_KMSB_VALUE_VAL << ADC_KMSB_VALUE;

	ncc0_pll_cfg.reg_l = &ncc0->pll0_l;
	ncc0_pll_cfg.l_val = l_val;

	ncc0_pll_cfg.reg_alpha = &ncc0->pll0_alpha;
	ncc0_pll_cfg.alpha_val = 0x00;
	ncc0_pll_cfg.reg_user_ctl = &ncc0->pll0_user_ctl;
	ncc0_pll_cfg.user_ctl_val = (1 << PLL_PLLOUT_EVEN_SHFT |
			1 << PLL_PLLOUT_MAIN_SHFT);

	if (clock_configure_enable_gpll(&ncc0_pll_cfg, false, 0) != CB_SUCCESS)
		return CB_ERR;

	ncc0_pll_cfg.reg_mode = &ncc0->pll0_mode;
	ncc0_pll_cfg.reg_opmode = &ncc0->pll0_opmode;
	ret = zondaole_pll_enable(&ncc0_pll_cfg);
	if (ret != CB_SUCCESS)
		return CB_ERR;

	setbits64p(NCC0_NCC_CMU_NCC_CLK_CFG, BIT(SELCKMUXPLLFASTCLK));

	return CB_SUCCESS;
}

static enum cb_err clock_configure_gpll0(void)
{
	struct alpha_pll_reg_val_config gpll0_cfg = {0};

	gpll0_cfg.reg_user_ctl = &gcc->gpll0.user_ctl;
	gpll0_cfg.user_ctl_val = read32(gpll0_cfg.reg_user_ctl) |
				 BIT(PLL_PLLOUT_MAIN_SHFT_CALYPSO) |
				 BIT(PLL_PLLOUT_EVEN_SHFT_CALYPSO) |
				 BIT(PLL_PLLOUT_ODD_SHFT_CALYPSO) |
				 1 << PLL_POST_DIV_EVEN_SHFT_CALYPSO |
				 2 << PLL_POST_DIV_ODD_SHFT_CALYPSO;

	return clock_configure_enable_gpll(&gpll0_cfg, false, 0);
}

static void speed_up_boot_cpu(void)
{
	/* 1363.2 MHz */
	if (pll_init_and_set(apss_ncc0, L_VAL_1363P2MHz) == CB_SUCCESS)
		printk(BIOS_DEBUG, "NCC frequency bumped to 1363.2 MHz\n");
	else
		printk(BIOS_ERR, "NCC PLL initialization failed\n");
}

void clock_init(void)
{
	if (clock_configure_gpll0() != CB_SUCCESS)
		printk(BIOS_ERR, "GPLL0 configuration failed\n");

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
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP1_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_core_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP1_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_m_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP_1_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_s_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP_1_S_AHB_CLK_ENA);

	clock_enable_vote(&gcc->qup_wrap2_core_2x_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP2_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_core_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP2_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_m_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP_2_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_s_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP_2_S_AHB_CLK_ENA);

	clock_enable_vote(&gcc->qup_oob_core_2x_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_OOB_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_oob_core_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_OOB_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_oob_s_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_OOB_S_AHB_CLK_ENA);

	speed_up_boot_cpu();
}

static struct pcie pcie_cfg[] = {
	[GCC_AGGRE_NOC_PCIE_5_EAST_SF_AXI_CLK] = {
		.clk = &gcc->pcie_5.aggre_noc_pcie_5_east_sf_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = AGGRE_NOC_PCIE_5_EAST_SF_AXI_CLK_ENA,
	},
	[GCC_AGGRE_NOC_PCIE_EAST_TUNNEL_AXI_CLK] = {
		.clk = &gcc->pcie_noc.aggre_noc_pcie_east_tunnel_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = AGGRE_NOC_PCIE_EAST_TUNNEL_AXI_CLK_ENA,
	},
	[GCC_AGGRE_NOC_PCIE_WEST_TUNNEL_AXI_CLK] = {
		.clk = &gcc->pcie_noc.aggre_noc_pcie_west_tunnel_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = AGGRE_NOC_PCIE_WEST_TUNNEL_AXI_CLK_ENA,
	},
	[GCC_ANOC_PCIE_PWRCTL_CLK] = {
		.clk = &gcc->anoc_pcie_pwrctl_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = ANOC_PCIE_PWRCTL_CLK_ENA,
	},
	[GCC_CNOC_PCIE_EAST_TUNNEL_CLK] = {
		.clk = &gcc->pcie_noc.cnoc_pcie_east_tunnel_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = CNOC_PCIE_EAST_TUNNEL_CLK_ENA,
	},
	[GCC_CNOC_PCIE_WEST_TUNNEL_CLK] = {
		.clk = &gcc->pcie_noc.cnoc_pcie_west_tunnel_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = CNOC_PCIE_WEST_TUNNEL_CLK_ENA,
	},
	[GCC_PCIE_LINK_XO_CLK] = {
		.clk = &gcc->pcie_noc.pcie_link_xo_cbcr,
		.vote_bit = NO_VOTE_BIT,
	},
	[GCC_HSCNOC_PCIE_SF_QTC_CLK] = {
		.clk = &gcc->pcie_noc.hscnoc_pcie_sf_qtc_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = HSCNOC_PCIE_SF_QTC_CLK_ENA,
	},
	[GCC_HSCNOC_PCIE_SLAVE_SF_EAST_CLK] = {
		.clk = &gcc->pcie_noc.hscnoc_pcie_slave_sf_east_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = HSCNOC_PCIE_SLAVE_SF_EAST_CLK_ENA,
	},
	[GCC_HSCNOC_PCIE_SLAVE_SF_WEST_CLK] = {
		.clk = &gcc->pcie_noc.hscnoc_pcie_slave_sf_west_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = HSCNOC_PCIE_SLAVE_SF_WEST_CLK_ENA,
	},
	[GCC_PCIE_NOC_PWRCTL_CLK] = {
		.clk = &gcc->pcie_noc.pcie_noc_pwrctl_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = PCIE_NOC_PWRCTL_CLK_ENA,
	},
	[GCC_PCIE_NOC_SF_CENTER_CLK] = {
		.clk = &gcc->pcie_noc.pcie_noc_sf_center_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = PCIE_NOC_SF_CENTER_CLK_ENA,
	},
	[GCC_PCIE_NOC_SLAVE_SF_EAST_CLK] = {
		.clk = &gcc->pcie_noc.pcie_noc_slave_sf_east_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = PCIE_NOC_SLAVE_SF_EAST_CLK_ENA,
	},
	[GCC_PCIE_NOC_SLAVE_SF_WEST_CLK] = {
		.clk = &gcc->pcie_noc.pcie_noc_slave_sf_west_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en1,
		.vote_bit = PCIE_NOC_SLAVE_SF_WEST_CLK_ENA,
	},
	[GCC_SMMU_PCIE_QTC_AT_CLK] = {
		.clk = &gcc->smmu_pcie_qtc_at_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = SMMU_PCIE_QTC_AT_CLK_ENA,
	},
	[GCC_TCU_PCIE_SF_QTC_CLK] = {
		.clk = &gcc->pcie_noc.tcu_pcie_sf_qtc_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en,
		.vote_bit = TCU_PCIE_SF_QTC_CLK_ENA,
	},
	[GCC_TRACE_NOC_PCIE_3B_AT_CLK] = {
		.clk = &gcc->pcie_noc.trace_noc_pcie_3b_at_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = TRACE_NOC_PCIE_3B_AT_CLK_ENA,
	},
	[GCC_TRACE_NOC_PCIE_5_AT_CLK] = {
		.clk = &gcc->pcie_noc.trace_noc_pcie_5_at_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = TRACE_NOC_PCIE_5_AT_CLK_ENA,
	},
	[GCC_TRACE_NOC_TCU_PCIE_QTC_AT_CLK] = {
		.clk = &gcc->trace_noc_tcu_pcie_qtc_at_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en2,
		.vote_bit = TRACE_NOC_TCU_PCIE_QTC_AT_CLK_ENA,
	},
	[GCC_PCIE_5_AT_CLK] = {
		.clk = &gcc->pcie_5.at_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_AT_CLK_ENA,
	},
	[GCC_PCIE_5_AUX_CLK] = {
		.clk = &gcc->pcie_5.aux_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_AUX_CLK_ENA,
	},
	[GCC_PCIE_5_CFG_AHB_CLK] = {
		.clk = &gcc->pcie_5_cfg_ahb_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_CFG_AHB_CLK_ENA,
	},
	[GCC_PCIE_5_MSTR_AXI_CLK] = {
		.clk = &gcc->pcie_5.mstr_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_MSTR_AXI_CLK_ENA,
	},
	[GCC_PCIE_5_PHY_RCHNG_CLK] = {
		.clk = &gcc->pcie_5.phy_rchng_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_PHY_RCHNG_CLK_ENA,
	},
	[GCC_PCIE_5_PIPE_DIV2_CLK] = {
		.clk = &gcc->pcie_5.pipe_div2_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_PIPE_DIV2_CLK_ENA,
	},
	[GCC_PCIE_5_SLV_AXI_CLK] = {
		.clk = &gcc->pcie_5.slv_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_SLV_AXI_CLK_ENA,
	},
	[GCC_PCIE_5_SLV_Q2A_AXI_CLK] = {
		.clk = &gcc->pcie_5.slv_q2a_axi_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_SLV_Q2A_AXI_CLK_ENA,
	},
	[GCC_PCIE_PHY_5_AUX_CLK] = {
		.clk = &gcc->pcie_phy_5_aux_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_PHY_5_AUX_CLK_ENA,
	},
	[GCC_PCIE_5_PIPE_CLK] = {
		.clk = &gcc->pcie_5.pipe_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = PCIE_5_PIPE_CLK_ENA,
	},
	[GCC_PCIE_5_PIPE_MUXR] = {
		.clk = &gcc->pcie_5.pipe_muxr,
		.vote_bit = NO_VOTE_BIT,
	},
	[GCC_QMIP_PCIE_5_AHB_CLK] = {
		.clk = &gcc->pcie_5.qmip_pcie_5_ahb_cbcr,
		.clk_br_en = &gcc->apcs_clk_br_en6,
		.vote_bit = QMIP_PCIE_5_AHB_CLK_ENA,
	},
};

static u32 *gdsc_regs[MAX_GDSC] = {
	[PCIE_5_GDSC]     = &gcc->pcie_5.gdscr,
	[PCIE_5_PHY_GDSC] = &gcc->pcie_phy_5_gdscr,
};

enum cb_err clock_enable_pcie(enum clk_pcie clk_type)
{
	int clk_vote_bit;

	if (clk_type >= PCIE_CLK_COUNT) {
		printk(BIOS_ERR, "%s: invalid PCIe clock %d\n", __func__, clk_type);
		return CB_ERR;
	}

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

enum cb_err clock_enable_gdsc(enum clk_gdsc gdsc_type)
{
	if (gdsc_type >= MAX_GDSC) {
		printk(BIOS_ERR, "%s: invalid GDSC %d\n", __func__, gdsc_type);
		return CB_ERR;
	}

	return enable_and_poll_gdsc_status(gdsc_regs[gdsc_type]);
}

/* Configure gcc_pcie_5_phy_rchng_clk to 100 MHz */
static struct clock_freq_config pcie5_phy_rchng_cfg[] = {
	{
		.hz  = 100 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(6),
	},
};

enum cb_err clock_configure_pcie(void)
{
	enum cb_err ret;

	/* Configure the RCG to 100 MHz */
	ret = clock_configure(&gcc->pcie_5.phy_rchng_rcg, pcie5_phy_rchng_cfg,
			      100 * MHz, ARRAY_SIZE(pcie5_phy_rchng_cfg));
	if (ret != CB_SUCCESS) {
		printk(BIOS_ERR, "%s: failed to configure PCIE5 PHY RCHNG clock\n",
		       __func__);
	}

	return ret;
}
