 /* SPDX-License-Identifier: GPL-2.0-only */
#include <soc/addressmap.h>
#include <types.h>

#ifndef __SOC_QUALCOMM_QCS405_CLOCK_H__
#define	__SOC_QUALCOMM_QCS405_CLOCK_H__

#define	BLSP1_AHB_CLK_ENA			10
#define BLSP2_AHB_CLK_ENA			20
#define SRC_XO_19_2MHZ				0
#define SRC_GPLL0_MAIN_800MHZ			1

/**
 *    USB BCR registers
 */
#define GCC_USB_HS_PHY_CFG_AHB_BCR	0x180000C
#define GCC_USB_HS_BCR			0x1841000
#define GCC_USB_30_BCR			0x1839000
#define GCC_USB2A_PHY_BCR		0x180000C
#define GCC_USB2_HS_PHY_ONLY_BCR	0x1841034
#define GCC_QUSB2_PHY_BCR		0x184103C

struct qcs405_rcg {
	u32 cmd;
	u32 cfg;
};

struct qcs405_clock {
	struct qcs405_rcg rcg;
	uint32_t m;
	uint32_t n;
	uint32_t d_2;
};

struct qcs405_bcr {
	uint32_t bcr;
};

struct qcs405_gpll {
	u32 mode;
	u32 l_val;
	u32 gpll_alpha_val;
	u32 gpll_alpha_val_u;
	u32 user_ctl;
	u32 user_ctl_u;
	u32 config_ctl;
	u32 test_ctl;
	u32 test_ctl_u;
};

struct qcs405_gcc {
	u8 _res0[0x1000 - 0x0];
	u32 blsp1_bcr;
	u32 blsp1_sleep_cbcr;
	u32 blsp1_ahb_cbcr;
	struct qcs405_rcg gcc_blsp_uart_sim_rcg;
	u8 _res1[0x2000 - 0x1014];
	u32 blsp1_qup1_bcr;
	u32 blsp1_qup1_spi_apps_cbcr;
	u32 blsp1_qup1_i2c_apps_cbcr;
	struct qcs405_clock blsp1_qup1_i2c_clk;
	u8 _res2[0x2024 - 0x2020];
	struct qcs405_clock blsp1_qup1_spi_clk;
	u8 _res3[0x3008-0x2038];
	u32 blsp1_qup2_bcr;
	u32 blsp1_qup2_spi_apps_cbcr;
	u8 _res4[0x3014 - 0x3010];
	struct qcs405_clock blsp1_qup2_spi_clk;
	u32 blsp1_uart2_bcr;
	u32 blsp1_uart2_apps_cbcr;
	u32 blsp1_uart2_sim_cbcr;
	struct qcs405_clock blsp1_uart2_apps_clk;
	u8 _res5[0x4018 - 0x3048];
	u32 blsp1_qup3_bcr;
	u32 blsp1_qup3_spi_apps_cbcr;
	u8 _res6[0x4024 - 0x4020];
	struct qcs405_clock blsp1_qup3_spi_clk;
	u8 _res7[0x5018 - 0x4038];
	u32 blsp1_qup4_bcr;
	u32 blsp1_qup4_spi_apps_cbcr;
	u8 _res8[0x5024 - 0x5020];
	struct qcs405_clock blsp1_qup4_spi_clk;
	u8 _res9[0x6020 - 0x5038];
	u32 blsp1_qup0_bcr;
	u32 blsp1_qup0_spi_apps_cbcr;
	u8 _res10[0x6034 - 0x6028];
	struct qcs405_clock blsp1_qup0_spi_clk;
	u8 _res11[0xB000 - 0x6048];
	u32 blsp2_bcr;
	u32 blsp2_sleep_cbcr;
	u32 blsp2_ahb_cbcr;
	u8 _res12[0xC000 - 0xB00C];
	u32 blsp2_qup0_bcr;
	u32 blsp2_qup0_spi_apps_cbcr;
	u8 _res13[0xC024 - 0xC008];
	struct qcs405_clock blsp2_qup0_spi_clk;
	u8 _res14[0x21000 - 0xC038];
	struct qcs405_gpll gpll0;
	u8 _res15[0x45004 - 0x21024];
	u32 gcc_apcs_clock_branch_en_vote;
};

struct mdss_clock_config {
	const char *clk_name;
	struct qcs405_clock *rcgr;
	uint32_t  *cbcr;
};

enum clk_ctl_gpll_user_ctl {
	CLK_CTL_GPLL_PLLOUT_LV_EARLY_BMSK	= 0x8,
	CLK_CTL_GPLL_PLLOUT_AUX2_BMSK		= 0x4,
	CLK_CTL_GPLL_PLLOUT_AUX_BMSK		= 0x2,
	CLK_CTL_GPLL_PLLOUT_MAIN_BMSK		= 0x1,
	CLK_CTL_GPLL_PLLOUT_LV_EARLY_SHFT	= 3,
	CLK_CTL_GPLL_PLLOUT_AUX2_SHFT		= 2,
	CLK_CTL_GPLL_PLLOUT_AUX_SHFT		= 1,
	CLK_CTL_GPLL_PLLOUT_MAIN_SHFT		= 0,
};

enum clk_ctl_cfg_rcgr {
	CLK_CTL_CFG_MODE_BMSK		= 0x3000,
	CLK_CTL_CFG_MODE_SHFT		= 12,
	CLK_CTL_CFG_SRC_SEL_BMSK	= 0x700,
	CLK_CTL_CFG_SRC_SEL_SHFT	= 8,
	CLK_CTL_CFG_SRC_DIV_BMSK	= 0x1F,
	CLK_CTL_CFG_SRC_DIV_SHFT	= 0
};

enum clk_ctl_cmd_rcgr {
	CLK_CTL_CMD_ROOT_OFF_BMSK	= 0x80000000,
	CLK_CTL_CMD_ROOT_OFF_SHFT	= 31,
	CLK_CTL_CMD_ROOT_EN_BMSK	= 0x2,
	CLK_CTL_CMD_ROOT_EN_SHFT	= 1,
	CLK_CTL_CMD_UPDATE_BMSK		= 0x1,
	CLK_CTL_CMD_UPDATE_SHFT		= 0
};

enum clk_ctl_cbcr {
	CLK_CTL_CBC_CLK_OFF_BMSK	= 0x80000000,
	CLK_CTL_CBC_CLK_OFF_SHFT	= 31,
	CLK_CTL_CBC_CLK_EN_BMSK		= 0x1,
	CLK_CTL_CBC_CLK_EN_SHFT		= 0
};

enum clk_ctl_rcg_mnd {
	CLK_CTL_RCG_MND_BMSK		= 0xFFFF,
	CLK_CTL_RCG_MND_SHFT		= 0,
};

enum clk_ctl_bcr {
	CLK_CTL_BCR_BLK_ARES_BMSK	= 0x1,
	CLK_CTL_BCR_BLK_ARES_SHFT	= 0,
};

struct clock_config {
	uint32_t hz;
	uint32_t hw_ctl;
	uint32_t src;
	uint32_t div;
	uint32_t m;
	uint32_t n;
	uint32_t d_2;
};

static struct qcs405_gcc *const gcc = (void *)GCC_BASE;

void clock_init(void);
void clock_reset_aop(void);
int clock_configure_qspi(uint32_t hz);
int clock_reset_bcr(void *bcr_addr, bool reset);
void clock_configure_uart(uint32_t hz);
void clock_configure_i2c(uint32_t hz);
void clock_configure_spi(int blsp, int qup, uint32_t hz);
void clock_enable_uart(void);
void clock_disable_uart(void);
void clock_enable_spi(int blsp, int qup);
void clock_disable_spi(int blsp, int qup);
void clock_enable_i2c(void);
void clock_disable_i2c(void);

#endif	// __SOC_QUALCOMM_QCS405_CLOCK_H__
