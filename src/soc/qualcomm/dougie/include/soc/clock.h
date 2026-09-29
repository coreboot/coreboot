/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_DOUGIE_CLOCK_H__
#define __SOC_QUALCOMM_DOUGIE_CLOCK_H__

#include <soc/addressmap.h>
#include <types.h>
#include <soc/clock_common.h>

#define SRC_XO_HZ		(19200 * KHz)
#define GPLL0_EVEN_HZ		(300 * MHz)
#define GPLL0_MAIN_HZ		(600 * MHz)
#define CLK_100MHZ		(100 * MHz)

/* CPU PLL */
#define L_VAL_1516P8MHz		0x4F
#define L_VAL_1190P4MHz		0x3E

#define QUPV3_WRAP0_CLK_ENA_S(idx)	(10 + (idx))
#define QUPV3_WRAP1_CLK_ENA_S(idx)	(22 + (idx))
#define QUPV3_WRAP1_CLK_ENA_1_S(idx)	(7 + (idx))

enum clk_pll_src {
	SRC_XO_19_2MHZ = 0,
	SRC_GPLL0_MAIN_600MHZ = 1,
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
	NO_VOTE_BIT = -1,
};

struct dougie_gpll {
	u32 mode;
	u32 l;
	u32 cal_l;
	u32 user_ctl;
	u32 user_ctl_u;
	u32 user_ctl_u1;
	u32 config_ctl;
	u32 config_ctl_u;
	u32 config_ctl_u1;
	u32 test_ctl;
	u32 test_ctl_u;
	u32 test_ctl_u1;
	u8 _res0[0x38 - 0x30];
	u32 opmode;
	u8 _res1[0x40 - 0x3c];
	u32 alpha;
};

struct dougie_gcc {
	struct dougie_gpll gpll0;
	u8 _res0[0xf000 - 0x44];
	u32 usb30_prim_bcr;
	u8 _res1[0x12000 - 0xf004];
	u32 qusb2phy_prim_bcr;
	u32 qusb2phy_sec_bcr;
	u8 _res2[0x17000 - 0x12008];
	u32 qup_wrap0_bcr;
	u32 qup_wrap0_m_ahb_cbcr;
	u32 qup_wrap0_s_ahb_cbcr;
	struct qupv3_clock qup_wrap0_s[8];
	u8 _res5[0x18000 - 0x1798c];
	u32 qup_wrap1_bcr;
	u32 qup_wrap1_m_ahb_cbcr;
	u32 qup_wrap1_s_ahb_cbcr;
	struct qupv3_clock qup_wrap1_s[8];
	u8 _res6[0x23000 - 0x1898c];
	u32 qup_wrap0_core_cbcr;
	u32 qup_wrap0_core_cdivr;
	u32 qup_wrap0_core_2x_cbcr;
	struct clock_rcg qup_wrap0_core_2x;
	u8 _res9[0x23138 - 0x23014];
	u32 qup_wrap1_core_cbcr;
	u32 qup_wrap1_core_cdivr;
	u32 qup_wrap1_core_2x_cbcr;
	struct clock_rcg qup_wrap1_core_2x;
	u8 _res10[0x4b000 - 0x2314c];
	u32 qspi_bcr;
	u32 qspi_cnoc_ahb_cbcr;
	u32 qspi_core_cbcr;
	struct clock_rcg qspi_core;
	u8 _res14[0x50000 - 0x4b014];
	u32 usb3_phy_prim_bcr;
	u32 usb3phy_phy_prim_bcr;
	u32 usb3_dp_phy_prim_bcr;
	u8 _res15[0x52000 - 0x5000c];
	u32 apcs_clk_br_en;
	u8 _res16[0x52008 - 0x52004];
	u32 apcs_clk_br_en1;
	u8 _res17[0x52010 - 0x5200c];
	u32 apcs_pll_br_en;
	u8 _res18[0x6a000 - 0x52014];
	u32 usb_phy_cfg_ahb2phy_bcr;
	u8 _res19[0x9e000 - 0x6a004];
	u32 usb30_sec_bcr;
};
check_member(dougie_gcc, qusb2phy_prim_bcr, 0x12000);
check_member(dougie_gcc, qup_wrap0_bcr, 0x17000);
check_member(dougie_gcc, qup_wrap1_bcr, 0x18000);
check_member(dougie_gcc, qup_wrap1_core_cbcr, 0x23138);
check_member(dougie_gcc, qspi_bcr, 0x4b000);
check_member(dougie_gcc, usb3_phy_prim_bcr, 0x50000);
check_member(dougie_gcc, apcs_clk_br_en1, 0x52008);
check_member(dougie_gcc, apcs_pll_br_en, 0x52010);
check_member(dougie_gcc, usb_phy_cfg_ahb2phy_bcr, 0x6a000);
check_member(dougie_gcc, usb30_sec_bcr, 0x9e000);

struct dougie_apss_pll {
	u32 mode;
	u32 l;
	u32 alpha;
	u32 user_ctl;
	u32 config_ctl_lo;
	u32 config_ctl_hi;
	u32 config_ctl_u1;
	u32 test_ctl_lo;
	u32 test_ctl_hi;
	u32 test_ctl_u1;
	u32 opmode;
	u8 _res0[0x38 - 0x2c];
	u32 status;
};

struct dougie_apss_clock {
	struct dougie_apss_pll pll;
	u8 _res0[0x84 - 0x3c];
	u32 cfg_gfmux;
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
};

enum subsystem_reset {
	AOP_RESET_SHFT = 0,
	SHRM_RUN_STALL = 0,
};

enum pll_config_ctl_lo {
	CTUNE_SHFT = 2,
	K_I_SHFT = 4,
	K_P_SHFT = 7,
	PFA_MSB_SHFT = 10,
	RES_BIT_SHFT = 14,
	RON_DEGEN_MUL_SHFT = 18,
	ALPHA_CAL_SHFT = 20,
	DCO_ADDER_EN_SHFT = 22,
	PLL_COUNTER_EN = 27,
};

enum pll_config_ctl_hi {
	CUR_TRIM_SHFT = 0,
	FREQ_DOUBLE_SHFT = 4,
	ADJ_ENABLE_SHFT = 5,
	ADJ_VALUE_SHFT = 6,
	KLSB_SHFT = 13,
	RON_MODE_SHFT = 17,
	CHP_REF_SHFT = 19,
	CHP_STARTUP = 21,
	ADC_KMSB_VAL = 23,
};

enum pll_config_ctl_u1 {
	FAST_LOCK_LOW_L_SHFT = 4,
	DCO_BIAS_ADJ_SHFT = 26,
};

enum apss_gfmux {
	GFMUX_SRC_SEL_BMSK = 0x3,
	APCS_SRC_EARLY = 0x2,
};

static struct dougie_gcc *const gcc = (void *)GCC_BASE;
static struct dougie_apss_clock *const apss_silver = (void *)SILVER_PLL_BASE;
static struct dougie_apss_clock *const apss_l3 = (void *)L3_PLL_BASE;

void clock_init(void);
void clock_configure_qspi(uint32_t hz);
void clock_enable_qup(int qup);
void clock_configure_dfsr(int qup);

/* Subsystem Reset */
static struct aoss *const aoss = (void *)AOSS_CC_BASE;
static struct shrm *const shrm = (void *)SHRM_SPROC_BASE;

#define clock_reset_aop() \
	do { \
		clock_reset_subsystem(&aoss->aoss_cc_apcs_misc, AOP_RESET_SHFT); \
		printk(BIOS_DEBUG, "SOC:AOP brought out of reset.\n"); \
	} while (0)

#define clock_reset_shrm() \
	do { \
		clock_reset_subsystem(&shrm->shrm_sproc_ctrl, SHRM_RUN_STALL); \
		printk(BIOS_DEBUG, "SOC:SHRM brought out of reset.\n"); \
	} while (0)

#endif /* __SOC_QUALCOMM_DOUGIE_CLOCK_H__ */
