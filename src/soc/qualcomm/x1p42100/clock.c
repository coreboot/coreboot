/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <types.h>
#include <stdbool.h>

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

static u32 *usb_gdsc[MAX_USB_GDSC] = {
	[USB30_MP_GDSC] = &gcc->gcc_usb30_mp_gdscr,
	[USB3_SS0_PHY_GDSC] = &gcc->gcc_usb3_mp_ss0_phy_gdscr,
	[USB3_SS1_PHY_GDSC] = &gcc->gcc_usb3_mp_ss1_phy_gdscr,
	[USB30_PRIM_GDSC] = &gcc->gcc_usb30_prim_gdscr,
	[USB30_SEC_GDSC] = &gcc->gcc_usb30_sec_gdscr,
};

static u32 *usb_mp_cbcr[USB_CLK_COUNT] = {
	[USB30_MP_MASTER_CBCR] = &gcc->gcc_usb30_mp_master_cbcr,
	[USB30_MP_SLEEP_CBCR] = &gcc->gcc_usb30_mp_sleep_cbcr,
	[USB30_MP_MOCK_UTMI_CBCR] = &gcc->gcc_usb30_mp_mock_utmi_cbcr,
	[USB3_MP_PHY_AUX_CBCR] = &gcc->gcc_usb3_mp_phy_aux_cbcr,
	[USB3_MP_PHY_COM_AUX_CBCR] = &gcc->gcc_usb3_mp_phy_com_aux_cbcr,
	[USB3_MP_PHY_PIPE_0_CBCR] = &gcc->gcc_usb3_mp_phy_pipe_0_cbcr,
	[USB3_MP_PHY_PIPE_1_CBCR] = &gcc->gcc_usb3_mp_phy_pipe_1_cbcr,
	[CFG_NOC_USB3_MP_AXI_CBCR] = &gcc->gcc_cfg_noc_usb3_mp_axi_cbcr,
	[AGGRE_USB3_MP_AXI_CBCR] = &gcc->gcc_aggre_usb3_mp_axi_cbcr,
	[SYS_NOC_USB_AXI_CBCR] = &gcc->gcc_sys_noc_usb_axi_cbcr,
	[CFG_NOC_USB_ANOC_NORTH_AHB_CBCR] = &gcc->gcc_cfg_noc_usb_anoc_north_ahb_cbcr,
	[CFG_NOC_USB_ANOC_SOUTH_AHB_CBCR] = &gcc->gcc_cfg_noc_usb_anoc_south_ahb_cbcr,
	[AGGRE_USB_NOC_AXI_CBCR] = &gcc->gcc_aggre_usb_noc_axi_cbcr,
	[AGGRE_NOC_USB_SOUTH_AXI_CBCR] = &gcc->gcc_aggre_noc_usb_south_axi_cbcr,
	[AGGRE_NOC_USB_NORTH_AXI_CBCR] = &gcc->gcc_aggre_noc_usb_north_axi_cbcr,
};

static u32 *usb_prim_cbcr[USB_PRIM_CLK_COUNT] = {
	[USB_PRIM_SYS_NOC_USB_AXI_CBCR] = &gcc->gcc_sys_noc_usb_axi_cbcr,
	[USB_PRIM_CFG_NOC_USB_ANOC_AHB_CBCR] = &gcc->gcc_cfg_noc_usb_anoc_ahb_cbcr,
	[USB_PRIM_CFG_NOC_USB_ANOC_NORTH_AHB_CBCR] = &gcc->gcc_cfg_noc_usb_anoc_north_ahb_cbcr,
	[USB_PRIM_CFG_NOC_USB_ANOC_SOUTH_AHB_CBCR] = &gcc->gcc_cfg_noc_usb_anoc_south_ahb_cbcr,
	[USB_PRIM_AGGRE_USB_NOC_AXI_CBCR] = &gcc->gcc_aggre_usb_noc_axi_cbcr,
	[USB_PRIM_AGGRE_NOC_USB_SOUTH_AXI_CBCR] = &gcc->gcc_aggre_noc_usb_south_axi_cbcr,
	[USB_PRIM_AGGRE_NOC_USB_NORTH_AXI_CBCR] = &gcc->gcc_aggre_noc_usb_north_axi_cbcr,
	[USB_PRIM_USB30_PRIM_MASTER_CBCR] = &gcc->gcc_usb30_prim_master_cbcr,
	[USB_PRIM_USB30_PRIM_SLEEP_CBCR] = &gcc->gcc_usb30_prim_sleep_cbcr,
	[USB_PRIM_USB30_PRIM_MOCK_UTMI_CBCR] = &gcc->gcc_usb30_prim_mock_utmi_cbcr,
	[USB_PRIM_USB3_PRIM_PHY_AUX_CBCR] = &gcc->gcc_usb3_prim_phy_aux_cbcr,
	[USB_PRIM_USB3_PRIM_PHY_COM_AUX_CBCR] = &gcc->gcc_usb3_prim_phy_com_aux_cbcr,
	[USB_PRIM_USB3_PRIM_PHY_PIPE_CBCR] = &gcc->gcc_usb3_prim_phy_pipe_cbcr,
	[USB_PRIM_CFG_NOC_USB3_PRIM_AXI_CBCR] = &gcc->gcc_cfg_noc_usb3_prim_axi_cbcr,
	[USB_PRIM_AGGRE_USB3_PRIM_AXI_CBCR] = &gcc->gcc_aggre_usb3_prim_axi_cbcr,
};

static u32 *usb_sec_cbcr[USB_SEC_CLK_COUNT] = {
	[USB_SEC_CFG_NOC_USB3_SEC_AXI_CBCR] = &gcc->gcc_cfg_noc_usb3_sec_axi_cbcr,
	[USB_SEC_AGGRE_USB3_SEC_AXI_CBCR] = &gcc->gcc_aggre_usb3_sec_axi_cbcr,
	[USB_SEC_USB30_SEC_MASTER_CBCR] = &gcc->gcc_usb30_sec_master_cbcr,
	[USB_SEC_USB30_SEC_SLEEP_CBCR] = &gcc->gcc_usb30_sec_sleep_cbcr,
	[USB_SEC_USB30_SEC_MOCK_UTMI_CBCR] = &gcc->gcc_usb30_sec_mock_utmi_cbcr,
	[USB_SEC_USB3_SEC_PHY_AUX_CBCR] = &gcc->gcc_usb3_sec_phy_aux_cbcr,
	[USB_SEC_USB3_SEC_PHY_COM_AUX_CBCR] = &gcc->gcc_usb3_sec_phy_com_aux_cbcr,
	[USB_SEC_USB3_SEC_PHY_PIPE_CBCR] = &gcc->gcc_usb3_sec_phy_pipe_cbcr,
	[USB_SEC_AGGRE_USB_NOC_AXI_CBCR] = &gcc->gcc_aggre_usb_noc_axi_cbcr,
	[USB_SEC_AGGRE_NOC_USB_NORTH_AXI_CBCR] = &gcc->gcc_aggre_noc_usb_north_axi_cbcr,
	[USB_SEC_AGGRE_NOC_USB_SOUTH_AXI_CBCR] = &gcc->gcc_aggre_noc_usb_south_axi_cbcr,
	[USB_SEC_SYS_NOC_USB_AXI_CBCR] = &gcc->gcc_sys_noc_usb_axi_cbcr,
};

static u32 *disp_gdsc[MAX_DISP_GDSC] = {
	[DISP_CC_CORE_GDSC] = &disp_cc->mdss_core_gdscr,
};

static u32 *mdss_cbcr[MDSS_CLK_COUNT] = {
	[GCC_DISP_AHB_CBCR] = &gcc->gcc_disp_ahb_cbcr,
	[GCC_DISP_XO_CBCR] = &gcc->gcc_disp_xo_cbcr,
	[GCC_DISP_HF_AXI_CBCR] = &gcc->gcc_disp_hf_axi_cbcr,
	[DISP_CC_MDSS_AHB_CBCR] = &disp_cc->mdss_ahb_cbcr,
	[DISP_CC_MDSS_MDP_CBCR] = &disp_cc->mdss_mdp_cbcr,
	[DISP_CC_MDSS_VSYNC_CBCR] = &disp_cc->mdss_vsync_cbcr,
	[DISP_CC_MDSS_RSCC_AHB_CBCR] = &disp_cc->mdss_rscc_ahb_cbcr,
	[DISP_CC_MDSS_RSCC_VSYNC_CBCR] = &disp_cc->mdss_rscc_vsync_cbcr,
	[DISP_CC_XO_CBCR] = &disp_cc->xo_cbcr,
	[DISP_CC_MDSS_DPTX3_PIXEL0_CBCR] = &disp_cc->mdss_dptx3_pixel0_cbcr,
	[DISP_CC_MDSS_DPTX3_LINK_CBCR] = &disp_cc->mdss_dptx3_link_cbcr,
	[DISP_CC_MDSS_DPTX3_AUX_CBCR] = &disp_cc->mdss_dptx3_aux_cbcr,
	[DISP_CC_MDSS_DPTX3_LINK_INTF_CBCR] = &disp_cc->mdss_dptx3_link_intf_cbcr,
};

static struct clock_freq_config pcie_core_cfg[] = {
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(6),
	},
};

static struct clock_freq_config usb_core_cfg[] = {
	{
		.hz = CLK_200MHZ,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(3),
	},
};

void clock_configure_qspi(uint32_t hz)
{
	clock_configure(&gcc->qspi_core, qspi_core_cfg, hz, ARRAY_SIZE(qspi_core_cfg));
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
	struct x1p42100_qupv3_wrap *wrap;
	void *se_clk_array[8];

	/* Determine which wrap based on qup value */
	if (qup <= QUP_WRAP0_S7)
		wrap = qup_wrap0_clk;
	else if (qup <= QUP_WRAP1_S7)
		wrap = qup_wrap1_clk;
	else
		wrap = qup_wrap2_clk;

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
	u32 *cbcr;

	if (qup <= QUP_WRAP0_S7) {
		clk_en_off = QUPV3_WRAP0_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en4;
	} else if (qup <= QUP_WRAP1_S7) {
		clk_en_off = (qup == QUP_WRAP1_S7) ? QUPV3_WRAP1_SE7_CLK_ENA : QUPV3_WRAP1_CLK_ENA_S(s);
		clk_br_en_ptr = (qup == QUP_WRAP1_S7) ? &gcc->apcs_clk_br_en2 : &gcc->apcs_clk_br_en1;
	} else {
		clk_en_off = (qup == QUP_WRAP2_S7) ? QUPV3_WRAP2_SE7_CLK_ENA : QUPV3_WRAP2_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en2;
	}

	if (s == 2 || s == 3)
		cbcr = &((struct qupv3_clock_v2 *)get_qup_se_clk(qup, s))->cbcr;
	else
		cbcr = &((struct qupv3_clock *)get_qup_se_clk(qup, s))->cbcr;

	/* Only call if a valid pointer was assigned */
	if (clk_br_en_ptr)
		clock_enable_vote(cbcr, clk_br_en_ptr, clk_en_off);
}

/* Clock Root clock Generator with DFS Operations */
void clock_configure_dfsr_table_x1p42100(int qup, struct clock_freq_config *clk_cfg,
	uint32_t num_perfs)
{
	unsigned int idx;
	int s = get_qup_se_index(qup);
	uint32_t reg_val;
	struct clock_rcg_dfsr *dfsr_clk;

	/* Validate s value is within bounds */
	if (s > 7) {
		printk(BIOS_ERR, "Invalid QUP index: s=%d\n", s);
		return;
	}

	/* Get DFSR clock pointer - cast based on SE type
	 * (s2, s3 are qupv3_clock_v2, others are qupv3_clock)
	 */
	if (s == 2 || s == 3)
		dfsr_clk = &((struct qupv3_clock_v2 *)get_qup_se_clk(qup, s))->dfsr_clk;
	else
		dfsr_clk = &((struct qupv3_clock *)get_qup_se_clk(qup, s))->dfsr_clk;

	clrsetbits32(&dfsr_clk->cmd_dfsr,
			BIT(CLK_CTL_CMD_RCG_SW_CTL_SHFT),
			BIT(CLK_CTL_CMD_DFSR_SHFT));

	for (idx = 0; idx < num_perfs; idx++) {
		reg_val = (clk_cfg[idx].src << CLK_CTL_CFG_SRC_SEL_SHFT) |
			(clk_cfg[idx].div << CLK_CTL_CFG_SRC_DIV_SHFT);

		write32(&dfsr_clk->perf_dfsr[idx], reg_val);

		if (clk_cfg[idx].m == 0)
			continue;

		setbits32(&dfsr_clk->perf_dfsr[idx], RCG_MODE_DUAL_EDGE << CLK_CTL_CFG_MODE_SHFT);

		reg_val = clk_cfg[idx].m & CLK_CTL_RCG_MND_BMSK;
		write32(&dfsr_clk->perf_m_dfsr[idx], reg_val);

		reg_val = ~(clk_cfg[idx].n - clk_cfg[idx].m) & CLK_CTL_RCG_MND_BMSK;
		write32(&dfsr_clk->perf_n_dfsr[idx], reg_val);

		reg_val = ~(clk_cfg[idx].d_2) & CLK_CTL_RCG_MND_BMSK;
		write32(&dfsr_clk->perf_d_dfsr[idx], reg_val);
	}
}

void clock_configure_dfsr(int qup)
{
	int s = get_qup_se_index(qup);
	uint32_t qupv3_cfg_size = (s < 4) ? (ARRAY_SIZE(qupv3_wrap_cfg) - 1) :
				ARRAY_SIZE(qupv3_wrap_cfg);

	clock_configure_dfsr_table_x1p42100(qup, qupv3_wrap_cfg, qupv3_cfg_size);
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

enum cb_err clock_enable_usb_gdsc(enum clk_usb_gdsc gdsc_type)
{
	if (gdsc_type >= MAX_USB_GDSC)
		return CB_ERR;

	return enable_and_poll_gdsc_status(usb_gdsc[gdsc_type]);
}

enum cb_err clock_enable_disp_gdsc(enum clk_disp_gdsc gdsc_type)
{
	if (gdsc_type >= MAX_DISP_GDSC)
		return CB_ERR;

	return enable_and_poll_gdsc_status(disp_gdsc[gdsc_type]);
}

enum cb_err usb_clock_enable(enum clk_usb clk_type)
{
	if (clk_type >= USB_CLK_COUNT)
		return CB_ERR;

	return clock_enable(usb_mp_cbcr[clk_type]);
}

void usb_clock_reset(enum clk_usb clk_type, bool assert)
{
	clock_reset(usb_mp_cbcr[clk_type], assert);
}

enum cb_err usb_prim_clock_enable(enum clk_usb_prim clk_type)
{
	if (clk_type >= USB_PRIM_CLK_COUNT) {
		printk(BIOS_ERR, "USB C0 clock enable failed: "
			"clock type %d out of range\n", clk_type);
		return CB_ERR;
	}

	return clock_enable(usb_prim_cbcr[clk_type]);
}

void usb_prim_clock_reset(enum clk_usb_prim clk_type, bool assert)
{
	clock_reset(usb_prim_cbcr[clk_type], assert);
}

enum cb_err usb_sec_clock_enable(enum clk_usb_sec clk_type)
{
	if (clk_type >= USB_SEC_CLK_COUNT) {
		printk(BIOS_ERR, "USB C1 clock enable failed: "
			"clock type %d out of range\n", clk_type);
		return CB_ERR;
	}

	return clock_enable(usb_sec_cbcr[clk_type]);
}

void usb_sec_clock_reset(enum clk_usb_sec clk_type, bool assert)
{
	clock_reset(usb_sec_cbcr[clk_type], assert);
}

void clock_configure_usb(void)
{
	clock_configure(&gcc->usb30_mp_master_rcg,
		usb_core_cfg, USB3_MASTER_CLK_MIN_FREQ_HZ, ARRAY_SIZE(usb_core_cfg));
	clock_configure(&gcc->usb30_prim_master_rcg,
		usb_core_cfg, USB3_MASTER_CLK_MIN_FREQ_HZ, ARRAY_SIZE(usb_core_cfg));
	clock_configure(&gcc->usb30_sec_master_rcg,
		usb_core_cfg, USB3_MASTER_CLK_MIN_FREQ_HZ, ARRAY_SIZE(usb_core_cfg));
}

enum cb_err usb_clock_configure_mux(enum clk_pipe_usb clk_type, u32 src_type)
{
	switch (clk_type) {
	case USB3_PHY_PIPE_0:
		write32(&gcc->gcc_usb3_mp_phy_pipe_0_muxr, src_type);
		break;
	case USB3_PHY_PIPE_1:
		write32(&gcc->gcc_usb3_mp_phy_pipe_1_muxr, src_type);
		break;
	case USB3_PRIM_PHY_PIPE:
		write32(&gcc->gcc_usb3_prim_phy_pipe_muxr, src_type);
		break;
	case USB3_SEC_PHY_PIPE:
		write32(&gcc->gcc_usb3_sec_phy_pipe_muxr, src_type);
		break;

	default:
		printk(BIOS_ERR, "Unhandled clk_type: %d, src_type: %u\n", clk_type, src_type);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err pll_init_and_set(struct x1p42100_ncc0_clock *ncc0, u32 l_val)
{
	int ret;
	struct alpha_pll_reg_val_config ncc0_pll_cfg = {0};

	setbits64p(NCC0_NCC_CMU_NCC_CLK_CFG, (BIT(PLLSWCTL) | BIT(OVRCKMUXPLLFASTCLK)));

	setbits64p(NCC0_NCC_CMU_NCC_PLL_CFG,
		(((LOCKTMOUTCNT_VAL & LOCKTMOUTCNT_BMSK) << LOCKTMOUTCNT) |
		((LOCKDEASSERTTMOUTCNT_VAL & LOCKDEASSERTTMOUTCNT_BMSK) << LOCKDEASSERTTMOUTCNT)));

	ncc0_pll_cfg.reg_config_ctl = &ncc0->pll0_config_ctl;

	ncc0_pll_cfg.config_ctl_val = (read32(ncc0_pll_cfg.reg_config_ctl) |
					PFA_MSB_VAL << PFA_MSB |
					RON_DEGEN_MULTIPLY_VAL << RON_DEGEN_MULTIPLY |
					FBC_ALPHA_CAL_VAL << FBC_ALPHA_CAL |
					PLL_COUNTER_ENABLE_VAL << PLL_COUNTER_ENABLE);

	ncc0_pll_cfg.reg_config_ctl_hi = &ncc0->pll0_config_ctl_u;
	ncc0_pll_cfg.config_ctl_hi_val = (read32(ncc0_pll_cfg.reg_config_ctl_hi) |
					CHP_REF_CUR_TRIM_VAL << CHP_REF_CUR_TRIM |
					ADC_KLSB_VALUE_VAL << ADC_KLSB_VALUE |
					ADC_KMSB_VALUE_VAL << ADC_KMSB_VALUE);

	ncc0_pll_cfg.reg_l = &ncc0->pll0_l;
	ncc0_pll_cfg.l_val = l_val;

	ncc0_pll_cfg.reg_alpha = &ncc0->pll0_alpha;
	ncc0_pll_cfg.alpha_val = 0x00;

	clock_configure_enable_gpll(&ncc0_pll_cfg, false, 0);

	ncc0_pll_cfg.reg_mode = &ncc0->pll0_mode;
	ncc0_pll_cfg.reg_opmode = &ncc0->pll0_opmode;
	ret =  zondaole_pll_enable(&ncc0_pll_cfg);
	if (ret != CB_SUCCESS)
		return CB_ERR;

	setbits64p(NCC0_NCC_CMU_NCC_CLK_CFG, BIT(SELCKMUXPLLFASTCLK));

	return CB_SUCCESS;
}

enum cb_err disp_pll_init_and_set(struct x1p42100_disp_pll_clock *disp_pll, u32 l_val, u32 alpha_val)
{
	int ret;
	struct alpha_pll_reg_val_config disp_pll_cfg = {0};

	disp_pll_cfg.reg_l = &disp_pll->pll_l;
	disp_pll_cfg.l_val = l_val;

	disp_pll_cfg.reg_alpha = &disp_pll->pll_alpha;
	disp_pll_cfg.alpha_val = alpha_val;

	disp_pll_cfg.reg_user_ctl = &disp_pll->pll_user_ctl;
	disp_pll_cfg.user_ctl_val = 0x1;

	clock_configure_enable_gpll(&disp_pll_cfg, false, 0);

	disp_pll_cfg.reg_mode = &disp_pll->pll_mode;
	disp_pll_cfg.reg_opmode = &disp_pll->pll_opmode;
	ret = lucidole_pll_enable(&disp_pll_cfg);
	if (ret != CB_SUCCESS)
		return CB_ERR;

	return CB_SUCCESS;
}

enum cb_err mdss_clock_enable(enum clk_mdss clk_type)
{
	if (clk_type >= MDSS_CLK_COUNT)
		return CB_ERR;

	/* Enable clock */
	return clock_enable(mdss_cbcr[clk_type]);
}

void enable_disp_clock_tcsr(void)
{
	write32(TCSR_GCC_EDP_CLKREF_EN_ADDR, 0x1);
}

static void speed_up_boot_cpu(void)
{
	/* 1363.2 MHz */
	if (!pll_init_and_set(apss_ncc0, L_VAL_1363P2MHz))
		printk(BIOS_DEBUG, "NCC Frequency bumped to 1.363(GHz)\n");
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
