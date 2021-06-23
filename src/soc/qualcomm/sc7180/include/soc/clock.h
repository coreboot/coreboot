/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <types.h>
#include <soc/clock_common.h>

#ifndef __SOC_QUALCOMM_SC7180_CLOCK_H__
#define __SOC_QUALCOMM_SC7180_CLOCK_H__

#define SRC_XO_HZ			(19200 * KHz)
#define GPLL0_EVEN_HZ			(300 * MHz)
#define GPLL0_MAIN_HZ			(600 * MHz)

#define QUPV3_WRAP0_CLK_ENA_S(idx)	(10 + idx)
#define QUPV3_WRAP1_CLK_ENA_S(idx)	(22 + idx)

enum apcs_branch_en_vote {
	QUPV3_WRAP_0_M_AHB_CLK_ENA = 6,
	QUPV3_WRAP_0_S_AHB_CLK_ENA = 7,
	QUPV3_WRAP0_CORE_CLK_ENA = 8,
	QUPV3_WRAP0_CORE_2X_CLK_ENA = 9,
	QUPV3_WRAP1_CORE_2X_CLK_ENA = 18,
	QUPV3_WRAP1_CORE_CLK_ENA = 19,
	QUPV3_WRAP_1_M_AHB_CLK_ENA = 20,
	QUPV3_WRAP_1_S_AHB_CLK_ENA = 21,
};

enum clk_pll_src {
	 SRC_XO_19_2MHZ = 0,
	 SRC_GPLL0_MAIN_600MHZ = 1,
	 SRC_GPLL0_EVEN_300MHZ = 6,
};

/* CPU PLL */
#define L_VAL_1516P8MHz		0x4F
#define L_VAL_1209P6MHz		0x3F

#define AOP_RESET_SHFT		0
#define SCALE_FREQ_SHFT		11

struct sc7180_gpll {
	u32 mode;
	u32 l;
	u32 cal_l;
	u32 user_ctl;
	u32 user_ctl_u;
	u32 config_ctl;
	u32 config_ctl_u;
	u32 test_ctl;
	u32 test_ctl_u;
	u8 _res[0x1000 - 0x24];
};

struct sc7180_gcc {
	struct sc7180_gpll gpll0;
	u8 _res0[0xf000 - 0x1000];
	u32 usb30_prim_bcr;
	u8 _res1[0x17000 - 0xf004];
	u32 qup_wrap0_bcr;
	u32 qup_wrap0_m_ahb_cbcr;
	u32 qup_wrap0_s_ahb_cbcr;
	u32 qup_wrap0_core_cbcr;
	u32 qup_wrap0_core_cdivr;
	u32 qup_wrap0_core_2x_cbcr;
	struct clock_rcg qup_wrap0_core_2x;
	u8 _res2[0x17030 - 0x17020];
	struct qupv3_clock qup_wrap0_s[6];
	u8 _res3[0x18000 - 0x17750];
	u32 qup_wrap1_bcr;
	u32 qup_wrap1_core_2x_cbcr;
	u32 qup_wrap1_core_cbcr;
	u32 qup_wrap1_m_ahb_cbcr;
	u32 qup_wrap1_s_ahb_cbcr;
	struct qupv3_clock qup_wrap1_s[6];
	u8 _res4[0x18994 - 0x18734];
	u32 qup_wrap1_core_cdivr;
	u8 _res5[0x26000 - 0x18998];
	u32 qusb2phy_prim_bcr;
	u8 _res6[0x4b000 - 0x26004];
	u32 qspi_bcr;
	u32 qspi_cnoc_ahb_cbcr;
	u32 qspi_core_cbcr;
	struct clock_rcg qspi_core;
	u8 _res7[0x50000 - 0x4b014];
	u32 usb3_phy_prim_bcr;
	u32 usb3phy_phy_prim_bcr;
	u32 usb3_dp_phy_prim_bcr;
	u32 usb3_phy_sec_bcr;
	u32 usb3phy_phy_sec_bcr;
	u32 usb3_dp_phy_sec_bcr;
	u8 _res8[0x52008 - 0x50018];
	u32 apcs_clk_br_en1;
	u8 _res9[0x1000000 - 0x5200c];
};
check_member(sc7180_gcc, usb30_prim_bcr, 0xf000);
check_member(sc7180_gcc, qup_wrap0_bcr, 0x17000);
check_member(sc7180_gcc, qup_wrap1_bcr, 0x18000);
check_member(sc7180_gcc, qup_wrap1_core_cdivr, 0x18994);
check_member(sc7180_gcc, qusb2phy_prim_bcr, 0x26000);
check_member(sc7180_gcc, usb3phy_phy_prim_bcr, 0x50004);
check_member(sc7180_gcc, usb3_phy_prim_bcr, 0x50000);
check_member(sc7180_gcc, apcs_clk_br_en1, 0x52008);

struct sc7180_disp_cc {
	u8 _res0[0x2004];
	u32 pclk0_cbcr;
	u8 _res1[0x2028 - 0x2008];
	u32 byte0_cbcr;
	u32 byte0_intf_cbcr;
	u8 _res2[0x2038 - 0x2030];
	u32 esc0_cbcr;
	u8 _res3[0x2098 - 0x203C];
	struct clock_rcg_mnd pclk0;
	u8 _res4[0x2110 - 0x20AC];
	struct clock_rcg_mnd byte0;
	u8 _res5[0x2148 - 0x2124];
	struct clock_rcg_mnd esc0;
	u8 _res6[0x10000 - 0x215C];
};
check_member(sc7180_disp_cc, byte0_cbcr, 0x2028);
check_member(sc7180_disp_cc, esc0_cbcr, 0x2038);

enum mdss_clock {
	MDSS_CLK_ESC0 = 0,
	MDSS_CLK_PCLK0,
	MDSS_CLK_BYTE0,
	MDSS_CLK_BYTE0_INTF,
	MDSS_CLK_COUNT
};

enum clk_qup {
	QUP_WRAP0_S0,
	QUP_WRAP0_S1,
	QUP_WRAP0_S2,
	QUP_WRAP0_S3,
	QUP_WRAP0_S4,
	QUP_WRAP0_S5,
	QUP_WRAP1_S0,
	QUP_WRAP1_S1,
	QUP_WRAP1_S2,
	QUP_WRAP1_S3,
	QUP_WRAP1_S4,
	QUP_WRAP1_S5,
};

struct sc7180_apss_pll {
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

struct sc7180_apss_clock {
	struct sc7180_apss_pll pll;
	u8 _res0[0x88 - 0x40];
	u32 cfg_gfmux;
};

enum pll_config_ctl_lo {
	CTUNE_SHFT = 2,
	K_I_SHFT = 4,
	K_P_SHFT = 7,
	PFA_MSB_SHFT = 10,
	REF_CONT_SHFT = 28,
};

enum pll_config_ctl_hi {
	CUR_ADJ_SHFT = 0,
	DMET_SHFT = 4,
	RES_SHFT = 6,
};

enum apss_gfmux {
	GFMUX_SRC_SEL_BMSK = 0x3,
	APCS_SRC_EARLY = 0x2,
};

static struct sc7180_gcc *const gcc = (void *)GCC_BASE;
static struct sc7180_apss_clock *const apss_silver = (void *)SILVER_PLL_BASE;
static struct sc7180_apss_clock *const apss_l3 = (void *)L3_PLL_BASE;
static struct sc7180_disp_cc *const mdss = (void *)DISP_CC_BASE;

void clock_init(void);
void clock_reset_aop(void);
void clock_configure_qspi(uint32_t hz);
void clock_configure_qup(int qup, uint32_t hz);
void clock_enable_qup(int qup);
void clock_configure_dfsr(int qup);
int mdss_clock_configure(enum mdss_clock clk_type, uint32_t source,
			uint32_t divider, uint32_t m, uint32_t n, uint32_t d);
int mdss_clock_enable(enum mdss_clock clk_type);

static struct aoss *const aoss = (void *)AOSS_CC_BASE;
#define clock_reset_aop()  \
	clock_reset_subsystem(&aoss->aoss_cc_apcs_misc, AOP_RESET_SHFT)

#endif	// __SOC_QUALCOMM_SC7180_CLOCK_H__
