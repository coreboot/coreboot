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

struct x1p42100_gcc {
	struct x1p42100_gpll gpll0;
	u8 _res1[0x18004 - 0x0002c];
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
	u8 _res12[0x42004 - 0x232cc];
	struct qupv3_clock qup_wrap0_s[8];
	u8 _res13[0x4b000 - 0x429c4];
	u32 qspi_bcr;
	u32 qspi_cnoc_ahb_cbcr;
	u32 qspi_core_cbcr;
	struct clock_rcg qspi_core;
	u8 _res14[0x52000 - 0x4b014];
	u32 apcs_clk_br_en;
	u8 _res15[0x52008 - 0x52004];
	u32 apcs_clk_br_en1;
	u8 _res16[0x52010 - 0x5200C];
	u32 apcs_clk_br_en2;
	u8 _res17[0x52018 - 0x52014];
	u32 apcs_clk_br_en3;
	u8 _res18[0x52020 - 0x5201c];
	u32 apcs_clk_br_en4;
	u8 _res19[0x52028 - 0x52024];
	u32 apcs_clk_br_en5;
	u8 _res20[0x52030 - 0x5202c];
	u32 apcs_pll_br_en;
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

/* TODO: update as per datasheet */
void clock_init(void);
void clock_configure_qspi(uint32_t hz);
void clock_enable_qup(int qup);
void clock_configure_dfsr(int qup);

/* Subsystem Reset */
static struct aoss *const aoss = (void *)AOSS_CC_BASE;
static struct x1p42100_gcc *const gcc = (void *)GCC_BASE;

#endif	// __SOC_QUALCOMM_X1P42100_CLOCK_H__
