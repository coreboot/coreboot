/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_MARVELL_MVMAP2315_APMU_H__
#define __SOC_MARVELL_MVMAP2315_APMU_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

enum pll_t {
	CPU_PLL = 93,
	MC_PLL = 94,
	MCFLC_PLL = 95,
	A2_PLL = 96,
	MAIN_PLL = 97,
	GPU_PLL0 = 98,
	GPU_PLL1 = 99,
	MIPI_PLL = 100,
	DISPLAY_PLL = 101,
	APLL0 = 102,
};

enum apll_freq_t {
	APLL_NOCHANGE = 0,
	APLL_589P824  = 1,
	APLL_541P9008 = 2,
	APLL_451P584  = 3
};

enum dev_statet_t {
	D0 = 0,
	D1 = 1,
	D2 = 2,
	D3 = 3,
	D3WAKEINT = 11
};

enum dev_t {
	APCPU_0 = 0,
	APCPU_L2 = 4,
	A2 = 5,
	MC = 6,
	DDRPHY_0 = 7,
	DDRPHY_1 = 8,
	DDRPHY_2 = 9,
	DDRPHY_3 = 10,
	MCFLC = 11,
	SDMMC = 50,
	AES256 = 54,
	AP_AXI_HS = 55,
	AP_UART0 = 56,
	AP_UART1 = 57,
	AP_M2M = 65,
	AP_APB = 66,
	AP_GIC = 67,
};

enum clk_t {
	M4CLK = 124,
};

enum clk_src_t {
	SRCSEL_NO_CHANGE =  0,
	SRCSEL_CPU_PLL =  1,
	SRCSEL_MC_PLL =  2,
	SRCSEL_MCFLC_PLL =  3,
	SRCSEL_A2_PLL =  4,
	SRCSEL_MAIN_PLL =  5,
	SRCSEL_GPU_PLL0 =  6,
	SRCSEL_GPU_PLL1 =  7,
	SRCSEL_MIPI_PLL =  8,
	SRCSEL_DISPLAY_PLL =  9,
	SRCSEL_APLL0 = 10,
	SRCSEL_APLL1 = 11,
};

enum clk_state_t {
	NOCHANGE = 0,
	GATED = 1,
	RUNNING = 2
};

#define MVMAP2315_APMU_PWRCMDFIFO_TIMEOUT		0x5000
#define MVMAP2315_APMU_PWRCMDFIFO_DONEIRQ		BIT(27)
#define MVMAP2315_APMU_ISTCLR_DONE			BIT(30)
#define MVMAP2315_APMU_ISTCLR_ERROR			BIT(31)

#define MVMAP2315_APMU_OPCODE_CLKST			1
#define MVMAP2315_APMU_OPCODE_PWRST			2
#define MVMAP2315_APMU_OPCODE_SHIFT			28

#define MVMAP2315_APMU_DEV_FREQ_SHIFT			12
#define MVMAP2315_APMU_DEV_STATE_SHIFT			8
#define MVMAP2315_APMU_DEV_OBJECT_SHIFT			0

#define MVMAP2315_APMU_CLK_STATE_SHIFT			25
#define MVMAP2315_APMU_CLK_DIV_SHIFT			12
#define MVMAP2315_APMU_CLK_SOURCE_SHIFT			8
#define MVMAP2315_APMU_CLK_OBJECT_SHIFT			0

struct mvmap2315_apmu_pwrctl_regs {
	 u32 pwrcmdfifo;
	 u32 pwrcmdfifostatus;
	 u8 _reserved0[0x08];
	 u32 intstatusset;
	 u32 intstatusclear;
	 u32 intstatus;
	 u8 _reserved1[0x04];
	 u32 doneirq;
	 u32 errorirq;
	 u32 scratch0;
	 u32 scratch1;
	 u8 _reserved2[0x10];
	 u32 pmp_spci_rx_int_status;
	 u32 pmp_spci_rx_int_set;
	 u32 pmp_spci_rx_int_clear;
	 u8 _reserved3[0x04];
	 u32 pmp_spci_tx_int_status;
	 u32 pmp_spci_tx_int_set;
	 u32 pmp_spci_tx_int_clear;
	 u8 _reserved4[0xa4];
	 u32 global_pwrstate;
	 u8 _reserved5[0x0c];
	 u32 a72core0_pwrstate;
	 u8 _reserved6[0x0c];
	 u32 a72core1_pwrstate;
	 u8 _reserved7[0x0c];
	 u32 a72core2_pwrstate;
	 u8 _reserved8[0x0c];
	 u32 a72core3_pwrstate;
	 u8 _reserved9[0x0c];
	 u32 a72l2_pwrstate;
	 u8 _reserved10[0x0c];
	 u32 a2fab_pwrstate;
	 u8 _reserved11[0x0c];
	 u32 mc_pwrstate;
	 u8 _reserved12[0x0c];
	 u32 mc_phy0_pwrstate;
	 u8 _reserved13[0x0c];
	 u32 mc_phy1_pwrstate;
	 u8 _reserved14[0x0c];
	 u32 mc_phy2_pwrstate;
	 u8 _reserved15[0x0c];
	 u32 mc_phy3_pwrstate;
	 u8 _reserved16[0x0c];
	 u32 mcflc_pwrstate;
	 u8 _reserved17[0x0c];
	 u32 mcflc_phy0_pwrstate;
	 u8 _reserved18[0x0c];
	 u32 mcflc_phy1_pwrstate;
	 u8 _reserved19[0x0c];
	 u32 mcflc_phy2_pwrstate;
	 u8 _reserved20[0x0c];
	 u32 mcflc_phy3_pwrstate;
	 u8 _reserved21[0x0c];
	 u32 gpu3dcore0_pwrstate;
	 u8 _reserved22[0x0c];
	 u32 gpu3dcore1_pwrstate;
	 u8 _reserved23[0x0c];
	 u32 gpu3dcore2_pwrstate;
	 u8 _reserved24[0x0c];
	 u32 gpu3dcore3_pwrstate;
	 u8 _reserved25[0x0c];
	 u32 gpu3dl2_pwrstate;
	 u8 _reserved26[0x0c];
	 u32 smmu_pwrstate;
	 u8 _reserved27[0x0c];
	 u32 vpudec_pwrstate;
	 u8 _reserved28[0x0c];
	 u32 jpeg_pwrstate;
	 u8 _reserved29[0x0c];
	 u32 decoder_pwrstate;
	 u8 _reserved30[0x0c];
	 u32 vpuenc_pwrstate;
	 u8 _reserved31[0x0c];
	 u32 vp8_pwrstate;
	 u8 _reserved32[0x0c];
	 u32 vp9_pwrstate;
	 u8 _reserved33[0x0c];
	 u32 zram_pwrstate;
	 u8 _reserved34[0x0c];
	 u32 gpu2d_pwrstate;
	 u8 _reserved35[0x0c];
	 u32 edisplay_pwrstate;
	 u8 _reserved36[0x0c];
	 u32 edp_phy_pwrstate;
	 u8 _reserved37[0x0c];
	 u32 dsi_phy0_pwrstate;
	 u8 _reserved38[0x0c];
	 u32 dsi_phy1_pwrstate;
	 u8 _reserved39[0x0c];
	 u32 display_pwrstate;
	 u8 _reserved40[0x0c];
	 u32 mci_x40_pwrstate;
	 u8 _reserved41[0x0c];
	 u32 mci_x4_phy0_pwrstate;
	 u8 _reserved42[0x0c];
	 u32 mci_x41_pwrstate;
	 u8 _reserved43[0x0c];
	 u32 mci_x4_phy1_pwrstate;
	 u8 _reserved44[0x0c];
	 u32 mci_x42_pwrstate;
	 u8 _reserved45[0x0c];
	 u32 mci_x4_phy2_pwrstate;
	 u8 _reserved46[0x0c];
	 u32 mci_x2_pwrstate;
	 u8 _reserved47[0x0c];
	 u32 mci_x2_phy_pwrstate;
	 u8 _reserved48[0x0c];
	 u32 mci_x10_pwrstate;
	 u8 _reserved49[0x0c];
	 u32 mci_x1_phy0_pwrstate;
	 u8 _reserved50[0x0c];
	 u32 mci_x11_pwrstate;
	 u8 _reserved51[0x0c];
	 u32 mci_x1_phy1_pwrstate;
	 u8 _reserved52[0x0c];
	 u32 mci_x12_pwrstate;
	 u8 _reserved53[0x0c];
	 u32 mci_x1_phy2_pwrstate;
	 u8 _reserved54[0x0c];
	 u32 mci_x13_pwrstate;
	 u8 _reserved55[0x0c];
	 u32 mci_x1_phy3_pwrstate;
	 u8 _reserved56[0x2c];
	 u32 sdmmc_pwrstate;
	 u8 _reserved57[0x0c];
	 u32 emmc_phy_pwrstate;
	 u8 _reserved58[0x0c];
	 u32 sdio_phy0_pwrstate;
	 u8 _reserved59[0x0c];
	 u32 sdio_phy1_pwrstate;
	 u8 _reserved60[0x0c];
	 u32 aes256_pwrstate;
	 u8 _reserved61[0x0c];
	 u32 ap_axi_pwrstate;
	 u8 _reserved62[0x1c];
	 u32 ap_uart0_pwrstate;
	 u8 _reserved63[0x0c];
	 u32 ap_uart1_pwrstate;
	 u8 _reserved64[0x0c];
	 u32 ap_ssp0_pwrstate;
	 u8 _reserved65[0x0c];
	 u32 ap_ssp1_pwrstate;
	 u8 _reserved66[0x0c];
	 u32 ap_i2c0_pwrstate;
	 u8 _reserved67[0x0c];
	 u32 ap_i2c1_pwrstate;
	 u8 _reserved68[0x0c];
	 u32 ap_i2c2_pwrstate;
	 u8 _reserved69[0x0c];
	 u32 ap_i2c3_pwrstate;
	 u8 _reserved70[0x0c];
	 u32 ap_i2c4_pwrstate;
	 u8 _reserved71[0x0c];
	 u32 ap_lcm_pwrstate;
	 u8 _reserved72[0x0c];
	 u32 ap_m2m_pwrstate;
	 u8 _reserved73[0x0c];
	 u32 ap_apb_pwrstate;
	 u8 _reserved74[0x0c];
	 u32 bcm_r4_pwrstate;
	 u8 _reserved75[0x0c];
	 u32 apmu_m4_pwrstate;
	 u8 _reserved76[0x0c];
	 u32 ap_gic_pwrstate;
	 u8 _reserved77[0x4c];
	 u32 cpu_pll_pwrstate;
	 u8 _reserved78[0x0c];
	 u32 mc_pll_pwrstate;
	 u8 _reserved79[0x0c];
	 u32 mcflc_pll_pwrstate;
	 u8 _reserved80[0x0c];
	 u32 a2_pll_pwrstate;
	 u8 _reserved81[0x0c];
	 u32 main_pll_pwrstate;
	 u8 _reserved82[0x0c];
	 u32 gpu_pll0_pwrstate;
	 u8 _reserved83[0x0c];
	 u32 gpu_pll1_pwrstate;
	 u8 _reserved84[0x0c];
	 u32 mipi_pll_pwrstate;
	 u8 _reserved85[0x0c];
	 u32 display_pll_pwrstate;
	 u8 _reserved86[0x0c];
	 u32 avs_vmain_pwrstate;
	 u8 _reserved87[0x0c];
	 u32 avs_vcpu_pwrstate;
	 u8 _reserved88[0x0c];
	 u32 avs_vgpu_pwrstate;
	 u8 _reserved89[0x0c];
	 u32 ap_tsene_pwrstate;
	 u8 _reserved90[0x0c];
	 u32 rng_pwrstate;
	 u8 _reserved91[0x3c];
	 u32 padgroup29_pwrstate;
	 u8 _reserved92[0x0c];
	 u32 padgroup30_pwrstate;
	 u8 _reserved93[0x0c];
	 u32 padgroup31_pwrstate;
	 u8 _reserved94[0x0c];
	 u32 padgroup32_pwrstate;
	 u8 _reserved95[0x0c];
	 u32 padgroup33_pwrstate;
};

check_member(mvmap2315_apmu_pwrctl_regs, padgroup33_pwrstate, 0x750);
static struct mvmap2315_apmu_pwrctl_regs * const mvmap2315_apmu_pwrctl
					= (void *)MVMAP2315_APMU_PWRCTL_BASE;

#define MVMAP2315_LCM_DIRACCESS_EN		BIT(0)
#define MVMAP2315_LCM_START_BANK		BIT(6)
#define MVMAP2315_LCM_END_BANK			(BIT(8) | BIT(10))
struct mvmap2315_lcm_regs {
	 u32 lcm_scfg;
	 u32 lcm_access;
	 u8 _reserved0[0x08];
	 u32 lcm_ctrl_bank_mstr_id_wr0;
	 u32 lcm_ctrl_bank_mstr_id_wr1;
	 u32 lcm_ctrl_bank_mstr_id_wr2;
	 u32 lcm_ctrl_bank_mstr_id_wr3;
	 u32 lcm_ctrl_bank_mstr_id_wr4;
	 u32 lcm_ctrl_bank_mstr_id_wr5;
	 u8 _reserved1[0x28];
	 u32 lcm_ctrl_bank_mask_id_wr0;
	 u32 lcm_ctrl_bank_mask_id_wr1;
	 u32 lcm_ctrl_bank_mask_id_wr2;
	 u32 lcm_ctrl_bank_mask_id_wr3;
	 u32 lcm_ctrl_bank_mask_id_wr4;
	 u32 lcm_ctrl_bank_mask_id_wr5;
	 u8 _reserved2[0x28];
	 u32 lcm_ctrl_bank_mstr_id_rd0;
	 u32 lcm_ctrl_bank_mstr_id_rd1;
	 u32 lcm_ctrl_bank_mstr_id_rd2;
	 u32 lcm_ctrl_bank_mstr_id_rd3;
	 u32 lcm_ctrl_bank_mstr_id_rd4;
	 u32 lcm_ctrl_bank_mstr_id_rd5;
	 u8 _reserved3[0x28];
	 u32 lcm_ctrl_bank_mask_id_rd0;
	 u32 lcm_ctrl_bank_mask_id_rd1;
	 u32 lcm_ctrl_bank_mask_id_rd2;
	 u32 lcm_ctrl_bank_mask_id_rd3;
	 u32 lcm_ctrl_bank_mask_id_rd4;
	 u32 lcm_ctrl_bank_mask_id_rd5;
};

check_member(mvmap2315_lcm_regs, lcm_ctrl_bank_mask_id_rd5, 0xe4);
static struct mvmap2315_lcm_regs * const mvmap2315_lcm_regs
					= (void *)MVMAP2315_LCM_REGS_BASE;

void apmu_start(void);
int apmu_set_pll(u32 dev, u32 state, u32 freq);
int apmu_set_dev(u32 dev, u32 state);
int apmu_set_clk(u32 clk, u32 state, u32 div, u32 src);

#endif /* __SOC_MARVELL_MVMAP2315_APMU_H__ */
