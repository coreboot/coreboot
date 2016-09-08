/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
 *
 * This program is free software;
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_MARVELL_MVMAP2315_CLOCK_H__
#define __SOC_MARVELL_MVMAP2315_CLOCK_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

#define MVMAP2315_CLK_M_KHZ		25000
#define MVMAP2315_GENTIMER_EN		BIT(0)

struct mvmap2315_gentimer_regs {
	u32 cntcr;
	u32 cntsr;
	u32 cntcvl;
	u32 cntcvu;
	u8 _reserved0[0x10];
	u32 cntfid0;
	u8 _reserved1[0xfac];
	u32 pidr4;
	u8 _reserved2[0x0c];
	u32 pidr0;
	u32 pidr1;
	u32 pidr2;
	u32 pidr3;
	u32 cidr0;
	u32 cidr1;
	u32 cidr2;
	u32 cidr3;
};

check_member(mvmap2315_gentimer_regs, cidr3, 0xFFC);
static struct mvmap2315_gentimer_regs * const mvmap2315_gentimer
					= (void *)MVMAP2315_GENTIMER_BASE;

#define MVMAP2315_PLL_LOCK		BIT(0)
#define MVMAP2315_PLL_BYPASS_EN		BIT(16)

struct mvmap2315_main_pll_regs {
	u32 rst_prediv;
	u32 mult_postdiv;
	u32 kvco;
	u32 misc;
	u32 feedback_mode_deskew;
	u32 offset_mode;
	u32 fixed_mode_ssc_mode;
	u32 ssc_freq_ssc_range;
	u32 clk_ctrl_marvell_test;
	u32 lock_status;
	u32 reserve_out;
};

check_member(mvmap2315_main_pll_regs, reserve_out, 0x28);
static struct mvmap2315_main_pll_regs * const mvmap2315_pll
					= (void *)MVMAP2315_MAIN_PLL_BASE;

#define MVMAP2315_UART_CLK_EN		BIT(1)
#define MVMAP2315_SDMMC_CLK_RSTN	BIT(0)
#define MVMAP2315_APMU_CLK_EN		BIT(1)
#define MVMAP2315_APMU_CLK_RSTN		BIT(0)
struct mvmap2315_apmu_clk_regs {
	u32 uartfracdivcfg0;
	u8 _reserved0[0x0c];
	u32 uartfracdivcfg1;
	u8 _reserved1[0x0c];
	u32 r4clkstatus;
	u8 _reserved2[0x5c];
	u32 busclk2x_a2_clkgenconfig;
	u32 busclk2x_a2_clkgenstatus;
	u8 _reserved3[0x08];
	u32 busclk_mcix2_clkgenconfig;
	u32 busclk_mcix2_clkgenstatus;
	u32 busclk_mcix2_phyreset_clkgenconfig;
	u32 busclk_mcix2_phyreset_clkgenstatus;
	u32 busclk_mcix10_clkgenconfig;
	u32 busclk_mcix10_clkgenstatus;
	u32 busclk_mcix1_phyreset0_clkgenconfig;
	u32 busclk_mcix1_phyreset0_clkgenstatus;
	u32 busclk_mcix11_clkgenconfig;
	u32 busclk_mcix11_clkgenstatus;
	u32 busclk_mcix1_phyreset1_clkgenconfig;
	u32 busclk_mcix1_phyreset1_clkgenstatus;
	u32 busclk_mcix12_clkgenconfig;
	u32 busclk_mcix12_clkgenstatus;
	u32 busclk_mcix1_phyreset2_clkgenconfig;
	u32 busclk_mcix1_phyreset2_clkgenstatus;
	u32 busclk_mcix13_clkgenconfig;
	u32 busclk_mcix13_clkgenstatus;
	u32 busclk_mcix1_phyreset3_clkgenconfig;
	u32 busclk_mcix1_phyreset3_clkgenstatus;
	u8 _reserved4[0x10];
	u32 busclk_aes_clkgenconfig;
	u32 busclk_aes_clkgenstatus;
	u32 busclk_apaonbus_hs_clkgenconfig;
	u32 busclk_apaonbus_hs_clkgenstatus;
	u32 busclk_a2_clkgenconfig;
	u32 busclk_a2_clkgenstatus;
	u8 _reserved5[0x78];
	u32 apaonclk_clkgenconfig;
	u32 apaonclk_clkgenstatus;
	u32 apaonclk_apmucpu_clkgenconfig;
	u32 apaonclk_apmucpu_clkgenstatus;
	u32 apaonclk_sdmmc_clkgenconfig;
	u32 apaonclk_sdmmc_clkgenstatus;
	u8 _reserved6[0x08];
	u32 apaonclk_m2m_clkgenconfig;
	u32 apaonclk_m2m_clkgenstatus;
	u32 apaonclk_apb_clkgenconfig;
	u32 apaonclk_apb_clkgenstatus;
	u8 _reserved7[0x50];
	u32 bistclk_clkgenconfig;
	u32 bistclk_clkgenstatus;
	u32 bistclk_a2reset_clkgenconfig;
	u32 bistclk_a2reset_clkgenstatus;
	u32 bistclk_apcpureset_clkgenconfig;
	u32 bistclk_apcpureset_clkgenstatus;
	u32 bistclk_coresightreset_clkgenconfig;
	u32 bistclk_coresightreset_clkgenstatus;
	u32 bistclk_mcflcreset_clkgenconfig;
	u32 bistclk_mcflcreset_clkgenstatus;
	u8 _reserved8[0x08];
	u32 bistclk_gpu3dreset_clkgenconfig;
	u32 bistclk_gpu3dreset_clkgenstatus;
	u32 bistclk_gpu3dcorereset0_clkgenconfig;
	u32 bistclk_gpu3dcorereset0_clkgenstatus;
	u32 bistclk_gpu3dcorereset1_clkgenconfig;
	u32 bistclk_gpu3dcorereset1_clkgenstatus;
	u32 bistclk_gpu3dcorereset2_clkgenconfig;
	u32 bistclk_gpu3dcorereset2_clkgenstatus;
	u32 bistclk_gpu3dcorereset3_clkgenconfig;
	u32 bistclk_gpu3dcorereset3_clkgenstatus;
	u32 bistclk_gpu2dreset_clkgenconfig;
	u32 bistclk_gpu2dreset_clkgenstatus;
	u32 bistclk_zramreset_clkgenconfig;
	u32 bistclk_zramreset_clkgenstatus;
	u32 bistclk_vpuencreset_clkgenconfig;
	u32 bistclk_vpuencreset_clkgenstatus;
	u32 bistclk_vpudecreset_clkgenconfig;
	u32 bistclk_vpudecreset_clkgenstatus;
	u32 bistclk_displayreset_clkgenconfig;
	u32 bistclk_displayreset_clkgenstatus;
	u32 bistclk_edisplayreset_clkgenconfig;
	u32 bistclk_edisplayreset_clkgenstatus;
	u8 _reserved9[0x78];
	u32 sdmmcbaseclk_clkgenconfig;
	u32 sdmmcbaseclk_clkgenstatus;
	u8 _reserved10[0x08];
	u32 cfgclk_a2_clkgenconfig;
	u32 cfgclk_a2_clkgenstatus;
	u8 _reserved11[0x08];
	u32 uartclk0_clkgenconfig;
	u32 uartclk0_clkgenstatus;
	u8 _reserved12[0x08];
	u32 uartclk1_clkgenconfig;
	u32 uartclk1_clkgenstatus;
	u8 _reserved13[0x08];
	u32 sspclk0_clkgenconfig;
	u32 sspclk0_clkgenstatus;
	u8 _reserved14[0x08];
	u32 sspclk1_clkgenconfig;
	u32 sspclk1_clkgenstatus;
	u8 _reserved15[0x08];
	u32 i2cclk0_clkgenconfig;
	u32 i2cclk0_clkgenstatus;
	u8 _reserved16[0x08];
	u32 i2cclk1_clkgenconfig;
	u32 i2cclk1_clkgenstatus;
	u8 _reserved17[0x08];
	u32 i2cclk2_clkgenconfig;
	u32 i2cclk2_clkgenstatus;
	u8 _reserved18[0x08];
	u32 i2cclk3_clkgenconfig;
	u32 i2cclk3_clkgenstatus;
	u8 _reserved19[0x08];
	u32 i2cclk4_clkgenconfig;
	u32 i2cclk4_clkgenstatus;
};

check_member(mvmap2315_apmu_clk_regs, i2cclk4_clkgenstatus, 0x3A4);
static struct mvmap2315_apmu_clk_regs * const mvmap2315_apmu_clk
					= (void *)MVMAP2315_APMU_CLK_BASE;

#define MVMAP2315_AP_RST_EN		BIT(0)
#define MVMAP2315_MCU_RST_EN		BIT(0)
struct mvmap2315_mpmu_clk_regs {
	u32 resetap;
	u32 resetmcu;
	u32 resetstatus;
	u8 _reserved0[4];
	u32 apaudiopllselect;
	u8 _reserved1[0x0c];
	u32 sspa_asrc_rx_clk0;
	u32 sspa_asrc_rx_clk1;
	u32 sspa_asrc_rx_clk2;
	u32 sspa_asrc_tx_clk0;
	u32 sspa_asrc_tx_clk1;
	u32 sspa_asrc_tx_clk2;
	u32 dmic_asrc_clk;
	u8 _reserved2[4];
	u32 uartfracdivcfg0;
	u8 _reserved3[0x0c];
	u32 uartfracdivcfg1;
	u8 _reserved4[0xcc];
	u32 clk32k_clkgenconfig;
	u32 clk32k_clkgenstatus;
	u8 _reserved5[0x08];
	u32 cpudbgclk_clkgenconfig;
	u32 cpudbgclk_clkgenstatus;
	u8 _reserved6[0x08];
	u32 m4clk_bist_clkgenconfig;
	u32 m4clk_bist_clkgenstatus;
	u8 _reserved7[0x08];
	u32 bspiclk_clkgenconfig;
	u32 bspiclk_clkgenstatus;
	u8 _reserved8[0x08];
	u32 dmicclk_clkgenconfig;
	u32 dmicclk_clkgenstatus;
	u8 _reserved9[0x48];
	u32 sspaclk0_clkgenconfig;
	u32 sspaclk0_clkgenstatus;
	u32 sspaclk1_clkgenconfig;
	u32 sspaclk1_clkgenstatus;
	u32 sspaclk2_clkgenconfig;
	u32 sspaclk2_clkgenstatus;
	u8 _reserved10[0x38];
	u32 mcuclk_clkgenconfig;
	u32 mcuclk_clkgenstatus;
	u8 _reserved11[0x08];
	u32 mcuclk_cdma_clkgenconfig;
	u32 mcuclk_cdma_clkgenstatus;
	u8 _reserved12[0x08];
	u32 mcuclk_bspi_clkgenconfig;
	u32 mcuclk_bspi_clkgenstatus;
	u8 _reserved13[0x08];
	u32 mcuclk_owi_clkgenconfig;
	u32 mcuclk_owi_clkgenstatus;
	u8 _reserved14[0x08];
	u32 mcuclk_uart0_clkgenconfig;
	u32 mcuclk_uart0_clkgenstatus;
	u8 _reserved15[0x08];
	u32 mcuclk_uart1_clkgenconfig;
	u32 mcuclk_uart1_clkgenstatus;
	u8 _reserved16[0x08];
	u32 mcuclk_ssp0_clkgenconfig;
	u32 mcuclk_ssp0_clkgenstatus;
	u8 _reserved17[0x08];
	u32 mcuclk_ssp1_clkgenconfig;
	u32 mcuclk_ssp1_clkgenstatus;
	u8 _reserved18[0x08];
	u32 mcuclk_sspa0_clkgenconfig;
	u32 mcuclk_sspa0_clkgenstatus;
	u8 _reserved19[0x08];
	u32 mcuclk_sspa1_clkgenconfig;
	u32 mcuclk_sspa1_clkgenstatus;
	u8 _reserved20[0x08];
	u32 mcuclk_sspa2_clkgenconfig;
	u32 mcuclk_sspa2_clkgenstatus;
	u8 _reserved21[0x08];
	u32 mcuclk_dmic0_clkgenconfig;
	u32 mcuclk_dmic0_clkgenstatus;
	u8 _reserved22[0x08];
	u32 mcuclk_dmic1_clkgenconfig;
	u32 mcuclk_dmic1_clkgenstatus;
	u8 _reserved23[0x08];
	u32 mcuclk_dmic2_clkgenconfig;
	u32 mcuclk_dmic2_clkgenstatus;
	u8 _reserved24[0x08];
	u32 mcuclk_dmic3_clkgenconfig;
	u32 mcuclk_dmic3_clkgenstatus;
	u8 _reserved25[0x18];
	u32 dmic_dclk0_clkgenconfig;
	u32 dmic_dclk0_clkgenstatus;
	u8 _reserved26[0x08];
	u32 dmic_dclk1_clkgenconfig;
	u32 dmic_dclk1_clkgenstatus;
	u8 _reserved27[0x08];
	u32 dmic_dclk2_clkgenconfig;
	u32 dmic_dclk2_clkgenstatus;
	u8 _reserved28[0x08];
	u32 dmic_dclk3_clkgenconfig;
	u32 dmic_dclk3_clkgenstatus;
	u8 _reserved29[0x08];
	u32 dmic_engdetclk_clkgenconfig;
	u32 dmic_engdetclk_clkgenstatus;
	u8 _reserved30[0x38];
	u32 refclk_clkgenconfig;
	u32 refclk_clkgenstatus;
	u8 _reserved31[0x08];
	u32 refclk_ssp0_clkgenconfig;
	u32 refclk_ssp0_clkgenstatus;
	u8 _reserved32[0x08];
	u32 refclk_ssp1_clkgenconfig;
	u32 refclk_ssp1_clkgenstatus;
	u8 _reserved33[0x08];
	u32 refclk_uart0_clkgenconfig;
	u32 refclk_uart0_clkgenstatus;
	u8 _reserved34[0x08];
	u32 refclk_uart1_clkgenconfig;
	u32 refclk_uart1_clkgenstatus;
	u8 _reserved35[0x08];
	u32 refclk_i2c0_clkgenconfig;
	u32 refclk_i2c0_clkgenstatus;
	u8 _reserved36[0x08];
	u32 refclk_i2c1_clkgenconfig;
	u32 refclk_i2c1_clkgenstatus;
	u8 _reserved37[0x08];
	u32 refclk_i2c2_clkgenconfig;
	u32 refclk_i2c2_clkgenstatus;
	u8 _reserved38[0x08];
	u32 refclk_i2c3_clkgenconfig;
	u32 refclk_i2c3_clkgenstatus;
	u8 _reserved39[0x08];
	u32 refclk_i2c4_clkgenconfig;
	u32 refclk_i2c4_clkgenstatus;
	u8 _reserved40[0x08];
	u32 refclk_i2c5_clkgenconfig;
	u32 refclk_i2c5_clkgenstatus;
	u8 _reserved41[0x08];
	u32 refclk_sspa0_clkgenconfig;
	u32 refclk_sspa0_clkgenstatus;
	u8 _reserved42[0x08];
	u32 refclk_sspa1_clkgenconfig;
	u32 refclk_sspa1_clkgenstatus;
	u8 _reserved43[0x08];
	u32 refclk_sspa2_clkgenconfig;
	u32 refclk_sspa2_clkgenstatus;
	u8 _reserved44[0x08];
	u32 tsenclk_clkgenconfig;
	u32 tsenclk_clkgenstatus;
	u8 _reserved45[0x08];
	u32 ap_tsenclk_clkgenconfig;
	u32 ap_tsenclk_clkgenstatus;
	u8 _reserved46[0x08];
	u32 sspa_mclk_clkgenconfig;
	u32 sspa_mclk_clkgenstatus;
};

check_member(mvmap2315_mpmu_clk_regs, sspa_mclk_clkgenstatus, 0x484);
static struct mvmap2315_mpmu_clk_regs * const mvmap2315_mpmu_clk
					= (void *)MVMAP2315_MPMU_CLK_BASE;

void clock_init(void);

#endif /* __SOC_MARVELL_MVMAP2315_CLOCK_H__ */
