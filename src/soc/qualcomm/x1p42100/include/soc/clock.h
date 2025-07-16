/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <types.h>
#include <soc/clock_common.h>

#ifndef __SOC_QUALCOMM_X1P42100_CLOCK_H__
#define __SOC_QUALCOMM_X1P42100_CLOCK_H__

#define SRC_XO_HZ		(19200 * KHz)

#define GPLL0_EVEN_HZ		(300 * MHz)
#define GPLL0_MAIN_HZ		(600 * MHz)
#define CLK_100MHZ		(100 * MHz)

#define PCIE_PHY_RCHNG_FREQ CLK_100MHZ

#define QUPV3_WRAP0_CLK_ENA_S(idx)		(10 + idx)
#define QUPV3_WRAP1_CLK_ENA_S(idx)		(22 + idx)
#define QUPV3_WRAP2_CLK_ENA_S(idx)		(4 + idx)

#define QUPV3_WRAP1_SE7_CLK_ENA			16
#define QUPV3_WRAP2_SE7_CLK_ENA			17

enum clk_ctl_gpll_user_ctl_x1p42100 {
	PLL_PLLOUT_MAIN_SHFT_X1P42100 = 0,
	PLL_PLLOUT_EVEN_SHFT_X1P42100 = 1,
	PLL_PLLOUT_ODD_SHFT_X1P42100 = 2,
	PLL_POST_DIV_EVEN_SHFT_X1P42100 = 10,
	PLL_POST_DIV_ODD_SHFT_X1P42100 = 14,
};

enum clk_pll_src {
	SRC_XO_19_2MHZ = 0,
	SRC_GPLL0_MAIN_600MHZ = 1,
	SRC_GPLL9_MAIN_808MHZ = 2,
	SRC_GCC_DISP_GPLL0_CLK = 4,
	SRC_GPLL10_MAIN_384MHZ = 5,
	SRC_GPLL0_EVEN_300MHZ = 6,
};

enum apcs_branch_en_vote {
	QUPV3_WRAP_0_M_AHB_CLK_ENA = 6,
	QUPV3_WRAP_0_S_AHB_CLK_ENA = 7,
	QUPV3_WRAP0_CORE_CLK_ENA = 8,
	QUPV3_WRAP0_CORE_2X_CLK_ENA = 9,
	QUPV3_WRAP1_CORE_2X_CLK_ENA = 18,
	QUPV3_WRAP1_CORE_CLK_ENA = 19,
	QUPV3_WRAP_1_M_AHB_CLK_ENA = 20,
	QUPV3_WRAP_1_S_AHB_CLK_ENA = 21,
	QUPV3_WRAP2_CORE_2X_CLK_ENA = 3,
	QUPV3_WRAP2_CORE_CLK_ENA = 0,
	QUPV3_WRAP_2_M_AHB_CLK_ENA = 2,
	QUPV3_WRAP_2_S_AHB_CLK_ENA = 1,
	ANOC_PCIE_AT_CLK_ENA = 11,
	ANOC_PCIE_QOSGEN_EXTREF_CLK_ENA = 13,
	DDRSS_PCIE_SF_QTB_CLK_ENA = 19,
	CNOC_PCIE_NORTH_SF_AXI_CLK_ENA = 6,
	CNOC_PCIE_SOUTH_SF_AXI_CLK_ENA = 12,
	NOC_PCIE_DCD_XO_CLK_ENA = 24,
	NOC_PCIE_SOUTH_DCD_XO_CLK_ENA = 25,
	NOC_PCIE_NORTH_DCD_XO_CLK_ENA = 29,
	CFG_NOC_PCIE_ANOC_AHB_CLK_ENA = 20,
	CFG_NOC_PCIE_ANOC_NORTH_AHB_CLK_ENA = 22,
	CFG_NOC_PCIE_ANOC_SOUTH_AHB_CLK_ENA = 20,
	AGGRE_NOC_PCIE_NORTH_AXI_CLK_ENA = 19,
	ANOC_PCIE_PWRCTL_CLK_ENA = 31,
	AGGRE_NOC_PCIE_SOUTH_AXI_CLK_ENA = 13,
	AGGRE_NOC_PCIE_HS_AXI_CLK_ENA = 14,
	AGGRE_NOC_PCIE_HS_NORTH_AXI_CLK_ENA = 15,
	AGGRE_NOC_PCIE_HS_SOUTH_AXI_CLK_ENA = 16,
	CNOC_PCIE_NORTH_SF_TUNNEL_AXI_CLK_ENA = 14,
	CNOC_PCIE_SOUTH_SF_TUNNEL_AXI_CLK_ENA = 15,
	PCIE_6A_SLV_Q2A_AXI_CLK_ENA = 20,
	PCIE_6A_SLV_AXI_CLK_ENA = 21,
	PCIE_6A_MSTR_AXI_CLK_ENA = 22,
	PCIE_6A_CFG_AHB_CLK_ENA = 23,
	PCIE_6A_AUX_CLK_ENA = 24,
	PCIE_6A_PHY_AUX_CLK_ENA = 25,
	PCIE_6A_PIPE_CLK_ENA = 26,
	PCIE_6A_PHY_RCHNG_CLK_ENA = 27,
	PCIE_6A_PIPEDIV2_CLK_ENA = 28,
	NO_VOTE_BIT = -1,
};

struct x1p42100_gpll {
	u32 mode;
	u32 opmode;
	u32 state;
	u32 status;
	u32 l;
	u32 alpha;
	u32 user_ctl;
	u32 user_ctl_u;
	u32 config_ctl;
	u32 config_ctl_u;
	u32 config_ctl_u1;
};

struct x1p42100_pcie_noc {
	u32 anoc_pcie_at_cbcr;
	u32 anoc_pcie_tsctr_cbcr;
	u32 anoc_pcie_qosgen_extref_cbcr;
	u32 ddrss_pcie_sf_qtb_cbcr;
	u32 cnoc_pcie_north_sf_axi_cbcr;
	u32 cnoc_pcie_south_sf_axi_cbcr;
	u32 noc_pcie_north_dcd_xo_cbcr;
	u32 noc_pcie_dcd_xo_cbcr;
	u32 noc_pcie_south_dcd_xo_cbcr;
	u32 cfg_noc_pcie_anoc_ahb_cbcr;
	u32 cfg_noc_pcie_anoc_north_ahb_cbcr;
	u32 cfg_noc_pcie_anoc_south_ahb_cbcr;
	u32 aggre_noc_pcie_north_axi_cbcr;
	u32 anoc_pcie_pwrctl_cbcr;
	u32 aggre_noc_pcie_south_axi_cbcr;
	u32 aggre_noc_pcie_hs_axi_cbcr;
	u32 aggre_noc_pcie_hs_north_axi_cbcr;
	u32 aggre_noc_pcie_hs_south_axi_cbcr;
	u32 cnoc_pcie_north_sf_tunnel_axi_cbcr;
	u32 cnoc_pcie_south_sf_tunnel_axi_cbcr;
};

struct x1p42100_pcie {
	u32 bcr;
	u32 gdscr;
	u8 _res0[0x131018 - 0x131008];
	u32 slv_q2a_axi_cbcr;
	u32 slv_axi_cbcr;
	u8 _res1[0x131028 - 0x131020];
	u32 mstr_axi_cbcr;
	u8 _res2[0x131034 - 0x13102c];
	u32 cfg_ahb_cbcr;
	u32 aux_cbcr;
	u8 _res3[0x131044 - 0x13103c];
	u32 phy_aux_cbcr;
	u8 _res4[0x131050 - 0x131048];
	u32 pipe_cbcr;
	u8 _res5[0x13105c - 0x131054];
	u32 phy_rchng_cbcr;
	u32 pipediv2_cbcr;
	u8 _res6[0x131070 - 0x131064];
	struct clock_rcg phy_rchng_rcg;
	u8 _res7[0x131088 - 0x131078];
	u32 pipe_muxr;
};

check_member(x1p42100_pcie, slv_q2a_axi_cbcr, 0x18);
check_member(x1p42100_pcie, mstr_axi_cbcr, 0x28);
check_member(x1p42100_pcie, cfg_ahb_cbcr, 0x34);
check_member(x1p42100_pcie, phy_aux_cbcr, 0x44);
check_member(x1p42100_pcie, pipe_cbcr, 0x50);
check_member(x1p42100_pcie, phy_rchng_cbcr, 0x5c);
check_member(x1p42100_pcie, phy_rchng_rcg, 0x70);
check_member(x1p42100_pcie, pipe_muxr, 0x88);

struct x1p42100_gcc {
	struct x1p42100_gpll gpll0;
	u8 _res0[0x10004 - 0x0002c];
	struct x1p42100_pcie_noc pcie_noc;
	u8 _res1[0x18004 - 0x10054];

	struct qupv3_clock qup_wrap1_s[8];
	u8 _res2[0x1e004 - 0x189c4];
	struct qupv3_clock qup_wrap2_s[8];
	u8 _res3[0x23000 - 0x1e9c4];
	u32 qup_wrap0_m_ahb_cbcr;
	u32 qup_wrap0_s_ahb_cbcr;
	u32 qup_wrap0_core_cbcr;
	u8 _res4[0x23014 - 0x2300c];
	u32 qup_wrap0_core_cdivr;
	u32 qup_wrap0_core_2x_cbcr;
	u8 _res5[0x23024 - 0x2301c];
	struct clock_rcg qup_wrap0_core_2x;
	u8 _res6[0x23150 - 0x2302c];
	u32 qup_wrap1_m_ahb_cbcr;
	u32 qup_wrap1_s_ahb_cbcr;
	u32 qup_wrap1_core_cbcr;
	u8 _res7[0x23164 - 0x2315c];
	u32 qup_wrap1_core_cdivr;
	u32 qup_wrap1_core_2x_cbcr;
	u8 _res8[0x23174 - 0x2316c];
	struct clock_rcg qup_wrap1_core_2x;
	u8 _res9[0x232a0 - 0x2317c];
	u32 qup_wrap2_m_ahb_cbcr;
	u32 qup_wrap2_s_ahb_cbcr;
	u32 qup_wrap2_core_cbcr;
	u8 _res10[0x232b4 - 0x232ac];
	u32 qup_wrap2_core_cdivr;
	u32 qup_wrap2_core_2x_cbcr;
	u8 _res11[0x232c4 - 0x232bc];
	struct clock_rcg qup_wrap2_core_2x;
	u8 _res12[0x31000 - 0x232cc];
	struct x1p42100_pcie pcie_6a;
	u8  res13[0x42004-0x3108c];
	struct qupv3_clock qup_wrap0_s[8];
	u8 _res14[0x4b000 - 0x429c4];
	u32 qspi_bcr;
	u32 qspi_cnoc_ahb_cbcr;
	u32 qspi_core_cbcr;
	struct clock_rcg qspi_core;
	u8 _res15[0x52000 - 0x4b014];
	u32 apcs_clk_br_en;
	u8 _res16[0x52008 - 0x52004];
	u32 apcs_clk_br_en1;
	u8 _res17[0x52010 - 0x5200c];
	u32 apcs_clk_br_en2;
	u8 _res18[0x52018 - 0x52014];
	u32 apcs_clk_br_en3;
	u8 _res19[0x52020 - 0x5201c];
	u32 apcs_clk_br_en4;
	u8 _res20[0x52028 - 0x52024];
	u32 apcs_clk_br_en5;
	u8 _res21[0x52030 - 0x5202c];
	u32 apcs_pll_br_en;
	u8 _res22[0x8e000 - 0x52034];
	u32 pcie_6_phy_gdscr;
	u8 _res23[0xac01c - 0x8e004];
	u32 pcie_6a_phy_bcr;
};

check_member(x1p42100_gcc, qup_wrap1_s, 0x18004);
check_member(x1p42100_gcc, qup_wrap2_s, 0x1e004);
check_member(x1p42100_gcc, qup_wrap0_m_ahb_cbcr, 0x23000);
check_member(x1p42100_gcc, qup_wrap0_core_cdivr, 0x23014);
check_member(x1p42100_gcc, qup_wrap0_core_2x, 0x23024);
check_member(x1p42100_gcc, qup_wrap1_m_ahb_cbcr, 0x23150);
check_member(x1p42100_gcc, qup_wrap1_core_cdivr, 0x23164);
check_member(x1p42100_gcc, qup_wrap1_core_2x, 0x23174);
check_member(x1p42100_gcc, qup_wrap2_m_ahb_cbcr, 0x232a0);
check_member(x1p42100_gcc, qup_wrap2_core_cdivr, 0x232b4);
check_member(x1p42100_gcc, qup_wrap2_core_2x, 0x232c4);
check_member(x1p42100_gcc, qup_wrap0_s, 0x42004);
check_member(x1p42100_gcc, qspi_bcr, 0x4b000);
check_member(x1p42100_gcc, apcs_clk_br_en, 0x52000);
check_member(x1p42100_gcc, apcs_clk_br_en1, 0x52008);
check_member(x1p42100_gcc, apcs_clk_br_en2, 0x52010);
check_member(x1p42100_gcc, apcs_clk_br_en3, 0x52018);
check_member(x1p42100_gcc, apcs_clk_br_en4, 0x52020);
check_member(x1p42100_gcc, apcs_clk_br_en5, 0x52028);
check_member(x1p42100_gcc, apcs_pll_br_en, 0x52030);
check_member(x1p42100_gcc, pcie_6_phy_gdscr, 0x8e000);
check_member(x1p42100_gcc, pcie_6a_phy_bcr, 0xac01c);

enum clk_qup {
	QUP_WRAP0_S0,
	QUP_WRAP0_S1,
	QUP_WRAP0_S2,
	QUP_WRAP0_S3,
	QUP_WRAP0_S4,
	QUP_WRAP0_S5,
	QUP_WRAP0_S6,
	QUP_WRAP0_S7,
	QUP_WRAP1_S0,
	QUP_WRAP1_S1,
	QUP_WRAP1_S2,
	QUP_WRAP1_S3,
	QUP_WRAP1_S4,
	QUP_WRAP1_S5,
	QUP_WRAP1_S6,
	QUP_WRAP1_S7,
	QUP_WRAP2_S0,
	QUP_WRAP2_S1,
	QUP_WRAP2_S2,
	QUP_WRAP2_S3,
	QUP_WRAP2_S4,
	QUP_WRAP2_S5,
	QUP_WRAP2_S6,
	QUP_WRAP2_S7,
};

struct pcie {
	uint32_t *gdscr;
	uint32_t *clk;
	uint32_t *clk_br_en;
	int vote_bit;
};

enum clk_gdsc {
	PCIE_6A_GDSC,
	PCIE_6_PHY_GDSC,
	MAX_GDSC
};

enum clk_pcie_src_sel {
	PCIE_6A_PIPE_SRC_SEL = 0,
	PCIE_6A_XO_SRC_SEL = 2,
};

enum clk_pcie {
	ANOC_PCIE_AT_CBCR = 1,
	ANOC_PCIE_QOSGEN_EXTREF_CBCR,
	DDRSS_PCIE_SF_QTB_CBCR,
	CNOC_PCIE_NORTH_SF_AXI_CBCR,
	CNOC_PCIE_SOUTH_SF_AXI_CBCR,
	NOC_PCIE_DCD_XO_CBCR,
	NOC_PCIE_SOUTH_DCD_XO_CBCR,
	NOC_PCIE_NORTH_DCD_XO_CBCR,
	CFG_NOC_PCIE_ANOC_AHB_CBCR,
	CFG_NOC_PCIE_ANOC_NORTH_AHB_CBCR,
	CFG_NOC_PCIE_ANOC_SOUTH_AHB_CBCR,
	AGGRE_NOC_PCIE_NORTH_AXI_CBCR,
	ANOC_PCIE_PWRCTL_CBCR,
	AGGRE_NOC_PCIE_SOUTH_AXI_CBCR,
	AGGRE_NOC_PCIE_HS_AXI_CBCR,
	AGGRE_NOC_PCIE_HS_NORTH_AXI_CBCR,
	AGGRE_NOC_PCIE_HS_SOUTH_AXI_CBCR,
	CNOC_PCIE_NORTH_SF_TUNNEL_AXI_CBCR,
	CNOC_PCIE_SOUTH_SF_TUNNEL_AXI_CBCR,
	PCIE_6A_SLV_Q2A_AXI_CLK,
	PCIE_6A_SLV_AXI_CLK,
	PCIE_6A_MSTR_AXI_CLK,
	PCIE_6A_CFG_AHB_CLK,
	PCIE_6A_AUX_CLK,
	PCIE_6A_PHY_AUX_CLK,
	PCIE_6A_PHY_RCHNG_CLK,
	PCIE_6A_PIPE_CLK,
	PCIE_6A_PIPEDIV2_CLK,
	PCIE_6A_PIPE_MUXR,
	PCIE_CLK_COUNT,
};
enum subsystem_reset {
	AOP_RESET_SHFT,
	CORE_SW_RESET,
};

/* TODO: update as per datasheet */
void clock_init(void);
void clock_configure_qspi(uint32_t hz);
void clock_enable_qup(int qup);
void clock_configure_dfsr(int qup);
void clock_configure_pcie(void);
enum cb_err clock_enable_gdsc(enum clk_gdsc gdsc_type);
enum cb_err clock_enable_pcie(enum clk_pcie clk_type);
enum cb_err clock_configure_mux(enum clk_pcie clk_type, u32 src_type);

/* Subsystem Reset */
static struct aoss *const aoss = (void *)AOSS_CC_BASE;
static struct x1p42100_gcc *const gcc = (void *)GCC_BASE;

#endif	// __SOC_QUALCOMM_X1P42100_CLOCK_H__
