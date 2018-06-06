/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#ifndef __SOC_MEDIATEK_MT8173_MCUCFG_H__
#define __SOC_MEDIATEK_MT8173_MCUCFG_H__

#include <soc/addressmap.h>

struct mt8173_mcucfg_regs {
	u32 mp0_ca7l_cache_config;
	struct {
		u32 mem_delsel0;
		u32 mem_delsel1;
	} mp0_cpu[4];
	u32 mp0_cache_mem_delsel0;
	u32 mp0_cache_mem_delsel1;
	u32 mp0_axi_config;
	u32 mp0_misc_config[10];
	u32 mp0_ca7l_cfg_dis;
	u32 mp0_ca7l_clken_ctrl;
	u32 mp0_ca7l_rst_ctrl;
	u32 mp0_ca7l_misc_config;
	u32 mp0_ca7l_dbg_pwr_ctrl;
	u32 mp0_rw_rsvd0;
	u32 mp0_rw_rsvd1;
	u32 mp0_ro_rsvd;
	u32 reserved0_0[100];
	u32 mp1_cpucfg;
	u32 mp1_miscdbg;
	u32 reserved0_1[13];
	u32 mp1_rst_ctl;
	u32 mp1_clkenm_div;
	u32 reserved0_2[7];
	u32 mp1_config_res;
	u32 reserved0_3[101];
	u32 mp0_rst_status;			/* 0x400 */
	u32 mp0_dbg_ctrl;
	u32 mp0_dbg_flag;
	u32 mp0_ca7l_ir_mon;
	struct {
		u32 pc_lw;
		u32 pc_hw;
		u32 fp_arch32;
		u32 sp_arch32;
		u32 fp_arch64_lw;
		u32 fp_arch64_hw;
		u32 sp_arch64_lw;
		u32 sp_arch64_hw;
	} mp0_dbg_core[4];
	u32 dfd_ctrl;
	u32 dfd_cnt_l;
	u32 dfd_cnt_h;
	u32 misccfg_mp0_rw_rsvd;
	u32 misccfg_sec_vio_status0;
	u32 misccfg_sec_vio_status1;
	u32 reserved1[22];
	u32 misccfg_rw_rsvd;			/* 0x500 */
	u32 mcusys_dbg_mon_sel_a;
	u32 mcusys_dbg_mon;
	u32 reserved2[61];
	u32 mcusys_config_a;			/* 0x600 */
	u32 mcusys_config1_a;
	u32 mcusys_gic_peribase_a;
	u32 reserved3;
	u32 sec_range0_start;			/* 0x610 */
	u32 sec_range0_end;
	u32 sec_range_enable;
	u32 reserved4;
	u32 int_pol_ctl[8];			/* 0x620 */
	u32 aclken_div;				/* 0x640 */
	u32 pclken_div;
	u32 l2c_sram_ctrl;
	u32 armpll_jit_ctrl;
	u32 cci_addrmap;			/* 0x650 */
	u32 cci_config;
	u32 cci_periphbase;
	u32 cci_nevntcntovfl;
	u32 cci_clk_ctrl;			/* 0x660 */
	u32 cci_acel_s1_ctrl;
	u32 bus_fabric_dcm_ctrl;
	u32 reserved5;
	u32 xgpt_ctl;				/* 0x670 */
	u32 xgpt_idx;
	u32 ptpod2_ctl0;
	u32 ptpod2_ctl1;
	u32 mcusys_revid;
	u32 mcusys_rw_rsvd0;
	u32 mcusys_rw_rsvd1;
};

check_member(mt8173_mcucfg_regs, mcusys_rw_rsvd1, 0x688);

static struct mt8173_mcucfg_regs *const mt8173_mcucfg = (void *)MCUCFG_BASE;

#endif  /* __SOC_MEDIATEK_MT8173_MCUCFG_H__ */
