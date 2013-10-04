/*
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TEGRA124_CLK_RST_H_
#define _TEGRA124_CLK_RST_H_

/* PLL registers - there are several PLLs in the clock controller */
struct clk_pll {
	u32 pll_base;	/* the control register */
	/* pll_out[0] is output A control, pll_out[1] is output B control */
	u32 pll_out[2];
	u32 pll_misc;	/* other misc things */
};

/* PLL registers - there are several PLLs in the clock controller */
struct clk_pll_simple {
	u32 pll_base;		/* the control register */
	u32 pll_misc;		/* other misc things */
};

struct clk_pllm {
	u32 pllm_base;		/* the control register */
	u32 pllm_out;		/* output control */
	u32 pllm_misc1;	/* misc1 */
	u32 pllm_misc2;	/* misc2 */
};

/* RST_DEV_(L,H,U,V,W)_(SET,CLR) and CLK_ENB_(L,H,U,V,W)_(SET,CLR) */
struct clk_set_clr {
	u32 set;
	u32 clr;
};

/*
 * Most PLLs use the clk_pll structure, but some have a simpler two-member
 * structure for which we use clk_pll_simple. The reason for this non-
 * othogonal setup is not stated.
 */
enum {
	TEGRA_CLK_PLLS		= 6,	/* Number of normal PLLs */
	TEGRA_CLK_SIMPLE_PLLS	= 3,	/* Number of simple PLLs */
	TEGRA_CLK_REGS		= 3,	/* Number of clock enable regs L/H/U */
	TEGRA_CLK_SOURCES	= 64,	/* Number of ppl clock sources L/H/U */
	TEGRA_CLK_REGS_VW	= 2,	/* Number of clock enable regs V/W */
	TEGRA_CLK_SOURCES_VW	= 32,	/* Number of ppl clock sources V/W */
	TEGRA_CLK_SOURCES_X	= 32,	/* Number of ppl clock sources X */
};

/* Clock/Reset Controller (CLK_RST_CONTROLLER_) regs */
struct clk_rst_ctlr {
	u32 crc_rst_src;			/* _RST_SOURCE_0,0x00 */
	u32 crc_rst_dev[TEGRA_CLK_REGS];	/* _RST_DEVICES_L/H/U_0 */
	u32 crc_clk_out_enb[TEGRA_CLK_REGS];	/* _CLK_OUT_ENB_L/H/U_0 */
	u32 crc_reserved0;		/* reserved_0,		0x1C */
	u32 crc_cclk_brst_pol;		/* _CCLK_BURST_POLICY_0, 0x20 */
	u32 crc_super_cclk_div;	/* _SUPER_CCLK_DIVIDER_0,0x24 */
	u32 crc_sclk_brst_pol;		/* _SCLK_BURST_POLICY_0, 0x28 */
	u32 crc_super_sclk_div;	/* _SUPER_SCLK_DIVIDER_0,0x2C */
	u32 crc_clk_sys_rate;		/* _CLK_SYSTEM_RATE_0,	0x30 */
	u32 crc_reserved01;		/* reserved_0_1,	0x34 */
	u32 crc_reserved02;		/* reserved_0_2,	0x38 */
	u32 crc_reserved1;		/* reserved_1,		0x3C */
	u32 crc_cop_clk_skip_plcy;	/* _COP_CLK_SKIP_POLICY_0,0x40 */
	u32 crc_clk_mask_arm;		/* _CLK_MASK_ARM_0,	0x44 */
	u32 crc_misc_clk_enb;		/* _MISC_CLK_ENB_0,	0x48 */
	u32 crc_clk_cpu_cmplx;		/* _CLK_CPU_CMPLX_0,	0x4C */
	u32 crc_osc_ctrl;		/* _OSC_CTRL_0,		0x50 */
	u32 crc_pll_lfsr;		/* _PLL_LFSR_0,		0x54 */
	u32 crc_osc_freq_det;		/* _OSC_FREQ_DET_0,	0x58 */
	u32 crc_osc_freq_det_stat;	/* _OSC_FREQ_DET_STATUS_0,0x5C */
	u32 crc_reserved2[8];		/* reserved_2[8],	0x60-7C */

	struct clk_pll crc_pll[TEGRA_CLK_PLLS];	/* PLLs from 0x80 to 0xdc */

	/* PLLs from 0xe0 to 0xf4    */
	struct clk_pll_simple crc_pll_simple[TEGRA_CLK_SIMPLE_PLLS];

	u32 crc_reserved10;		/* _reserved_10,	0xF8 */
	u32 crc_reserved11;		/* _reserved_11,	0xFC */

	u32 crc_clk_src[TEGRA_CLK_SOURCES]; /*_I2S1_0...	0x100-1fc */

	u32 crc_reserved20[32];	/* _reserved_20,	0x200-27c */

	u32 crc_clk_out_enb_x;		/* _CLK_OUT_ENB_X_0,	0x280 */
	u32 crc_clk_enb_x_set;		/* _CLK_ENB_X_SET_0,	0x284 */
	u32 crc_clk_enb_x_clr;		/* _CLK_ENB_X_CLR_0,	0x288 */

	u32 crc_rst_devices_x;		/* _RST_DEVICES_X_0,	0x28c */
	u32 crc_rst_dev_x_set;		/* _RST_DEV_X_SET_0,	0x290 */
	u32 crc_rst_dev_x_clr;		/* _RST_DEV_X_CLR_0,	0x294 */

	u32 crc_reserved21[23];	/* _reserved_21,	0x298-2f0 */

	u32 crc_dfll_base;		/* _DFLL_BASE_0,	0x2f4 */

	u32 crc_reserved22[2];		/* _reserved_22,	0x2f8-2fc */

	/* _RST_DEV_L/H/U_SET_0 0x300 ~ 0x314 */
	struct clk_set_clr crc_rst_dev_ex[TEGRA_CLK_REGS];

	u32 crc_reserved30[2];		/* _reserved_30,	0x318, 0x31c */

	/* _CLK_ENB_L/H/U_CLR_0 0x320 ~ 0x334 */
	struct clk_set_clr crc_clk_enb_ex[TEGRA_CLK_REGS];

	u32 crc_reserved31;		/* _reserved_31,	0x338 */

	u32 crc_ccplex_pg_sm_ovrd;	/* _CCPLEX_PG_SM_OVRD_0,    0x33c */

	u32 crc_rst_cpu_cmplx_set;	/* _RST_CPU_CMPLX_SET_0,    0x340 */
	u32 crc_rst_cpu_cmplx_clr;	/* _RST_CPU_CMPLX_CLR_0,    0x344 */
	u32 crc_clk_cpu_cmplx_set;	/* _CLK_CPU_CMPLX_SET_0,    0x348 */
	u32 crc_clk_cpu_cmplx_clr;	/* _CLK_CPU_CMPLX_SET_0,    0x34c */

	u32 crc_reserved32[2];		/* _reserved_32,      0x350,0x354 */

	u32 crc_rst_dev_vw[TEGRA_CLK_REGS_VW]; /* _RST_DEVICES_V/W_0 */
	u32 crc_clk_out_enb_vw[TEGRA_CLK_REGS_VW]; /* _CLK_OUT_ENB_V/W_0 */
	u32 crc_cclkg_brst_pol;	/* _CCLKG_BURST_POLICY_0,   0x368 */
	u32 crc_super_cclkg_div;	/* _SUPER_CCLKG_DIVIDER_0,  0x36C */
	u32 crc_cclklp_brst_pol;	/* _CCLKLP_BURST_POLICY_0,  0x370 */
	u32 crc_super_cclkp_div;	/* _SUPER_CCLKLP_DIVIDER_0, 0x374 */
	u32 crc_clk_cpug_cmplx;	/* _CLK_CPUG_CMPLX_0,       0x378 */
	u32 crc_clk_cpulp_cmplx;	/* _CLK_CPULP_CMPLX_0,      0x37C */
	u32 crc_cpu_softrst_ctrl;	/* _CPU_SOFTRST_CTRL_0,     0x380 */
	u32 crc_cpu_softrst_ctrl1;	/* _CPU_SOFTRST_CTRL1_0,    0x384 */
	u32 crc_cpu_softrst_ctrl2;	/* _CPU_SOFTRST_CTRL2_0,    0x388 */
	u32 crc_reserved33[9];		/* _reserved_33,        0x38c-3ac */
	u32 crc_clk_src_vw[TEGRA_CLK_SOURCES_VW];	/* 0x3B0-0x42C */
	/* _RST_DEV_V/W_SET_0 0x430 ~ 0x43c */
	struct clk_set_clr crc_rst_dev_ex_vw[TEGRA_CLK_REGS_VW];
	/* _CLK_ENB_V/W_CLR_0 0x440 ~ 0x44c */
	struct clk_set_clr crc_clk_enb_ex_vw[TEGRA_CLK_REGS_VW];
	u32 crc_rst_cpug_cmplx_set;	/* _RST_CPUG_CMPLX_SET_0,   0x450*/
	u32 crc_rst_cpug_cmplx_clr;	/* _RST_CPUG_CMPLX_CLR_0,   0x454*/
	u32 crc_rst_cpulp_cmplx_set;	/* _RST_CPULP_CMPLX_SET_0,  0x458*/
	u32 crc_rst_cpulp_cmplx_clr;	/* _RST_CPULP_CMPLX_CLR_0,  0x45C*/
	u32 crc_clk_cpug_cmplx_set;	/* _CLK_CPUG_CMPLX_SET_0,  0x460 */
	u32 crc_clk_cpug_cmplx_clr;	/* _CLK_CPUG_CMPLX_CLR_0,  0x464 */
	u32 crc_clk_cpulp_cmplx_set;	/* _CLK_CPULP_CMPLX_SET_0, 0x468 */
	u32 crc_clk_cpulp_cmplx_clr;	/* _CLK_CPULP_CMPLX_CLR_0, 0x46C */
	u32 crc_cpu_cmplx_status;	/* _CPU_CMPLX_STATUS_0,    0x470 */
	u32 crc_reserved40[1];		/* _reserved_40,        0x474 */
	u32 crc_intstatus;		/* __INTSTATUS_0,       0x478 */
	u32 crc_intmask;		/* __INTMASK_0,         0x47C */
	u32 crc_utmip_pll_cfg0;	/* _UTMIP_PLL_CFG0_0,	0x480 */
	u32 crc_utmip_pll_cfg1;	/* _UTMIP_PLL_CFG1_0,	0x484 */
	u32 crc_utmip_pll_cfg2;	/* _UTMIP_PLL_CFG2_0,	0x488 */

	u32 crc_plle_aux;		/* _PLLE_AUX_0,		0x48C */
	u32 crc_sata_pll_cfg0;		/* _SATA_PLL_CFG0_0,	0x490 */
	u32 crc_sata_pll_cfg1;		/* _SATA_PLL_CFG1_0,	0x494 */
	u32 crc_pcie_pll_cfg0;		/* _PCIE_PLL_CFG0_0,	0x498 */

	u32 crc_prog_audio_dly_clk;	/* _PROG_AUDIO_DLY_CLK_0, 0x49C */
	u32 crc_audio_sync_clk_i2s0;	/* _AUDIO_SYNC_CLK_I2S0_0, 0x4A0 */
	u32 crc_audio_sync_clk_i2s1;	/* _AUDIO_SYNC_CLK_I2S1_0, 0x4A4 */
	u32 crc_audio_sync_clk_i2s2;	/* _AUDIO_SYNC_CLK_I2S2_0, 0x4A8 */
	u32 crc_audio_sync_clk_i2s3;	/* _AUDIO_SYNC_CLK_I2S3_0, 0x4AC */
	u32 crc_audio_sync_clk_i2s4;	/* _AUDIO_SYNC_CLK_I2S4_0, 0x4B0 */
	u32 crc_audio_sync_clk_spdif;	/* _AUDIO_SYNC_CLK_SPDIF_0, 0x4B4 */

	u32 crc_plld2_base;		/* _PLLD2_BASE_0, 0x4B8 */
	u32 crc_plld2_misc;		/* _PLLD2_MISC_0, 0x4BC */
	u32 crc_utmip_pll_cfg3;	/* _UTMIP_PLL_CFG3_0, 0x4C0 */
	u32 crc_pllrefe_base;		/* _PLLREFE_BASE_0, 0x4C4 */
	u32 crc_pllrefe_misc;		/* _PLLREFE_MISC_0, 0x4C8 */
	u32 crs_reserved_50[7];	/* _reserved_50, 0x4CC-0x4E4 */
	u32 crc_pllc2_base;		/* _PLLC2_BASE_0, 0x4E8 */
	u32 crc_pllc2_misc0;		/* _PLLC2_MISC_0_0, 0x4EC */
	u32 crc_pllc2_misc1;		/* _PLLC2_MISC_1_0, 0x4F0 */
	u32 crc_pllc2_misc2;		/* _PLLC2_MISC_2_0, 0x4F4 */
	u32 crc_pllc2_misc3;		/* _PLLC2_MISC_3_0, 0x4F8 */
	u32 crc_pllc3_base;		/* _PLLC3_BASE_0, 0x4FC */
	u32 crc_pllc3_misc0;		/* _PLLC3_MISC_0_0, 0x500 */
	u32 crc_pllc3_misc1;		/* _PLLC3_MISC_1_0, 0x504 */
	u32 crc_pllc3_misc2;		/* _PLLC3_MISC_2_0, 0x508 */
	u32 crc_pllc3_misc3;		/* _PLLC3_MISC_3_0, 0x50C */
	u32 crc_pllx_misc1;		/* _PLLX_MISC_1_0, 0x510 */
	u32 crc_pllx_misc2;		/* _PLLX_MISC_2_0, 0x514 */
	u32 crc_pllx_misc3;		/* _PLLX_MISC_3_0, 0x518 */
	u32 crc_xusbio_pll_cfg0;	/* _XUSBIO_PLL_CFG0_0, 0x51C */
	u32 crc_xusbio_pll_cfg1;	/* _XUSBIO_PLL_CFG0_1, 0x520 */
	u32 crc_plle_aux1;		/* _PLLE_AUX1_0, 0x524 */
	u32 crc_pllp_reshift;		/* _PLLP_RESHIFT_0, 0x528 */
	u32 crc_utmipll_hw_pwrdn_cfg0;	/* _UTMIPLL_HW_PWRDN_CFG0_0, 0x52C */
	u32 crc_pllu_hw_pwrdn_cfg0;	/* _PLLU_HW_PWRDN_CFG0_0, 0x530 */
	u32 crc_xusb_pll_cfg0;		/* _XUSB_PLL_CFG0_0, 0x534 */
	u32 crc_reserved51[1];		/* _reserved_51,     0x538 */
	u32 crc_clk_cpu_misc;		/* _CLK_CPU_MISC_0, 0x53C */
	u32 crc_clk_cpug_misc;		/* _CLK_CPUG_MISC_0, 0x540 */
	u32 crc_clk_cpulp_misc;	/* _CLK_CPULP_MISC_0, 0x544 */
	u32 crc_pllx_hw_ctrl_cfg;	/* _PLLX_HW_CTRL_CFG_0, 0x548 */
	u32 crc_pllx_sw_ramp_cfg;	/* _PLLX_SW_RAMP_CFG_0, 0x54C */
	u32 crc_pllx_hw_ctrl_status;	/* _PLLX_HW_CTRL_STATUS_0, 0x550 */
	u32 crc_reserved52[1];		/* _reserved_52,     0x554 */
	u32 crc_super_gr3d_clk_div;	/* _SUPER_GR3D_CLK_DIVIDER_0, 0x558 */
	u32 crc_spare_reg0;		/* _SPARE_REG0_0, 0x55C */

	/* T124 - skip to 0x600 here for new CLK_SOURCE_ regs */
	u32 crc_reserved60[40];	/* _reserved_60,     0x560 - 0x5FC */
	u32 crc_clk_src_x[TEGRA_CLK_SOURCES_X]; /* XUSB, etc, 0x600-0x678 */
};

#define TEGRA_DEV_L			0
#define TEGRA_DEV_H			1
#define TEGRA_DEV_U			2
#define TEGRA_DEV_V			0
#define TEGRA_DEV_W			1

#define SIMPLE_PLLX		(CLOCK_ID_XCPU - CLOCK_ID_FIRST_SIMPLE)

/* Bits to enable/reset modules */
#define CLK_ENB_CPU			(1 << 0)
#define SWR_TRIG_SYS_RST		(1 << 2)
#define SWR_CSITE_RST			(1 << 9)
#define CLK_ENB_CSITE			(1 << 9)

/* CRC_SUPER_CCLK_DIVIDER_0 0x24 */
#define SUPER_CDIV_ENB_ENABLE		(1 << 31)

/* CLK_RST_CONTROLLER_MISC_CLK_ENB 0x48 */
#define EN_PPSB_STOPCLK			(1 << 0)

/* CLK_RST_CONTROLLER_CLK_CPU_CMPLX_0 (0x4C) */
#define CPU3_CLK_STP_SHIFT		11
#define CPU2_CLK_STP_SHIFT		10
#define CPU1_CLK_STP_SHIFT		9
#define CPU0_CLK_STP_SHIFT		8
#define CPU0_CLK_STP_MASK		(1U << CPU0_CLK_STP_SHIFT)

/* CRC_OSC_CTRL_0 0x50 */
#define OSC_CTRL_OSC_FREQ		(0xf << 28)
#define OSC_CTRL_OSC_FREQ_SHIFT		28
#define OSC_FREQ_OSC13			0	/* 13.0MHz */
#define OSC_FREQ_OSC19P2		4	/* 19.2MHz */
#define OSC_FREQ_OSC12			8	/* 12.0MHz */
#define OSC_FREQ_OSC26			12	/* 26.0MHz */
#define OSC_FREQ_OSC16P8		1	/* 16.8MHz */
#define OSC_FREQ_OSC38P4		5	/* 38.4MHz */
#define OSC_FREQ_OSC48			9	/* 48.0MHz */

/* CLK_RST_CONTROLLER_PLLx_BASE_0 */
#define PLL_BYPASS_SHIFT		31
#define PLL_BYPASS_MASK			(1U << PLL_BYPASS_SHIFT)

#define PLL_ENABLE_SHIFT		30
#define PLL_ENABLE_MASK			(1U << PLL_ENABLE_SHIFT)

#define PLL_BASE_OVRRIDE_MASK		(1U << 28)
#define PLL_BASE_LOCK_MASK		(1U << 27)

#define PLL_DIVP_SHIFT			20
#define PLL_DIVP_MASK			(7U << PLL_DIVP_SHIFT)

#define PLL_DIVN_SHIFT			8
#define PLL_DIVN_MASK			(0x3ffU << PLL_DIVN_SHIFT)

#define PLL_DIVM_SHIFT			0
#define PLL_DIVM_MASK			(0x1f << PLL_DIVM_SHIFT)

/* SPECIAL CASE: PLLM, PLLC and PLLX use different-sized fields here */
#define PLLCMX_DIVP_MASK		(0xfU << PLL_DIVP_SHIFT)
#define PLLCMX_DIVN_MASK		(0xffU << PLL_DIVN_SHIFT)
#define PLLCMX_DIVM_MASK		(0xffU << PLL_DIVM_SHIFT)

/* CLK_RST_CONTROLLER_PLLx_OUTx_0 */
#define PLL_OUT_RSTN			(1 << 0)
#define PLL_OUT_CLKEN			(1 << 1)
#define PLL_OUT_OVRRIDE			(1 << 2)

#define PLL_OUT_RATIO_SHIFT		8
#define PLL_OUT_RATIO_MASK		(0xffU << PLL_OUT_RATIO_SHIFT)

#define PLL_OUT2_RSTN			(1 << 16)
#define PLL_OUT2_CLKEN			(1 << 17)
#define PLL_OUT2_OVRRIDE		(1 << 18)

#define PLL_OUT2_RATIO_SHIFT		24
#define PLL_OUT2_RATIO_MASK		(0xffU << PLL_OUT2_RATIO_SHIFT)

/* CLK_RST_CONTROLLER_PLLx_MISC_0 */
#define PLL_DCCON_SHIFT			20
#define PLL_DCCON_MASK			(1U << PLL_DCCON_SHIFT)

#define PLL_LOCK_ENABLE_SHIFT		18
#define PLL_LOCK_ENABLE_MASK		(1U << PLL_LOCK_ENABLE_SHIFT)

#define PLL_CPCON_SHIFT			8
#define PLL_CPCON_MASK			(15U << PLL_CPCON_SHIFT)

#define PLL_LFCON_SHIFT			4
#define PLL_LFCON_MASK			(15U << PLL_LFCON_SHIFT)

#define PLLU_VCO_FREQ_SHIFT		20
#define PLLU_VCO_FREQ_MASK		(1U << PLLU_VCO_FREQ_SHIFT)

#define PLLP_OUT1_OVR			(1 << 2)
#define PLLP_OUT2_OVR			(1 << 18)
#define PLLP_OUT3_OVR			(1 << 2)
#define PLLP_OUT4_OVR			(1 << 18)
#define PLLP_OUT1_RATIO			8
#define PLLP_OUT2_RATIO			24
#define PLLP_OUT3_RATIO			8
#define PLLP_OUT4_RATIO			24

#define PLLP_OUT3_RSTN_DIS		(1 << 0)
#define PLLP_OUT3_RSTN_EN		(0 << 0)
#define PLLP_OUT3_CLKEN			(1 << 1)
#define PLLP_OUT3_OVRRIDE		(1 << 2)
#define PLLP_OUT4_RSTN_DIS		(1 << 16)
#define PLLP_OUT4_RSTN_EN		(0 << 16)
#define PLLP_OUT4_CLKEN			(1 << 17)
#define PLLP_OUT4_OVRRIDE		(1 << 18)

enum {
	IN_408_OUT_204_DIVISOR = 2,
	IN_408_OUT_102_DIVISOR = 6,
	IN_408_OUT_48_DIVISOR = 15,
	IN_408_OUT_9_6_DIVISOR = 83,
};

/* CRC_PLLP_MISC_0 0xac */
#define PLLP_MISC_PLLP_CPCON_8		(8 << 8)
#define PLLP_MISC_PLLP_LOCK_ENABLE	(1 << 18)

/* CRC_PLLU_BASE_0 0xc0 */
#define PLLU_BYPASS_ENABLE		(1 << 31)
#define PLLU_ENABLE_ENABLE		(1 << 30)
#define PLLU_REF_DIS_REF_DISABLE	(1 << 29)
#define PLLU_OVERRIDE_ENABLE		(1 << 24)

/* CRC_PLLU_MISC_0 0xcc */
#define PLLU_LOCK_ENABLE_ENABLE		(1 << 22)

/* PLLX_BASE_0 0xe0 */
#define PLLX_BASE_PLLX_ENABLE		(1 << 30)

/* CLK_RST_CONTROLLER_PLLX_MISC_3 */
#define PLLX_IDDQ_SHIFT			3
#define PLLX_IDDQ_MASK			(1U << PLLX_IDDQ_SHIFT)

/* CLK_RST_CONTROLLER_OSC_CTRL_0 0x50 */
#define OSC_XOE_SHIFT			0
#define OSC_XOE_MASK			(1 << OSC_XOE_SHIFT)
#define OSC_XOE_ENABLE			(1 << OSC_XOE_SHIFT)
#define OSC_XOBP_SHIFT			1
#define OSC_XOBP_MASK			(1U << OSC_XOBP_SHIFT)
#define OSC_XOFS_SHIFT			4
#define OSC_XOFS_MASK			(0x3F << OSC_XOFS_SHIFT)
#define OSC_DRIVE_STRENGTH		7

/*
 * CLK_RST_CONTROLLER_CLK_SOURCE_x_OUT_0 - the mask here is normally 8 bits
 * but can be 16. We could use knowledge we have to restrict the mask in
 * the 8-bit cases (the divider_bits value returned by
 * get_periph_clock_source()) but it does not seem worth it since the code
 * already checks the ranges of values it is writing, in clk_get_divider().
 */
#define OUT_CLK_DIVISOR_SHIFT		0
#define OUT_CLK_DIVISOR_MASK		(0xffff << OUT_CLK_DIVISOR_SHIFT)

#define OUT_CLK_SOURCE_SHIFT		30
#define OUT_CLK_SOURCE_MASK		(3U << OUT_CLK_SOURCE_SHIFT)

#define OUT_CLK_SOURCE3_SHIFT		29
#define OUT_CLK_SOURCE3_MASK		(7U << OUT_CLK_SOURCE3_SHIFT)

#define OUT_CLK_SOURCE4_SHIFT		28
#define OUT_CLK_SOURCE4_MASK		(15U << OUT_CLK_SOURCE4_SHIFT)

/* CLK_RST_CONTROLLER_SCLK_BURST_POLICY */
#define SCLK_SYS_STATE_SHIFT		28U
#define SCLK_SYS_STATE_MASK		(15U << SCLK_SYS_STATE_SHIFT)
enum {
	SCLK_SYS_STATE_STDBY,
	SCLK_SYS_STATE_IDLE,
	SCLK_SYS_STATE_RUN,
	SCLK_SYS_STATE_IRQ = 4U,
	SCLK_SYS_STATE_FIQ = 8U,
};
#define SCLK_COP_FIQ_MASK		(1 << 27)
#define SCLK_CPU_FIQ_MASK		(1 << 26)
#define SCLK_COP_IRQ_MASK		(1 << 25)
#define SCLK_CPU_IRQ_MASK		(1 << 24)

#define SCLK_SWAKEUP_FIQ_SOURCE_SHIFT		12
#define SCLK_SWAKEUP_FIQ_SOURCE_MASK		\
		(7 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT)
#define SCLK_SWAKEUP_IRQ_SOURCE_SHIFT		8
#define SCLK_SWAKEUP_IRQ_SOURCE_MASK		\
		(7 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT)
#define SCLK_SWAKEUP_RUN_SOURCE_SHIFT		4
#define SCLK_SWAKEUP_RUN_SOURCE_MASK		\
		(7 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT)
#define SCLK_SWAKEUP_IDLE_SOURCE_SHIFT		0

#define SCLK_SWAKEUP_IDLE_SOURCE_MASK		\
		(7 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT)
enum {
	SCLK_SOURCE_CLKM,
	SCLK_SOURCE_PLLC_OUT1,
	SCLK_SOURCE_PLLP_OUT4,
	SCLK_SOURCE_PLLP_OUT3,
	SCLK_SOURCE_PLLP_OUT2,
	SCLK_SOURCE_CLKD,
	SCLK_SOURCE_CLKS,
	SCLK_SOURCE_PLLM_OUT1,
};

#define SCLK_SWAKE_FIQ_SRC_CLKM		(0 << 12)
#define SCLK_SWAKE_IRQ_SRC_CLKM		(0 << 8)
#define SCLK_SWAKE_RUN_SRC_CLKM		(0 << 4)
#define SCLK_SWAKE_IDLE_SRC_CLKM	(0 << 0)
#define SCLK_SWAKE_FIQ_SRC_PLLM_OUT1	(7 << 12)
#define SCLK_SWAKE_IRQ_SRC_PLLM_OUT1	(7 << 8)
#define SCLK_SWAKE_RUN_SRC_PLLM_OUT1	(7 << 4)
#define SCLK_SWAKE_IDLE_SRC_PLLM_OUT1	(7 << 0)

/* CLK_RST_CONTROLLER_SUPER_SCLK_DIVIDER 0x2c */
#define SUPER_SCLK_ENB_SHIFT		31U
#define SUPER_SCLK_ENB_MASK		(1U << 31)
#define SUPER_SCLK_DIVIDEND_SHIFT	8
#define SUPER_SCLK_DIVIDEND_MASK	(0xff << SUPER_SCLK_DIVIDEND_SHIFT)
#define SUPER_SCLK_DIVISOR_SHIFT	0
#define SUPER_SCLK_DIVISOR_MASK		(0xff << SUPER_SCLK_DIVISOR_SHIFT)

/* CLK_RST_CONTROLLER_CLK_SYSTEM_RATE 0x30 */
#define CLK_SYS_RATE_HCLK_DISABLE_SHIFT 7
#define CLK_SYS_RATE_HCLK_DISABLE_MASK  (1 << CLK_SYS_RATE_HCLK_DISABLE_SHIFT)
#define CLK_SYS_RATE_AHB_RATE_SHIFT     4
#define CLK_SYS_RATE_AHB_RATE_MASK      (3 << CLK_SYS_RATE_AHB_RATE_SHIFT)
#define CLK_SYS_RATE_PCLK_DISABLE_SHIFT 3
#define CLK_SYS_RATE_PCLK_DISABLE_MASK  (1 << CLK_SYS_RATE_PCLK_DISABLE_SHIFT)
#define CLK_SYS_RATE_APB_RATE_SHIFT     0
#define CLK_SYS_RATE_APB_RATE_MASK      (3 << CLK_SYS_RATE_AHB_RATE_SHIFT)

/* CLK_RST_CONTROLLER_RST_CPUxx_CMPLX_CLR 0x344 */
#define CLR_CPURESET0			(1 << 0)
#define CLR_CPURESET1			(1 << 1)
#define CLR_CPURESET2			(1 << 2)
#define CLR_CPURESET3			(1 << 3)
#define CLR_DBGRESET0			(1 << 12)
#define CLR_DBGRESET1			(1 << 13)
#define CLR_DBGRESET2			(1 << 14)
#define CLR_DBGRESET3			(1 << 15)
#define CLR_CORERESET0			(1 << 16)
#define CLR_CORERESET1			(1 << 17)
#define CLR_CORERESET2			(1 << 18)
#define CLR_CORERESET3			(1 << 19)
#define CLR_CXRESET0			(1 << 20)
#define CLR_CXRESET1			(1 << 21)
#define CLR_CXRESET2			(1 << 22)
#define CLR_CXRESET3			(1 << 23)
#define CLR_L2RESET			(1 << 24)
#define CLR_NONCPURESET			(1 << 29)
#define CLR_PRESETDBG			(1 << 30)

/* CLK_RST_CONTROLLER_CLK_CPU_CMPLX_CLR 0x34c */
#define CLR_CPU3_CLK_STP		(1 << 11)
#define CLR_CPU2_CLK_STP		(1 << 10)
#define CLR_CPU1_CLK_STP		(1 << 9)
#define CLR_CPU0_CLK_STP		(1 << 8)

/* CRC_CLK_SOURCE_MSELECT_0 0x3b4 */
#define MSELECT_CLK_SRC_PLLP_OUT0	(0 << 29)

/* CRC_CLK_ENB_V_SET_0 0x440 */
#define SET_CLK_ENB_CPUG_ENABLE		(1 << 0)
#define SET_CLK_ENB_CPULP_ENABLE	(1 << 1)
#define SET_CLK_ENB_MSELECT_ENABLE	(1 << 3)

/* CLK_RST_CONTROLLER_UTMIP_PLL_CFG1_0 0x484 */
#define PLLU_POWERDOWN			(1 << 16)
#define PLL_ENABLE_POWERDOWN		(1 << 14)
#define PLL_ACTIVE_POWERDOWN		(1 << 12)

/* CLK_RST_CONTROLLER_UTMIP_PLL_CFG2_0 0x488 */
#define UTMIP_FORCE_PD_SAMP_C_POWERDOWN	(1 << 4)
#define UTMIP_FORCE_PD_SAMP_B_POWERDOWN	(1 << 2)
#define UTMIP_FORCE_PD_SAMP_A_POWERDOWN	(1 << 0)

#endif	/* _TEGRA124_CLK_RST_H_ */
