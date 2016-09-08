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

#ifndef __SOC_MARVELL_MVMAP2315_A2BUS_H__
#define __SOC_MARVELL_MVMAP2315_A2BUS_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

#define MVMAP2315_CCU_WIN1_ALR				0x00000000
#define MVMAP2315_CCU_WIN1_AHR				0x000007FF
#define MVMAP2315_CCU_WIN1_UID				0x0000000B

#define MVMAP2315_CCU_WIN2_ALR				0x00000000
#define MVMAP2315_CCU_WIN2_AHR				0x000007FF
#define MVMAP2315_CCU_WIN2_UID				0x00000020

#define MVMAP2315_CCU_WIN3_ALR				0x00000800
#define MVMAP2315_CCU_WIN3_AHR				0x00000EFF
#define MVMAP2315_CCU_WIN3_UID				0x00000002

#define MVMAP2315_CCU_WIN4_ALR				0x00008000
#define MVMAP2315_CCU_WIN4_AHR				0x0000BFFF
#define MVMAP2315_CCU_WIN4_UID				0x0000000B

#define MVMAP2315_CCU_RGF_WIN0_ALR			0x00000010
#define MVMAP2315_CCU_RGF_WIN0_AHR			0x000001FF
#define MVMAP2315_CCU_RGF_WIN0_UID			0x00000006

#define MVMAP2315_CCU_RGF_WIN1_ALR			0x00000200
#define MVMAP2315_CCU_RGF_WIN1_AHR			0x000004ff
#define MVMAP2315_CCU_RGF_WIN1_UID			0x00000003

#define MVMAP2315_CCU_RGF_WIN2_ALR			0x00000500
#define MVMAP2315_CCU_RGF_WIN2_AHR			0x0000060F
#define MVMAP2315_CCU_RGF_WIN2_UID			0x0000000C

#define MVMAP2315_CCU_RGF_WIN3_ALR			0x00000680
#define MVMAP2315_CCU_RGF_WIN3_AHR			0x0000068F
#define MVMAP2315_CCU_RGF_WIN3_UID			0x00000001

#define MVMAP2315_CCU_RGF_WIN4_ALR			0x00000700
#define MVMAP2315_CCU_RGF_WIN4_AHR			0x0000073F
#define MVMAP2315_CCU_RGF_WIN4_UID			0x00000000

#define MVMAP2315_CCU_RGF_WIN5_ALR			0x00000800
#define MVMAP2315_CCU_RGF_WIN5_AHR			0x000008FF
#define MVMAP2315_CCU_RGF_WIN5_UID			0x00000005

#define MVMAP2315_CCU_RGF_WIN6_ALR			0x00000A00
#define MVMAP2315_CCU_RGF_WIN6_AHR			0x00000FFF
#define MVMAP2315_CCU_RGF_WIN6_UID			0x0000000F

#define MVMAP2315_CCU_RGF_WIN7_ALR			0x00001000
#define MVMAP2315_CCU_RGF_WIN7_AHR			0x000017FF
#define MVMAP2315_CCU_RGF_WIN7_UID			0x0000000D

#define MVMAP2315_CCU_WIN_ALR_ADDRLOW_SHIFT		4
#define MVMAP2315_CCU_WIN_AHR_ADDRHIGH_SHIFT		4
#define MVMAP2315_CCU_WIN_CR_WINTARGET_SHIFT		8
#define MVMAP2315_CCU_CR_WINEN				BIT(0)
#define MVMAP2315_CCU_RGF_WIN_ADDR_HI_SHIFT		19
#define MVMAP2315_CCU_RGF_WIN_ADDR_LO_SHIFT		6
#define MVMAP2315_CCU_RGF_WIN_UID_SHIFT			2
#define MVMAP2315_CCU_RGF_WIN_RGFWINEN			BIT(0)
#define MVMAP2315_CCU_WIN_CR_WINTARGET_MASK		0x7f00
#define MVMAP2315_CCU_WIN_ALR_ADDRLOW_MASK		0xfffff0
#define MVMAP2315_CCU_WIN_ALR_ADDRHIGH_MASK		0xfffff0
struct mvmap2315_a2bus_ccu_regs {
	u32 ccu_win0_cr;
	u32 ccu_win0_scr;
	u32 ccu_win0_alr;
	u32 ccu_win0_ahr;
	u32 ccu_win1_cr;
	u32 ccu_win1_scr;
	u32 ccu_win1_alr;
	u32 ccu_win1_ahr;
	u32 ccu_win2_cr;
	u32 ccu_win2_scr;
	u32 ccu_win2_alr;
	u32 ccu_win2_ahr;
	u32 ccu_win3_cr;
	u32 ccu_win3_scr;
	u32 ccu_win3_alr;
	u32 ccu_win3_ahr;
	u32 ccu_win4_cr;
	u32 ccu_win4_scr;
	u32 ccu_win4_alr;
	u32 ccu_win4_ahr;
	u32 ccu_win5_cr;
	u32 ccu_win5_scr;
	u32 ccu_win5_alr;
	u32 ccu_win5_ahr;
	u32 ccu_win6_cr;
	u32 ccu_win6_scr;
	u32 ccu_win6_alr;
	u32 ccu_win6_ahr;
	u32 ccu_win7_cr;
	u32 ccu_win7_scr;
	u32 ccu_win7_alr;
	u32 ccu_win7_ahr;
	u8 _reserved0[0x10];
	u32 ccu_rgf_win0;
	u32 ccu_rgf_win1;
	u32 ccu_rgf_win2;
	u32 ccu_rgf_win3;
	u32 ccu_rgf_win4;
	u32 ccu_rgf_win5;
	u32 ccu_rgf_win6;
	u32 ccu_rgf_win7;
	u32 ccu_rgf_win8;
	u32 ccu_rgf_win9;
	u32 ccu_rgf_win10;
	u32 ccu_rgf_win11;
	u32 ccu_rgf_win12;
	u32 ccu_rgf_win13;
	u32 ccu_rgf_win14;
	u32 ccu_rgf_win15;
	u32 ccu_win_gcr;
	u32 ccu_rgf_sidr0;
	u32 ccu_rgf_sidr1;
	u8 _reserved1[0x04];
	u32 ccu_rar0;
	u32 ccu_rar1;
	u8 _reserved2[0x18];
	u32 ccu_sl_hmr;
	u32 ccu_sl_mimr0;
	u32 ccu_sl_mimr1;
	u32 ccu_sl_revid;
	u32 ccu_sl_dtpr0;
	u32 ccu_sl_dtpr1;
	u32 ccu_sl_dtpr2;
	u32 ccu_sl_dtpr3;
	u32 ccu_sl_dtpr4;
	u32 ccu_sl_dtpr5;
	u32 ccu_sl_dtpr6;
	u32 ccu_sl_dtpr7;
	u32 ccu_sl_dtpr8;
	u32 ccu_sl_dtpr9;
	u32 ccu_sl_dtpr10;
	u32 ccu_sl_dtpr11;
	u32 ccu_sl_dtpr12;
	u32 ccu_sl_dtpr13;
	u32 ccu_sl_dtpr14;
	u32 ccu_sl_dtpr15;
	u8 _reserved3[0xb0];
	u32 ccu_htc_cr;
	u32 ccu_htc_sr;
	u32 ccu_htc_mnt_cr;
	u8 _reserved4[0x04];
	u32 ccu_htc_acr0;
	u32 ccu_htc_acr1;
	u32 ccu_htc_acr2;
	u32 ccu_htc_acr3;
	u32 ccu_htc_acr4;
	u32 ccu_htc_acr5;
	u32 ccu_htc_acr6;
	u32 ccu_htc_acr7;
	u32 ccu_htc_acr8;
	u32 ccu_htc_acr9;
	u32 ccu_htc_acr10;
	u32 ccu_htc_acr11;
	u32 ccu_htc_acr12;
	u32 ccu_htc_acr13;
	u32 ccu_htc_acr14;
	u32 ccu_htc_acr15;
	u8 _reserved5[0x10];
	u32 ccu_htc_aasr;
	u32 ccu_htc_aset;
	u32 ccu_htc_aclr;
	u32 ccu_sft_eir0;
	u32 ccu_sft_eir1;
	u8 _reserved6[0x8c];
	u32 ccu_ltc_cr;
	u32 ccu_ltc_sr;
	u32 ccu_ltc_hhr;
	u32 ccu_ltc_cr1;
	u8 _reserved7[0xe0];
	u32 ccu_gspmu_cr;
	u32 ccu_gspmu_sr;
	u8 _reserved8[0x18];
	u32 ccu_mc_cr;
	u8 _reserved9[0x3ec];
	u32 ccu_rgf_cr;
	u32 ccu_rgf_scr;
};

check_member(mvmap2315_a2bus_ccu_regs, ccu_rgf_scr, 0x804);
static struct mvmap2315_a2bus_ccu_regs * const mvmap2315_a2bus_ccu
					= (void *)MVMAP2315_A2BUS_CCU_BASE;

#define MVMAP2315_CCU_B_LTC_CR_USERDMODROB		BIT(0)
#define MVMAP2315_CCU_B_IDACR_PRIVATEUID_SHIFT		0
#define MVMAP2315_CCU_B_SMC_MVN_MVNENABLE		BIT(0)
struct mvmap2315_a2bus_regs {
	u32 ccu_b_uid;
	u32 ccu_b_urd;
	u32 ccu_b_sfwd;
	u8 _reserved0[0x04];
	u32 ccu_b_rapc;
	u32 ccu_b_dlapc;
	u8 _reserved1[0x08];
	u32 ccu_b_rsc;
	u32 ccu_b_rsu;
	u8 _reserved2[0x08];
	u32 ccu_b_idacr;
	u8 _reserved3[0x0c];
	u32 ccu_b_rsrv0;
	u8 _reserved4[0x0c];
	u32 ccu_dbg_ltc_cr;
	u32 ccu_dbg_ltc_sr;
	u8 _reserved5[0x08];
	u32 ccu_dbg_htc_cr;
	u32 ccu_dbg_htc_sr;
	u8 _reserved6[0x08];
	u32 ccu_dbg_rsu_cr;
	u32 ccu_dbg_rsu_sr;
	u8 _reserved7[0x08];
	u32 ccu_dbg_smc_cr;
	u32 ccu_dbg_smc_sr;
	u8 _reserved8[0x98];
	u32 ccu_b_reicr;
	u32 ccu_b_reimr;
	u32 ccu_b_seicr;
	u32 ccu_b_seimr;
	u8 _reserved9[0xd0];
	u32 ccu_b_mpccr;
	u32 ccu_b_pcr;
	u32 ccu_b_drar;
	u32 ccu_b_mpidr;
	u32 ccu_b_pdcr;
	u32 ccu_b_evar;
	u32 ccu_b_l2efr;
	u8 _reserved10[0x04];
	u32 ccu_b_psr;
	u32 ccu_b_mpsr;
	u32 ccu_b_adb_cr;
	u32 ccu_b_adb_sr;
	u32 ccu_b_pdr_sr;
	u32 ccu_b_pdr_cr;
	u32 ccu_b_l2dr_sr;
	u32 ccu_b_l2dr_cr;
	u32 ccu_b_rvbar0;
	u32 ccu_b_rvbar1;
	u32 ccu_b_rvbar2;
	u32 ccu_b_rvbar3;
	u32 ccu_b_prcr0;
	u32 ccu_b_prcr1;
	u32 ccu_b_prcr2;
	u32 ccu_b_prcr3;
	u32 ccu_b_wrcr0;
	u32 ccu_b_wrcr1;
	u32 ccu_b_wrcr2;
	u32 ccu_b_wrcr3;
	u32 ccu_b_drcr0;
	u32 ccu_b_drcr1;
	u32 ccu_b_drcr2;
	u32 ccu_b_drcr3;
	u32 ccu_b_crcr;
	u32 ccu_b_rcr;
	u8 _reserved11[0x78];
	u32 ccu_mc_rcr;
	u32 ccu_mc_rsbr;
	u32 ccu_mc_rtbr;
	u32 ccu_b_smc;
	u32 ccu_b_smc_mvn;
	u8 _reserved12[0x2c];
	u32 ccu_b_ltc_sr;
	u32 ccu_b_ltc_cr;
	u32 ccu_b_ltc_qos_ovrd;
	u32 qos_time_seg_cr;
	u32 qos_bw_guarantee_th;
	u32 qos_bw_peak_th;
	u32 qos_bw_jam_th;
	u32 qos_regulator_out_trans_block_cr0;
	u32 qos_prio_cr;
	u32 qos_critical_path_cr;
	u32 qos_regulator_out_trans_block_cr1;
	u32 ccu_b_ltc_tc_ovrd;
	u8 _reserved13[0x30];
	u32 ccu_b_htc_sr;
	u32 ccu_b_htc_prio_dl;
	u32 ccu_b_sp_prio_mesh_lo;
	u32 ccu_b_sp_prio_mesh_hi;
	u32 ccu_b_sft_far_lo;
	u32 ccu_b_sft_far_hi;
	u32 ccu_b_htc_mnt_sr;
};

check_member(mvmap2315_a2bus_regs, ccu_b_htc_mnt_sr, 0x3b8);
static struct mvmap2315_a2bus_regs * const mvmap2315_a2bus_banked
				= (void *)MVMAP2315_A2BUS_BANKED_BASE;
static struct mvmap2315_a2bus_regs * const mvmap2315_a2bus_alias6
				= (void *)MVMAP2315_A2BUS_ALIAS6_BASE;
static struct mvmap2315_a2bus_regs * const mvmap2315_a2bus_alias9
				= (void *)MVMAP2315_A2BUS_ALIAS9_BASE;
static struct mvmap2315_a2bus_regs * const mvmap2315_a2bus_alias11
				= (void *)MVMAP2315_A2BUS_ALIAS11_BASE;

void init_a2bus_config(void);

#endif /* __SOC_MARVELL_MVMAP2315_A2BUS_H__ */
