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
	u8 _res8[0x62040 - 0x6202C];
	u32 apcs_pll_br_en;
	u8 _res9[0xB3004 - 0x62044];
	struct qupv3_clock qup_wrap1_s[8];
	u8 _res10[0xB4004 - 0xB3984];
	struct qupv3_clock qup_wrap2_s[8];
	u8 _res11[0xC5028 - 0xB4984];
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
	u8 _res22[0xE7004 - 0xC5460];
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
