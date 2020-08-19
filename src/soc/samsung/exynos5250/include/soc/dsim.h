/* SPDX-License-Identifier: GPL-2.0-only */

/* Register map for Exynos5 MIPI-DSIM */

#ifndef CPU_SAMSUNG_EXYNOS5250_DSIM_H
#define CPU_SAMSUNG_EXYNOS5250_DSIM_H

/* DSIM register map */
struct exynos5_dsim {
	unsigned int status;
	unsigned int swrst;
	unsigned int clkctrl;
	unsigned int timeout;
	unsigned int config;
	unsigned int escmode;
	unsigned int mdresol;
	unsigned int mvporch;
	unsigned int mhporch;
	unsigned int msync;
	unsigned int sdresol;
	unsigned int intsrc;
	unsigned int intmsk;
	unsigned int pkthdr;
	unsigned int payload;
	unsigned int rxfifo;
	unsigned int res1;
	unsigned int fifoctrl;
	unsigned int res2;
	unsigned int pllctrl;
	unsigned int plltmr;
	unsigned int phyacchr;
	unsigned int phyacchr1;
};
check_member(exynos5_dsim, phyacchr1, 0x54);

#define ENABLE		1
#define DISABLE		0

#define DSIM_SWRST			(1 << 0)
#define NUM_OF_DAT_LANE_IS_FOUR		(3 << 5)
#define DATA_LANE_0_EN			(1 << 0)
#define DATA_LANE_1_EN			(1 << 1)
#define DATA_LANE_2_EN			(1 << 2)
#define DATA_LANE_3_EN			(1 << 3)
#define CLK_LANE_EN			(1 << 4)
#define ENABLE_ALL_DATA_LANE		DATA_LANE_0_EN | \
					DATA_LANE_1_EN | \
					DATA_LANE_2_EN | \
					DATA_LANE_3_EN
#define	MAIN_PIX_FORMAT_OFFSET		12
#define RGB_565_16_BIT			0x4
#define VIDEO_MODE			(1 << 25)
#define BURST_MODE			(1 << 26)

#define DSIM_PHYACCHR_AFC_EN		(1 << 14)
#define DSIM_PHYACCHR_AFC_CTL_OFFSET	5

#define DSIM_PLLCTRL_PMS_OFFSET		1
#define DSIM_FREQ_BAND_OFFSET		24

#define LANE_ESC_CLK_EN_ALL		(0x1f << 19)
#define BYTE_CLK_EN			(1 << 24)
#define DSIM_ESC_CLK_EN			(1 << 28)
#define TXREQUEST_HS_CLK_ON		(1 << 31)

#define LP_MODE_ENABLE			(1 << 7)
#define STOP_STATE_CNT_OFFSET		21

#define MAIN_VBP_OFFSET		0
#define STABLE_VFP_OFFSET	16
#define CMD_ALLOW_OFFSET	28

#define MAIN_HBP_OFFSET		0
#define MAIN_HFP_OFFSET		16

#define MAIN_HSA_OFFSET		0
#define MAIN_VSA_OFFSET		22

#define MAIN_STANDBY		(1 << 31)
#define MAIN_VRESOL_OFFSET	16
#define MAIN_HRESOL_OFFSET	0

#define SFR_FIFO_EMPTY		(1 << 29)

#define DSIM_PLL_EN_SHIFT	(1 << 23)
#define PLL_STABLE		(1 << 31)

#define DSIM_STOP_STATE_DAT(x)  (((x) & 0xf) << 0)
#define DSIM_STOP_STATE_CLK     (1 << 8)
#define DSIM_TX_READY_HS_CLK    (1 << 10)

#endif
