/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef SOC_MEDIATEK_MT8183_SMI_H
#define SOC_MEDIATEK_MT8183_SMI_H

#include <soc/addressmap.h>
#include <types.h>

struct mt8183_smi_regs {
	u32 reserved1[64];
	u32 smi_l1len;
	u32 smi_l1arb0;
	u32 smi_l1arb1;
	u32 smi_l1arb2;
	u32 smi_l1arb3;
	u32 smi_l1arb4;
	u32 smi_l1arb5;
	u32 smi_l1arb6;
	u32 smi_l1arb7;
	u32 reserved2[31];
	u32 smi_mon_axi_ena;
	u32 smi_mon_axi_clr;
	u32 reserved3[1];
	u32 smi_mon_axi_type;
	u32 smi_mon_axi_con;
	u32 reserved4[3];
	u32 smi_mon_axi_act_cnt;
	u32 smi_mon_axi_req_cnt;
	u32 smi_mon_axi_ostd_cnt;
	u32 smi_mon_axi_bea_cnt;
	u32 smi_mon_axi_byt_cnt;
	u32 smi_mon_axi_cp_cnt;
	u32 smi_mon_axi_dp_cnt;
	u32 smi_mon_axi_cp_max;
	u32 smi_mon_axi_cos_max;
	u32 reserved5[15];
	u32 smi_bus_sel;
	u32 reserved6[1];
	u32 smi_wrr_reg0;
	u32 smi_wrr_reg1;
	u32 smi_read_fifo_th;
	u32 smi_m4u_th;
	u32 smi_fifo_th1;
	u32 smi_fifo_th2;
	u32 smi_preultra_mask0;
	u32 smi_preultra_mask1;
	u32 reserved7[46];
	u32 smi_dcm;
	u32 smi_ela;
	u32 smi_m1_rultra_wrr0;
	u32 smi_m1_rultra_wrr1;
	u32 smi_m1_wultra_wrr0;
	u32 smi_m1_wultra_wrr1;
	u32 smi_m2_rultra_wrr0;
	u32 smi_m2_rultra_wrr1;
	u32 smi_m2_wultra_wrr0;
	u32 smi_m2_wultra_wrr1;
	u32 reserved8[38];
	u32 smi_common_clamp_en;
	u32 smi_common_clamp_en_set;
	u32 smi_common_clamp_en_clr;
	u32 reserved9[13];
	u32 smi_debug_s0;
	u32 smi_debug_s1;
	u32 smi_debug_s2;
	u32 smi_debug_s3;
	u32 smi_debug_s4;
	u32 smi_debug_s5;
	u32 smi_debug_s6;
	u32 smi_debug_s7;
	u32 reserved10[4];
	u32 smi_debug_m0;
	u32 smi_debug_m1;
	u32 reserved11[2];
	u32 smi_debug_misc;
	u32 smi_dummy;
	u32 reserved12[46];
	u32 smi_hist_rec0;
	u32 smi_hist_rec_data0;
	u32 smi_hist_rec_data1;
	u32 smi_hist_rec_data2;
	u32 smi_hist_rec_data3;
	u32 smi_hist_rec_data4;
	u32 smi_hist_rec_data5;
	u32 smi_hist_rec_data6;
	u32 smi_hist_rec_data7;
	u32 smi_hist_rec_data8;
	u32 smi_hist_rec_data9;
};

check_member(mt8183_smi_regs, smi_l1len, 0x0100);
check_member(mt8183_smi_regs, smi_mon_axi_ena, 0x01a0);
check_member(mt8183_smi_regs, smi_mon_axi_act_cnt, 0x01c0);
check_member(mt8183_smi_regs, smi_bus_sel, 0x0220);
check_member(mt8183_smi_regs, smi_dcm, 0x0300);
check_member(mt8183_smi_regs, smi_common_clamp_en, 0x03c0);
check_member(mt8183_smi_regs, smi_debug_s0, 0x0400);
check_member(mt8183_smi_regs, smi_debug_m0, 0x0430);
check_member(mt8183_smi_regs, smi_debug_misc, 0x0440);
check_member(mt8183_smi_regs, smi_hist_rec0, 0x0500);
check_member(mt8183_smi_regs, smi_hist_rec_data9, 0x0528);

static struct mt8183_smi_regs *const mt8183_smi = (void *)SMI_BASE;

#endif  /* SOC_MEDIATEK_MT8183_SMI_H */
