/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <types.h>
#include <soc/clock_common.h>

#ifndef __SOC_QUALCOMM_CALYPSO_CLOCK_H__
#define __SOC_QUALCOMM_CALYPSO_CLOCK_H__

#define SRC_XO_HZ		(19200 * KHz)

#define GPLL0_EVEN_HZ           (300 * MHz)
#define GPLL0_MAIN_HZ           (600 * MHz)
#define CLK_100MHZ              (100 * MHz)
#define CLK_200MHZ              (200 * MHz)
#define CLK_400MHZ              (400 * MHz)
#define CLK_75MHZ               (75 * MHz)
#define CLK_575MHZ              (575 * MHz)
#define CLK_37_5MHZ             (37500 * KHz)

/* CPU PLL*/
#define L_VAL_2995P2MHz         0x9C
#define L_VAL_1363P2MHz         0x47
#define L_VAL_710P4MHz          0x25

#define QUPV3_WRAP0_CLK_ENA_S(idx)              (13 + idx)
#define QUPV3_WRAP1_CLK_ENA_S(idx)              (15 + idx)
#define QUPV3_WRAP2_CLK_ENA_S(idx)              (idx)
#define QUPV3_OOB_CLK_ENA_S(idx)                (6 + idx)

#define QUPV3_WRAP2_SE0_CLK_ENA                 30
#define QUPV3_WRAP2_SE1_CLK_ENA                 31

#define LOCKDEASSERTTMOUTCNT_BMSK               0xFFF
#define LOCKTMOUTCNT_BMSK                       0xFFFF

#define LOCKDEASSERTTMOUTCNT_VAL                960LL
#define LOCKTMOUTCNT_VAL                        960LL

#define PFA_MSB_VAL                             2
#define RON_DEGEN_MULTIPLY_VAL                  1
#define FBC_ALPHA_CAL_VAL                       2
#define PLL_COUNTER_ENABLE_VAL                  1

#define CHP_REF_CUR_TRIM_VAL                    1
#define ADC_KLSB_VALUE_VAL                      4
#define ADC_KMSB_VALUE_VAL                      10

/* TODO: update as per datasheet */
void clock_init(void);
void clock_configure_qspi(uint32_t hz);
void clock_enable_qup(int qup);
void clock_configure_dfsr(int qup);

/* Does nothing */
#define clock_reset_aop() do {} while (0)
/* Does nothing */
#define clock_reset_shrm() do {} while (0)

enum apcs_branch_en_vote {
	QUPV3_OOB_S_AHB_CLK_ENA = 3,
	QUPV3_OOB_CORE_CLK_ENA = 4,
	QUPV3_OOB_CORE_2X_CLK_ENA = 5,
	QUPV3_WRAP_0_M_AHB_CLK_ENA = 9,
	QUPV3_WRAP_0_S_AHB_CLK_ENA = 10,
	QUPV3_WRAP0_CORE_CLK_ENA = 11,
	QUPV3_WRAP0_CORE_2X_CLK_ENA = 12,
	QUPV3_WRAP1_CORE_2X_CLK_ENA = 14,
	QUPV3_WRAP1_CORE_CLK_ENA = 13,
	QUPV3_WRAP_1_M_AHB_CLK_ENA = 11,
	QUPV3_WRAP_1_S_AHB_CLK_ENA = 12,
	QUPV3_WRAP2_CORE_2X_CLK_ENA = 29,
	QUPV3_WRAP2_CORE_CLK_ENA = 28,
	QUPV3_WRAP_2_M_AHB_CLK_ENA = 26,
	QUPV3_WRAP_2_S_AHB_CLK_ENA = 27,
	/* VOTE_0 (apcs_clk_br_en) - SMMU/TCU/HSCNOC PCIe */
	HSCNOC_PCIE_SF_QTC_CLK_ENA = 24,
	HSCNOC_PCIE_SLAVE_SF_EAST_CLK_ENA = 25,
	HSCNOC_PCIE_SLAVE_SF_WEST_CLK_ENA = 26,
	TCU_PCIE_SF_QTC_CLK_ENA = 27,
	SMMU_PCIE_QTC_AT_CLK_ENA = 30,
	/* VOTE_1 (apcs_clk_br_en1) - AGGRE_NOC/ANOC/CNOC/PCIE_NOC */
	PCIE_NOC_PWRCTL_CLK_ENA = 7,
	PCIE_NOC_SF_CENTER_CLK_ENA = 8,
	PCIE_NOC_SLAVE_SF_EAST_CLK_ENA = 9,
	PCIE_NOC_SLAVE_SF_WEST_CLK_ENA = 10,
	CNOC_PCIE_WEST_TUNNEL_CLK_ENA = 14,
	CNOC_PCIE_EAST_TUNNEL_CLK_ENA = 15,
	ANOC_PCIE_PWRCTL_CLK_ENA = 26,
	AGGRE_NOC_PCIE_5_EAST_SF_AXI_CLK_ENA = 30,
	/* VOTE_2 (apcs_clk_br_en2) - TRACE_NOC/AGGRE_NOC_TUNNEL */
	AGGRE_NOC_PCIE_EAST_TUNNEL_AXI_CLK_ENA = 0,
	AGGRE_NOC_PCIE_WEST_TUNNEL_AXI_CLK_ENA = 1,
	TRACE_NOC_TCU_PCIE_QTC_AT_CLK_ENA = 8,
	TRACE_NOC_PCIE_3B_AT_CLK_ENA = 9,
	TRACE_NOC_PCIE_5_AT_CLK_ENA = 10,
	/* VOTE_6 (apcs_clk_br_en6) - PCIE_5 specific */
	QMIP_PCIE_5_AHB_CLK_ENA = 0,
	PCIE_5_SLV_Q2A_AXI_CLK_ENA = 1,
	PCIE_5_SLV_AXI_CLK_ENA = 2,
	PCIE_5_MSTR_AXI_CLK_ENA = 3,
	PCIE_5_AUX_CLK_ENA = 5,
	PCIE_5_PIPE_CLK_ENA = 6,
	PCIE_5_PIPE_DIV2_CLK_ENA = 7,
	PCIE_5_PHY_RCHNG_CLK_ENA = 8,
	PCIE_5_AT_CLK_ENA = 9,
	PCIE_PHY_5_AUX_CLK_ENA = 11,
	PCIE_5_CFG_AHB_CLK_ENA = 4,
	/* Direct CBCR enable (no vote register) */
	NO_VOTE_BIT = -1,
};

enum clk_pll_src {
	SRC_XO_19_2MHZ = 0,
	SRC_GPLL0_MAIN_600MHZ = 1,
	SRC_GPLL9_MAIN_808MHZ = 2,
	SRC_GCC_DISP_GPLL0_CLK = 4,
	SRC_GPLL10_MAIN_384MHZ = 5,
	SRC_GPLL0_EVEN_300MHZ = 6,
};

struct calypso_gpll {
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
	u32 test_ctl;
	u32 test_ctl_u;
	u32 ssc;
};

enum ncc0_cmu_clk_cfg_calypso {
	OVRCKMUXPLLFASTCLK = 2,
	SELCKMUXPLLFASTCLK = 3,
	PLLSWCTL = 25,
};

enum ncc0_cmu_pll_cfg_calypso {
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

struct calypso_ncc0_clock {
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

enum clk_ctl_gpll_user_ctl_calypso {
	PLL_PLLOUT_MAIN_SHFT_CALYPSO = 0,
	PLL_PLLOUT_EVEN_SHFT_CALYPSO = 1,
	PLL_PLLOUT_ODD_SHFT_CALYPSO = 2,
	PLL_POST_DIV_EVEN_SHFT_CALYPSO = 10,
	PLL_POST_DIV_ODD_SHFT_CALYPSO = 14,
};

/*
 * PCIE NOC register block
 */
struct calypso_pcie_noc {
	u8 _res0[0x268];
	u32 hscnoc_pcie_sf_qtc_cbcr;
	u8 _res1[0x270 - 0x26C];
	u32 hscnoc_pcie_slave_sf_east_cbcr;
	u8 _res2[0x278 - 0x274];
	u32 hscnoc_pcie_slave_sf_west_cbcr;
	u8 _res3[0x280 - 0x27C];
	u32 tcu_pcie_sf_qtc_cbcr;
	u8 _res4[0x2AC - 0x284];
	u32 pcie_noc_pwrctl_cbcr;
	u32 pcie_noc_sf_center_cbcr;
	u8 _res5[0x2B8 - 0x2B4];
	u32 pcie_noc_slave_sf_east_cbcr;
	u8 _res6[0x2C0 - 0x2BC];
	u32 pcie_noc_slave_sf_west_cbcr;
	u8 _res7[0x2DC - 0x2C4];
	u32 cnoc_pcie_west_tunnel_cbcr;
	u8 _res8[0x2E4 - 0x2E0];
	u32 cnoc_pcie_east_tunnel_cbcr;
	u8 _res9[0x300 - 0x2E8];
	u32 aggre_noc_pcie_east_tunnel_axi_cbcr;
	u32 aggre_noc_pcie_west_tunnel_axi_cbcr;
	u8 _res10[0x448 - 0x308];
	u32 trace_noc_pcie_3b_at_cbcr;
	u32 trace_noc_pcie_5_at_cbcr;
	u8 _res11[0x4E4 - 0x450];
	u32 pcie_link_xo_cbcr;
};
check_member(calypso_pcie_noc, hscnoc_pcie_sf_qtc_cbcr, 0x268);
check_member(calypso_pcie_noc, hscnoc_pcie_slave_sf_east_cbcr, 0x270);
check_member(calypso_pcie_noc, hscnoc_pcie_slave_sf_west_cbcr, 0x278);
check_member(calypso_pcie_noc, tcu_pcie_sf_qtc_cbcr, 0x280);
check_member(calypso_pcie_noc, pcie_noc_pwrctl_cbcr, 0x2AC);
check_member(calypso_pcie_noc, pcie_noc_sf_center_cbcr, 0x2B0);
check_member(calypso_pcie_noc, pcie_noc_slave_sf_east_cbcr, 0x2B8);
check_member(calypso_pcie_noc, pcie_noc_slave_sf_west_cbcr, 0x2C0);
check_member(calypso_pcie_noc, cnoc_pcie_west_tunnel_cbcr, 0x2DC);
check_member(calypso_pcie_noc, cnoc_pcie_east_tunnel_cbcr, 0x2E4);
check_member(calypso_pcie_noc, aggre_noc_pcie_east_tunnel_axi_cbcr, 0x300);
check_member(calypso_pcie_noc, aggre_noc_pcie_west_tunnel_axi_cbcr, 0x304);
check_member(calypso_pcie_noc, trace_noc_pcie_3b_at_cbcr, 0x448);
check_member(calypso_pcie_noc, trace_noc_pcie_5_at_cbcr, 0x44C);
check_member(calypso_pcie_noc, pcie_link_xo_cbcr, 0x4E4);

/*
 * PCIE5 register block
 * Embedded in calypso_gcc at offset 0xC3000 (absolute: GCC_BASE + 0xC3000)
 */
struct calypso_pcie_5 {
	u32 bcr;
	u32 gdscr;
	u8 _res0[0x18 - 0x8];
	u32 qmip_pcie_5_ahb_cbcr;
	u32 slv_q2a_axi_cbcr;
	u8 _res1[0x24 - 0x20];
	u32 slv_axi_cbcr;
	u8 _res2[0x38 - 0x28];
	u32 mstr_axi_cbcr;
	u8 _res3[0x4C - 0x3C];
	u32 aux_cbcr;
	u8 _res4[0x5C - 0x50];
	u32 pipe_cbcr;
	u8 _res5[0x70 - 0x60];
	u32 pipe_div2_cbcr;
	u8 _res6[0x80 - 0x74];
	u32 phy_rchng_cbcr;
	struct clock_rcg phy_rchng_rcg;
	u8 _res7a[0x9C - 0x8C];
	u32 pipe_muxr;
	u8 _res7b[0xC0 - 0xA0];
	u32 at_cbcr;
	u8 _res8[0xD0 - 0xC4];
	u32 aggre_noc_pcie_5_east_sf_axi_cbcr;
};
check_member(calypso_pcie_5, gdscr, 0x004);
check_member(calypso_pcie_5, qmip_pcie_5_ahb_cbcr, 0x018);
check_member(calypso_pcie_5, slv_q2a_axi_cbcr, 0x01C);
check_member(calypso_pcie_5, slv_axi_cbcr, 0x024);
check_member(calypso_pcie_5, mstr_axi_cbcr, 0x038);
check_member(calypso_pcie_5, aux_cbcr, 0x04C);
check_member(calypso_pcie_5, pipe_cbcr, 0x05C);
check_member(calypso_pcie_5, pipe_div2_cbcr, 0x070);
check_member(calypso_pcie_5, phy_rchng_cbcr, 0x080);
check_member(calypso_pcie_5, phy_rchng_rcg, 0x084);
check_member(calypso_pcie_5, pipe_muxr, 0x09C);
check_member(calypso_pcie_5, at_cbcr, 0x0C0);
check_member(calypso_pcie_5, aggre_noc_pcie_5_east_sf_axi_cbcr, 0x0D0);

struct calypso_gcc {
	struct calypso_gpll gpll0;
	u8 _res0[0x28004 - 0x3A];
	struct qupv3_clock qup_wrap0_s[8];
	u8 _res1[0x50000 - 0x28984];
	u32 qspi_bcr;
	u32 qspi_cnoc_ahb_cbcr;
	u32 qspi_core_cbcr;
	struct clock_rcg qspi_core;
	u8 _res2[0x62000 - 0x50014];
	u32 apcs_clk_br_en;
	u8 _res3[0x62008 - 0x62004];
	u32 apcs_clk_br_en1;
	u8 _res4[0x62010 - 0x6200C];
	u32 apcs_clk_br_en2;
	u8 _res5[0x62018 - 0x62014];
	u32 apcs_clk_br_en3;
	u8 _res6[0x62020 - 0x6201C];
	u32 apcs_clk_br_en4;
	u8 _res7[0x62028 - 0x62024];
	u32 apcs_clk_br_en5;
	u8 _res8a[0x62030 - 0x6202C];
	u32 apcs_clk_br_en6;
	u8 _res8b[0x62040 - 0x62034];
	u32 apcs_pll_br_en;
	u8 _res9a[0x82064 - 0x62044];
	u32 anoc_pcie_pwrctl_cbcr;
	u8 _res9b[0x9C030 - 0x82068];
	u32 trace_noc_tcu_pcie_qtc_at_cbcr;
	u8 _res9c[0xB3004 - 0x9C034];
	struct qupv3_clock qup_wrap1_s[8];
	u8 _res10[0xB4004 - 0xB3984];
	struct qupv3_clock qup_wrap2_s[8];
	u8 _res11[0xB8008 - 0xB4984];
	u32 pcie_rscc_xo_cbcr;
	u8 _res11b[0xB9048 - 0xB800C];
	u32 smmu_pcie_qtc_at_cbcr;
	u8 _res11c[0xBA000 - 0xB904C];
	struct calypso_pcie_noc pcie_noc;
	u8 _res11da[0xBA4F8 - 0xBA4E8];
	u32 pcie_5_cfg_ahb_cbcr;
	u8 _res11db[0xC3000 - 0xBA4FC];
	struct calypso_pcie_5 pcie_5;
	u8 _res11e[0xC5028 - 0xC30D4];
	u32 qup_oob_s_ahb_cbcr;
	u32 qup_oob_core_cbcr;
	u8 _res12[0xC503C - 0xC5030];
	u32 qup_oob_core_cdivr;
	u32 qup_oob_core_2x_cbcr;
	u8 _res13[0xC517C - 0xc5044];
	u32 qup_wrap1_m_ahb_cbcr;
	u32 qup_wrap1_s_ahb_cbcr;
	u32 qup_wrap1_core_cbcr;
	u8 _res14[0xC5194 - 0xC5188];
	u32 qup_wrap1_core_cdivr;
	u32 qup_wrap1_core_2x_cbcr;
	u8 _res15[0xC51A8 - 0xC519C];
	struct clock_rcg qup_wrap1_core_2x;
	u8 _res16[0xC52D4 - 0xC51B0];
	u32 qup_wrap2_m_ahb_cbcr;
	u32 qup_wrap2_s_ahb_cbcr;
	u32 qup_wrap2_core_cbcr;
	u8 _res17[0xC52EC - 0xC52E0];
	u32 qup_wrap2_core_cdivr;
	u32 qup_wrap2_core_2x_cbcr;
	u8 _res18[0xC5300 - 0xC52F4];
	struct clock_rcg qup_wrap2_core_2x;
	u8 _res19[0xC542C - 0xC5308];
	u32 qup_wrap0_m_ahb_cbcr;
	u32 qup_wrap0_s_ahb_cbcr;
	u32 qup_wrap0_core_cbcr;
	u8 _res20[0xC5444 - 0xC5438];
	u32 qup_wrap0_core_cdivr;
	u32 qup_wrap0_core_2x_cbcr;
	u8 _res21[0xC5458 - 0xC544C];
	struct clock_rcg qup_wrap0_core_2x;
	u8 _res20a[0xD2004 - 0xC5460];
	u32 pcie_phy_5_gdscr;
	u8 _res20b[0xD2030 - 0xD2008];
	u32 pcie_phy_5_aux_cbcr;
	u8 _res20c[0xE7004 - 0xD2034];
	u32 qup_oob_m_ahb_cbcr;
	u8 _res23[0xE7014 - 0xE7008];
	struct qupv3_clock qup_wrap3_s[2];
};

check_member(calypso_gcc, qup_wrap0_s, 0x28004);
check_member(calypso_gcc, qspi_bcr, 0x50000);
check_member(calypso_gcc, apcs_clk_br_en, 0x62000);
check_member(calypso_gcc, apcs_clk_br_en1, 0x62008);
check_member(calypso_gcc, apcs_clk_br_en2, 0x62010);
check_member(calypso_gcc, apcs_clk_br_en3, 0x62018);
check_member(calypso_gcc, apcs_clk_br_en4, 0x62020);
check_member(calypso_gcc, apcs_clk_br_en5, 0x62028);
check_member(calypso_gcc, apcs_pll_br_en, 0x62040);
check_member(calypso_gcc, qup_wrap1_s, 0xB3004);
check_member(calypso_gcc, qup_wrap2_s, 0xB4004);
check_member(calypso_gcc, pcie_rscc_xo_cbcr, 0xB8008);
check_member(calypso_gcc, smmu_pcie_qtc_at_cbcr, 0xB9048);
check_member(calypso_gcc, pcie_noc, 0xBA000);
check_member(calypso_gcc, pcie_5_cfg_ahb_cbcr, 0xBA4F8);
check_member(calypso_gcc, pcie_5, 0xC3000);
check_member(calypso_gcc, qup_oob_s_ahb_cbcr, 0xC5028);
check_member(calypso_gcc, qup_oob_core_cbcr, 0xC502C);
check_member(calypso_gcc, qup_oob_core_cdivr, 0xC503C);
check_member(calypso_gcc, qup_oob_core_2x_cbcr, 0xC5040);
check_member(calypso_gcc, qup_wrap1_m_ahb_cbcr, 0xC517C);
check_member(calypso_gcc, qup_wrap1_core_2x, 0xC51A8);
check_member(calypso_gcc, qup_wrap2_m_ahb_cbcr, 0xC52D4);
check_member(calypso_gcc, qup_wrap2_core_2x, 0xC5300);
check_member(calypso_gcc, qup_wrap0_m_ahb_cbcr, 0xC542C);
check_member(calypso_gcc, qup_wrap0_core_2x, 0xC5458);
check_member(calypso_gcc, pcie_phy_5_gdscr, 0xD2004);
check_member(calypso_gcc, pcie_phy_5_aux_cbcr, 0xD2030);
check_member(calypso_gcc, qup_oob_m_ahb_cbcr, 0xE7004);
check_member(calypso_gcc, qup_wrap3_s, 0xE7014);

/* Generic QUPV3 wrapper structure - all wrappers have identical layout */
struct calypso_qupv3_wrap {
	struct qupv3_clock qupv3_s0;
	struct qupv3_clock qupv3_s1;
	struct qupv3_clock_v2 qupv3_s2;
	struct qupv3_clock_v2 qupv3_s3;
	struct qupv3_clock qupv3_s4;
	struct qupv3_clock qupv3_s5;
	struct qupv3_clock qupv3_s6;
	struct qupv3_clock qupv3_s7;
};

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
	QUP_WRAP3_S0,
	QUP_WRAP3_S1,
};

/* PCIe clock indices - each maps to a CBCR register in pcie_cfg[]. */
enum clk_pcie {
	GCC_AGGRE_NOC_PCIE_5_EAST_SF_AXI_CLK,
	GCC_AGGRE_NOC_PCIE_EAST_TUNNEL_AXI_CLK,
	GCC_AGGRE_NOC_PCIE_WEST_TUNNEL_AXI_CLK,
	GCC_ANOC_PCIE_PWRCTL_CLK,
	GCC_CNOC_PCIE_EAST_TUNNEL_CLK,
	GCC_CNOC_PCIE_WEST_TUNNEL_CLK,
	GCC_PCIE_LINK_XO_CLK,
	GCC_HSCNOC_PCIE_SF_QTC_CLK,
	GCC_HSCNOC_PCIE_SLAVE_SF_EAST_CLK,
	GCC_HSCNOC_PCIE_SLAVE_SF_WEST_CLK,
	GCC_PCIE_NOC_PWRCTL_CLK,
	GCC_PCIE_NOC_SF_CENTER_CLK,
	GCC_PCIE_NOC_SLAVE_SF_EAST_CLK,
	GCC_PCIE_NOC_SLAVE_SF_WEST_CLK,
	GCC_SMMU_PCIE_QTC_AT_CLK,
	GCC_TCU_PCIE_SF_QTC_CLK,
	GCC_TRACE_NOC_PCIE_3B_AT_CLK,
	GCC_TRACE_NOC_PCIE_5_AT_CLK,
	GCC_TRACE_NOC_TCU_PCIE_QTC_AT_CLK,
	GCC_PCIE_5_AT_CLK,
	GCC_PCIE_5_AUX_CLK,
	GCC_PCIE_5_CFG_AHB_CLK,
	GCC_PCIE_5_MSTR_AXI_CLK,
	GCC_PCIE_5_PHY_RCHNG_CLK,
	GCC_PCIE_5_PIPE_DIV2_CLK,
	GCC_PCIE_5_SLV_AXI_CLK,
	GCC_PCIE_5_SLV_Q2A_AXI_CLK,
	GCC_PCIE_PHY_5_AUX_CLK,
	GCC_PCIE_5_PIPE_CLK,
	GCC_PCIE_5_PIPE_MUXR,
	GCC_QMIP_PCIE_5_AHB_CLK,
	PCIE_CLK_COUNT,
};

struct pcie {
	uint32_t *gdscr;
	uint32_t *clk;
	uint32_t *clk_br_en;
	int vote_bit;
};

enum clk_gdsc {
	PCIE_5_GDSC,
	PCIE_5_PHY_GDSC,
	MAX_GDSC,
};

/* Pipe clock source select: 0 = PHY pipe clock (post-PHY init) */
#define GCC_PCIE_5_PIPE_SRC_SEL		0

enum cb_err clock_enable_pcie(enum clk_pcie clk_type);
enum cb_err clock_configure_mux(enum clk_pcie clk_type, u32 src_type);
enum cb_err clock_enable_gdsc(enum clk_gdsc gdsc_type);
enum cb_err clock_configure_pcie(void);

/* Subsystem Reset */
static struct aoss *const aoss = (void *)AOSS_CC_BASE;
static struct calypso_gcc *const gcc = (void *)GCC_BASE;
static struct calypso_ncc0_clock *const apss_ncc0 = (void *)NCC0_BASE;
static struct calypso_qupv3_wrap *const qup_wrap0_clk = (void *)GCC_QUPV3_WRAP0_BASE;
static struct calypso_qupv3_wrap *const qup_wrap1_clk = (void *)GCC_QUPV3_WRAP1_BASE;
static struct calypso_qupv3_wrap *const qup_wrap2_clk = (void *)GCC_QUPV3_WRAP2_BASE;
static struct calypso_qupv3_wrap *const qup_oob_clk = (void *)GCC_QUPV3_OOB_BASE;

enum cb_err pll_init_and_set(struct calypso_ncc0_clock *ncc0, u32 l_val);

#endif	// __SOC_QUALCOMM_CALYPSO_CLOCK_H__
