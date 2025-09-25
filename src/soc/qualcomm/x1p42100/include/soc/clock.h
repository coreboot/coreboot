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
#define CLK_200MHZ		(200 * MHz)

/* CPU PLL*/
#define L_VAL_1363P2MHz		0x47

#define PCIE_PHY_RCHNG_FREQ CLK_100MHZ
#define USB3_MASTER_CLK_MIN_FREQ_HZ CLK_200MHZ

#define QUPV3_WRAP0_CLK_ENA_S(idx)		(10 + idx)
#define QUPV3_WRAP1_CLK_ENA_S(idx)		(22 + idx)
#define QUPV3_WRAP2_CLK_ENA_S(idx)		(4 + idx)

#define QUPV3_WRAP1_SE7_CLK_ENA			16
#define QUPV3_WRAP2_SE7_CLK_ENA			17

#define LOCKDEASSERTTMOUTCNT_BMSK		0xFFF
#define LOCKTMOUTCNT_BMSK			0xFFFF

#define LOCKDEASSERTTMOUTCNT_VAL		960LL
#define LOCKTMOUTCNT_VAL			960LL

#define PFA_MSB_VAL				2
#define RON_DEGEN_MULTIPLY_VAL			1
#define FBC_ALPHA_CAL_VAL			2
#define PLL_COUNTER_ENABLE_VAL			1

#define CHP_REF_CUR_TRIM_VAL			1
#define ADC_KLSB_VALUE_VAL			4
#define ADC_KMSB_VALUE_VAL			10

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

enum ncc0_cmu_clk_cfg_x1p42100 {
	OVRCKMUXPLLFASTCLK = 2,
	SELCKMUXPLLFASTCLK = 3,
	PLLSWCTL = 25,
};

enum ncc0_cmu_pll_cfg_x1p42100 {
	LOCKTMOUTCNT = 0,
	LOCKDEASSERTTMOUTCNT = 32,
};

enum ncc0_pll0_config_ctl {
	PFA_MSB = 10,
	RON_DEGEN_MULTIPLY = 18,
	FBC_ALPHA_CAL = 20,
	PLL_COUNTER_ENABLE = 27,
};

enum ncc0_pll0_config_ctl_u {
	CHP_REF_CUR_TRIM = 0,
	ADC_KLSB_VALUE = 13,
	ADC_KMSB_VALUE = 23,
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

struct x1p42100_ncc0_clock {
	u32 pll0_mode;
	u32 pll0_l;
	u32 pll0_alpha;
	u32 pll0_user_ctl;
	u32 pll0_user_ctl_u;
	u32 pll0_config_ctl;
	u32 pll0_config_ctl_u;
	u32 pll0_config_ctl_u1;
	u32 pll0_config_ctl_u2;
	u32 pll0_test_ctl;
	u32 pll0_test_ctl_u;
	u32 pll0_test_ctl_u1;
	u32 pll0_opmode;
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
	u8 _res1[0x12000 - 0x10054];
	u32 gcc_qusb2phy_prim_bcr;
	u32 gcc_qusb2phy_sec_bcr;
	u32 gcc_qusb2phy_tert_bcr;
	u32 qusb2phy_hs0_mp_bcr;
	u32 qusb2phy_hs1_mp_bcr;
	u8 _res2[0x17000 - 0x12014];
	u32 gcc_usb30_mp_bcr;
	u32 gcc_usb30_mp_gdscr;
	u8 _res3[0x17018 - 0x17008];
	u32 gcc_usb30_mp_master_cbcr;
	u8 _res4[0x17024 - 0x1701c];
	u32 gcc_usb30_mp_sleep_cbcr;
	u32 gcc_usb30_mp_mock_utmi_cbcr;
	struct clock_rcg usb30_mp_master_rcg;
	u8 _res5[0x17288 - 0x17034];
	u32 gcc_usb3_mp_phy_aux_cbcr;
	u32 gcc_usb3_mp_phy_com_aux_cbcr;
	u32 gcc_usb3_mp_phy_pipe_0_cbcr;
	u32 gcc_usb3_mp_phy_pipe_0_muxr;
	u32 gcc_usb3_mp_phy_pipe_1_cbcr;
	u32 gcc_usb3_mp_phy_pipe_1_muxr;
	u8 _res6[0x173cc - 0x172a0];
	u32 gcc_cfg_noc_usb3_mp_axi_cbcr;
	u32 gcc_aggre_usb3_mp_axi_cbcr;
	u8 _res7[0x18004 - 0x173d4];
	struct qupv3_clock qup_wrap1_s[8];
	u8 _res8[0x19000 - 0x189c4];
	u32 usb3_uniphy_mp0_bcr;
	u32 usb3uniphy_phy_mp0_bcr;
	u8 _res9[0x1900c - 0x19008];
	u32 gcc_usb3_mp_ss0_phy_gdscr;
	u8 _res10[0x1e004 - 0x19010];
	struct qupv3_clock qup_wrap2_s[8];
	u8 _res11[0x23000 - 0x1e9c4];
	u32 qup_wrap0_m_ahb_cbcr;
	u32 qup_wrap0_s_ahb_cbcr;
	u32 qup_wrap0_core_cbcr;
	u8 _res12[0x23014 - 0x2300c];
	u32 qup_wrap0_core_cdivr;
	u32 qup_wrap0_core_2x_cbcr;
	u8 _res13[0x23024 - 0x2301c];
	struct clock_rcg qup_wrap0_core_2x;
	u8 _res14[0x23150 - 0x2302c];
	u32 qup_wrap1_m_ahb_cbcr;
	u32 qup_wrap1_s_ahb_cbcr;
	u32 qup_wrap1_core_cbcr;
	u8 _res15[0x23164 - 0x2315c];
	u32 qup_wrap1_core_cdivr;
	u32 qup_wrap1_core_2x_cbcr;
	u8 _res16[0x23174 - 0x2316c];
	struct clock_rcg qup_wrap1_core_2x;
	u8 _res17[0x232a0 - 0x2317c];
	u32 qup_wrap2_m_ahb_cbcr;
	u32 qup_wrap2_s_ahb_cbcr;
	u32 qup_wrap2_core_cbcr;
	u8 _res18[0x232b4 - 0x232ac];
	u32 qup_wrap2_core_cdivr;
	u32 qup_wrap2_core_2x_cbcr;
	u8 _res19[0x232c4 - 0x232bc];
	struct clock_rcg qup_wrap2_core_2x;
	u8 _res20[0x2a000 - 0x232cc];
	u32 gcc_usb3_phy_sec_bcr;
	u32 gcc_usb3phy_phy_sec_bcr;
	u8 _res21[0x2a010 - 0x2a008];
	u32 gcc_usb4_1_dp0_phy_sec_bcr;
	u8 _res22[0x2d014 - 0x2a014];
	u32 gcc_sys_noc_usb_axi_cbcr;
	u8 _res23[0x2d024 - 0x2d018];
	u32 gcc_cfg_noc_usb_anoc_ahb_cbcr;
	u32 gcc_cfg_noc_usb_anoc_north_ahb_cbcr;
	u32 gcc_cfg_noc_usb_anoc_south_ahb_cbcr;
	u8 _res24[0x2d034 - 0x2d030];
	u32 gcc_aggre_usb_noc_axi_cbcr;
	u8 _res25[0x2d174 - 0x2d038];
	u32 gcc_aggre_noc_usb_south_axi_cbcr;
	u8 _res26[0x2d17c - 0x2d178];
	u32 gcc_aggre_noc_usb_north_axi_cbcr;
	u8 _res27[0x31000 - 0x2d180];
	struct x1p42100_pcie pcie_6a;
	u8  _res28[0x39000 - 0x3108c];
	u32 gcc_usb30_prim_bcr;
	u32 gcc_usb30_prim_gdscr;
	u8  _res29[0x39018 - 0x39008];
	u32 gcc_usb30_prim_master_cbcr;
	u8  _res30[0x39024 - 0x3901c];
	u32 gcc_usb30_prim_sleep_cbcr;
	u32 gcc_usb30_prim_mock_utmi_cbcr;
	struct clock_rcg usb30_prim_master_rcg;
	u8  _res31[0x39060 - 0x39034];
	u32 gcc_usb3_prim_phy_aux_cbcr;
	u32 gcc_usb3_prim_phy_com_aux_cbcr;
	u32 gcc_usb3_prim_phy_pipe_cbcr;
	u32 gcc_usb3_prim_phy_pipe_muxr;
	u32 gcc_usb4_mode_prim_phy_pipe_muxr;
	u8  _res32[0x3908c - 0x39074];
	u32 gcc_cfg_noc_usb3_prim_axi_cbcr;
	u32 gcc_aggre_usb3_prim_axi_cbcr;
	u8  _res33[0x42004 - 0x39094];
	struct qupv3_clock qup_wrap0_s[8];
	u8 _res34[0x4b000 - 0x429c4];
	u32 qspi_bcr;
	u32 qspi_cnoc_ahb_cbcr;
	u32 qspi_core_cbcr;
	struct clock_rcg qspi_core;
	u8 _res35[0x50000 - 0x4b014];
	u32 gcc_usb3_phy_prim_bcr;
	u32 gcc_usb3phy_phy_prim_bcr;
	u8 _res36[0x50010 - 0x50008];
	u32 gcc_usb4_0_dp0_phy_prim_bcr;
	u8 _res37[0x52000 - 0x50014];
	u32 apcs_clk_br_en;
	u8 _res38[0x52008 - 0x52004];
	u32 apcs_clk_br_en1;
	u8 _res39[0x52010 - 0x5200c];
	u32 apcs_clk_br_en2;
	u8 _res40[0x52018 - 0x52014];
	u32 apcs_clk_br_en3;
	u8 _res41[0x52020 - 0x5201c];
	u32 apcs_clk_br_en4;
	u8 _res42[0x52028 - 0x52024];
	u32 apcs_clk_br_en5;
	u8 _res43[0x52030 - 0x5202c];
	u32 apcs_pll_br_en;
	u8 _res44[0x54000 - 0x52034];
	u32 usb3_uniphy_mp1_bcr;
	u32 usb3uniphy_phy_mp1_bcr;
	u32 gcc_usb3_mp_ss1_phy_bcr;
	u32 gcc_usb3_mp_ss1_phy_gdscr;
	u8 _res45[0x8e000 - 0x54010];
	u32 pcie_6_phy_gdscr;
	u8 _res46[0xa1000 - 0x8e004];
	u32 gcc_usb30_sec_bcr;
	u32 gcc_usb30_sec_gdscr;
	u8 _res47[0xa1018 - 0xa1008];
	u32 gcc_usb30_sec_master_cbcr;
	u8 _res48[0xa1024 - 0xa101c];
	u32 gcc_usb30_sec_sleep_cbcr;
	u32 gcc_usb30_sec_mock_utmi_cbcr;
	struct clock_rcg usb30_sec_master_rcg;
	u8 _res49[0xa1060 - 0xa1034];
	u32 gcc_usb3_sec_phy_aux_cbcr;
	u32 gcc_usb3_sec_phy_com_aux_cbcr;
	u32 gcc_usb3_sec_phy_pipe_cbcr;
	u32 gcc_usb3_sec_phy_pipe_muxr;
	u8 _res50[0xa108c - 0xa1070];
	u32 gcc_cfg_noc_usb3_sec_axi_cbcr;
	u32 gcc_aggre_usb3_sec_axi_cbcr;
	u8 _res51[0xa2000 - 0xa1094];
	u32 gcc_usb30_tert_bcr;
	u32 gcc_usb30_tert_gdscr;
	u8 _res52[0xa2018 - 0xa2008];
	u32 gcc_usb30_tert_master_cbcr;
	u8 _res53[0xa2024 - 0xa201c];
	u32 gcc_usb30_tert_sleep_cbcr;
	u32 gcc_usb30_tert_mock_utmi_cbcr;
	u8 _res54[0xa2034 - 0xa202c];
	u32 gcc_usb30_tert_master_m;
	u32 gcc_usb30_tert_master_n;
	u32 gcc_usb30_tert_master_d;
	u8 _res55[0xa2060 - 0xa2040];
	u32 gcc_usb3_tert_phy_aux_cbcr;
	u32 gcc_usb3_tert_phy_com_aux_cbcr;
	u32 gcc_usb3_tert_phy_pipe_cbcr;
	u32 gcc_usb3_tert_phy_pipe_muxr;
	u8 _res56[0xa208c - 0xa2070];
	u32 gcc_cfg_noc_usb3_tert_axi_cbcr;
	u32 gcc_aggre_usb3_tert_axi_cbcr;
	u8 _res57[0xa3000 - 0xa2094];
	u32 gcc_usb3_phy_tert_bcr;
	u32 gcc_usb3phy_phy_tert_bcr;
	u8 _res58[0xa3010 - 0xa3008];
	u32 gcc_usb4_2_dp0_phy_tert_bcr;
	u8 _res59[0xac01c - 0xa3014];
	u32 pcie_6a_phy_bcr;
};

check_member(x1p42100_gcc, gcc_usb30_prim_bcr, 0x39000);
check_member(x1p42100_gcc, gcc_usb30_prim_master_cbcr, 0x39018);
check_member(x1p42100_gcc, gcc_cfg_noc_usb3_prim_axi_cbcr, 0x3908c);
check_member(x1p42100_gcc, gcc_usb30_sec_bcr, 0xa1000);
check_member(x1p42100_gcc, gcc_usb30_sec_master_cbcr, 0xa1018);
check_member(x1p42100_gcc, gcc_usb3_sec_phy_aux_cbcr, 0xa1060);
check_member(x1p42100_gcc, gcc_usb30_tert_bcr, 0xa2000);
check_member(x1p42100_gcc, gcc_qusb2phy_prim_bcr, 0x12000);
check_member(x1p42100_gcc, gcc_qusb2phy_sec_bcr, 0x12004);
check_member(x1p42100_gcc, gcc_qusb2phy_tert_bcr, 0x12008);
check_member(x1p42100_gcc, qusb2phy_hs1_mp_bcr, 0x12010);
check_member(x1p42100_gcc, gcc_usb3_phy_sec_bcr, 0x2a000);
check_member(x1p42100_gcc, gcc_usb3phy_phy_sec_bcr, 0x2a004);
check_member(x1p42100_gcc, gcc_usb4_1_dp0_phy_sec_bcr, 0x2a010);
check_member(x1p42100_gcc, gcc_usb3_phy_prim_bcr, 0x50000);
check_member(x1p42100_gcc, gcc_usb3phy_phy_prim_bcr, 0x50004);
check_member(x1p42100_gcc, gcc_usb4_0_dp0_phy_prim_bcr, 0x50010);
check_member(x1p42100_gcc, gcc_usb3_phy_tert_bcr, 0xa3000);
check_member(x1p42100_gcc, gcc_usb3phy_phy_tert_bcr, 0xa3004);
check_member(x1p42100_gcc, gcc_usb4_2_dp0_phy_tert_bcr, 0xa3010);
check_member(x1p42100_gcc, gcc_usb30_mp_bcr, 0x17000);
check_member(x1p42100_gcc, qup_wrap1_s, 0x18004);
check_member(x1p42100_gcc, gcc_usb30_mp_master_cbcr, 0x17018);
check_member(x1p42100_gcc, gcc_usb30_mp_sleep_cbcr, 0x17024);
check_member(x1p42100_gcc, gcc_usb3_mp_phy_aux_cbcr, 0x17288);
check_member(x1p42100_gcc, gcc_cfg_noc_usb3_mp_axi_cbcr, 0x173cc);
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
check_member(x1p42100_gcc, gcc_sys_noc_usb_axi_cbcr, 0x2d014);
check_member(x1p42100_gcc, gcc_cfg_noc_usb_anoc_north_ahb_cbcr, 0x2d028);
check_member(x1p42100_gcc, gcc_aggre_usb_noc_axi_cbcr, 0x2d034);
check_member(x1p42100_gcc, gcc_aggre_noc_usb_south_axi_cbcr, 0x2d174);
check_member(x1p42100_gcc, gcc_aggre_noc_usb_north_axi_cbcr, 0x2d17c);
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

enum clk_usb_gdsc {
	USB30_MP_GDSC,
	USB3_SS0_PHY_GDSC,
	USB3_SS1_PHY_GDSC,
	USB30_PRIM_GDSC,
	USB30_SEC_GDSC,
	MAX_USB_GDSC,
};
enum clk_usb {
	USB30_MP_MASTER_CBCR,
	USB30_MP_SLEEP_CBCR,
	USB30_MP_MOCK_UTMI_CBCR,
	USB3_MP_PHY_AUX_CBCR,
	USB3_MP_PHY_COM_AUX_CBCR,
	USB3_MP_PHY_PIPE_0_CBCR,
	USB3_MP_PHY_PIPE_1_CBCR,
	CFG_NOC_USB3_MP_AXI_CBCR,
	AGGRE_USB3_MP_AXI_CBCR,
	SYS_NOC_USB_AXI_CBCR,
	CFG_NOC_USB_ANOC_NORTH_AHB_CBCR,
	CFG_NOC_USB_ANOC_SOUTH_AHB_CBCR,
	AGGRE_USB_NOC_AXI_CBCR,
	AGGRE_NOC_USB_SOUTH_AXI_CBCR,
	AGGRE_NOC_USB_NORTH_AXI_CBCR,
	USB_CLK_COUNT,
};

enum clk_usb_prim {
	USB_PRIM_SYS_NOC_USB_AXI_CBCR,
	USB_PRIM_CFG_NOC_USB_ANOC_AHB_CBCR,
	USB_PRIM_CFG_NOC_USB_ANOC_NORTH_AHB_CBCR,
	USB_PRIM_CFG_NOC_USB_ANOC_SOUTH_AHB_CBCR,
	USB_PRIM_AGGRE_USB_NOC_AXI_CBCR,
	USB_PRIM_AGGRE_NOC_USB_SOUTH_AXI_CBCR,
	USB_PRIM_AGGRE_NOC_USB_NORTH_AXI_CBCR,
	USB_PRIM_USB30_PRIM_MASTER_CBCR,
	USB_PRIM_USB30_PRIM_SLEEP_CBCR,
	USB_PRIM_USB30_PRIM_MOCK_UTMI_CBCR,
	USB_PRIM_USB3_PRIM_PHY_AUX_CBCR,
	USB_PRIM_USB3_PRIM_PHY_COM_AUX_CBCR,
	USB_PRIM_USB3_PRIM_PHY_PIPE_CBCR,
	USB_PRIM_CFG_NOC_USB3_PRIM_AXI_CBCR,
	USB_PRIM_AGGRE_USB3_PRIM_AXI_CBCR,
	USB_PRIM_CLK_COUNT,
};

enum clk_usb_sec {
	USB_SEC_CFG_NOC_USB3_SEC_AXI_CBCR,
	USB_SEC_AGGRE_USB3_SEC_AXI_CBCR,
	USB_SEC_USB30_SEC_MASTER_CBCR,
	USB_SEC_USB30_SEC_SLEEP_CBCR,
	USB_SEC_USB30_SEC_MOCK_UTMI_CBCR,
	USB_SEC_USB3_SEC_PHY_AUX_CBCR,
	USB_SEC_USB3_SEC_PHY_COM_AUX_CBCR,
	USB_SEC_USB3_SEC_PHY_PIPE_CBCR,
	USB_SEC_AGGRE_USB_NOC_AXI_CBCR,
	USB_SEC_AGGRE_NOC_USB_NORTH_AXI_CBCR,
	USB_SEC_AGGRE_NOC_USB_SOUTH_AXI_CBCR,
	USB_SEC_SYS_NOC_USB_AXI_CBCR,
	USB_SEC_CLK_COUNT,
};

enum clk_pipe_usb {
	USB3_PHY_PIPE_0,
	USB3_PHY_PIPE_1,
	USB3_PRIM_PHY_PIPE,
	USB3_SEC_PHY_PIPE,
	USB3_PIPE_CLK_COUNT
};

enum clk_usb_phy_src_sel {
		USB_PHY_PIPE_SRC_SEL = 0,
		USB_PHY_XO_SRC_SEL = 2,
};

enum subsystem_reset {
	AOP_RESET_SHFT,
	CORE_SW_RESET,
};

void clock_init(void);
void clock_configure_qspi(uint32_t hz);
void clock_enable_qup(int qup);
void clock_configure_dfsr(int qup);
void clock_configure_pcie(void);
void clock_configure_usb(void);
enum cb_err clock_enable_gdsc(enum clk_gdsc gdsc_type);
enum cb_err clock_enable_pcie(enum clk_pcie clk_type);
enum cb_err clock_configure_mux(enum clk_pcie clk_type, u32 src_type);
enum cb_err usb_clock_configure_mux(enum clk_pipe_usb clk_type, u32 src_type);
enum cb_err usb_clock_enable(enum clk_usb clk_type);
enum cb_err clock_enable_usb_gdsc(enum clk_usb_gdsc gdsc_type);
enum cb_err usb_prim_clock_enable(enum clk_usb_prim clk_type);
enum cb_err usb_sec_clock_enable(enum clk_usb_sec clk_type);

void usb_clock_reset(enum clk_usb clk_type, bool assert);
void usb_prim_clock_reset(enum clk_usb_prim clk_type, bool assert);
void usb_sec_clock_reset(enum clk_usb_sec clk_type, bool assert);

/* Subsystem Reset */
static struct aoss *const aoss = (void *)AOSS_CC_BASE;
static struct x1p42100_gcc *const gcc = (void *)GCC_BASE;
static struct x1p42100_ncc0_clock *const apss_ncc0 = (void *)NCC0_BASE;

/* Does nothing */
#define clock_reset_aop() do {} while (0)
/* Does nothing */
#define clock_reset_shrm() do {} while (0)

#endif	// __SOC_QUALCOMM_X1P42100_CLOCK_H__
