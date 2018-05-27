/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2010 - 2013, NVIDIA CORPORATION.  All rights reserved.
 * Copyright (C) 2013 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA124_MC_H__
#define __SOC_NVIDIA_TEGRA124_MC_H__

#include <stddef.h>
#include <stdint.h>

// Memory Controller registers we need/care about

struct tegra_mc_regs {
	uint32_t rsvd_0x0[4];			/* 0x00 */
	uint32_t smmu_config;			/* 0x10 */
	uint32_t smmu_tlb_config;		/* 0x14 */
	uint32_t smmu_ptc_config;		/* 0x18 */
	uint32_t smmu_ptb_asid;			/* 0x1c */
	uint32_t smmu_ptb_data;			/* 0x20 */
	uint32_t rsvd_0x24[3];			/* 0x24 */
	uint32_t smmu_tlb_flush;		/* 0x30 */
	uint32_t smmu_ptc_flush;		/* 0x34 */
	uint32_t rsvd_0x38[6];			/* 0x38 */
	uint32_t emem_cfg;			/* 0x50 */
	uint32_t emem_adr_cfg;			/* 0x54 */
	uint32_t emem_adr_cfg_dev0;		/* 0x58 */
	uint32_t emem_adr_cfg_dev1;		/* 0x5c */
	uint32_t rsvd_0x60[1];			/* 0x60 */
	uint32_t emem_adr_cfg_bank_mask_0;	/* 0x64 */
	uint32_t emem_adr_cfg_bank_mask_1;	/* 0x68 */
	uint32_t emem_adr_cfg_bank_mask_2;	/* 0x6c */
	uint32_t rsvd_0x70[8];			/* 0x70 */
	uint32_t emem_arb_cfg;			/* 0x90 */
	uint32_t emem_arb_outstanding_req;	/* 0x94 */
	uint32_t emem_arb_timing_rcd;		/* 0x98 */
	uint32_t emem_arb_timing_rp;		/* 0x9c */
	uint32_t emem_arb_timing_rc;		/* 0xa0 */
	uint32_t emem_arb_timing_ras;		/* 0xa4 */
	uint32_t emem_arb_timing_faw;		/* 0xa8 */
	uint32_t emem_arb_timing_rrd;		/* 0xac */
	uint32_t emem_arb_timing_rap2pre;	/* 0xb0 */
	uint32_t emem_arb_timing_wap2pre;	/* 0xb4 */
	uint32_t emem_arb_timing_r2r;		/* 0xb8 */
	uint32_t emem_arb_timing_w2w;		/* 0xbc */
	uint32_t emem_arb_timing_r2w;		/* 0xc0 */
	uint32_t emem_arb_timing_w2r;		/* 0xc4 */
	uint32_t rsvd_0xc8[2];			/* 0xc8 */
	uint32_t emem_arb_da_turns;		/* 0xd0 */
	uint32_t emem_arb_da_covers;		/* 0xd4 */
	uint32_t emem_arb_misc0;		/* 0xd8 */
	uint32_t emem_arb_misc1;		/* 0xdc */
	uint32_t emem_arb_ring1_throttle;	/* 0xe0 */
	uint32_t emem_arb_ring3_throttle;	/* 0xe4 */
	uint32_t emem_arb_override;		/* 0xe8 */
	uint32_t emem_arb_rsv;			/* 0xec */
	uint32_t rsvd_0xf0[1];			/* 0xf0 */
	uint32_t clken_override;		/* 0xf4 */
	uint32_t timing_control_dbg;		/* 0xf8 */
	uint32_t timing_control;		/* 0xfc */
	uint32_t stat_control;			/* 0x100 */
	uint32_t rsvd_0x104[65];		/* 0x104 */
	uint32_t emem_arb_isochronous_0;	/* 0x208 */
	uint32_t emem_arb_isochronous_1;	/* 0x20c */
	uint32_t emem_arb_isochronous_2;	/* 0x210 */
	uint32_t rsvd_0x214[38];		/* 0x214 */
	uint32_t dis_extra_snap_levels;		/* 0x2ac */
	uint32_t rsvd_0x2b0[90];		/* 0x2b0 */
	uint32_t video_protect_vpr_override;	/* 0x418 */
	uint32_t rsvd_0x41c[93];		/* 0x41c */
	uint32_t video_protect_vpr_override1;	/* 0x590 */
	uint32_t rsvd_0x594[29];		/* 0x594 */
	uint32_t display_snap_ring;		/* 0x608 */
	uint32_t rsvd_0x60c[15];		/* 0x60c */
	uint32_t video_protect_bom;		/* 0x648 */
	uint32_t video_protect_size_mb;		/* 0x64c */
	uint32_t video_protect_reg_ctrl;	/* 0x650 */
	uint32_t rsvd_0x654[4];			/* 0x654 */
	uint32_t emem_cfg_access_ctrl;		/* 0x664 */
	uint32_t rsvd_0x668[2];			/* 0x668 */
	uint32_t sec_carveout_bom;		/* 0x670 */
	uint32_t sec_carveout_size_mb;		/* 0x674 */
	uint32_t sec_carveout_reg_ctrl;		/* 0x678 */
	uint32_t rsvd_0x67c[187];		/* 0x67c */
	uint32_t emem_arb_override_1;		/* 0x968 */
	uint32_t rsvd_0x96c[3];			/* 0x96c */
	uint32_t video_protect_bom_adr_hi;	/* 0x978 */
	uint32_t rsvd_0x97c[2];			/* 0x97c */
	uint32_t video_protect_gpu_override_0;	/* 0x984 */
	uint32_t video_protect_gpu_override_1;	/* 0x988 */
	uint32_t rsvd_0x98c[5];			/* 0x98c */
	uint32_t mts_carveout_bom;		/* 0x9a0 */
	uint32_t mts_carveout_size_mb;		/* 0x9a4 */
	uint32_t mts_carveout_adr_hi;		/* 0x9a8 */
	uint32_t mts_carveout_reg_ctrl;		/* 0x9ac */
	uint32_t rsvd_0x9b0[4];			/* 0x9b0 */
	uint32_t emem_bank_swizzle_cfg0;	/* 0x9c0 */
	uint32_t emem_bank_swizzle_cfg1;	/* 0x9c4 */
	uint32_t emem_bank_swizzle_cfg2;	/* 0x9c8 */
	uint32_t emem_bank_swizzle_cfg3;	/* 0x9cc */
	uint32_t rsvd_0x9d0[1];			/* 0x9d0 */
	uint32_t sec_carveout_adr_hi;		/* 0x9d4 */
};

enum {
	MC_EMEM_CFG_SIZE_MB_SHIFT = 0,
	MC_EMEM_CFG_SIZE_MB_MASK = 0x3fff,

	MC_EMEM_ARB_MISC0_MC_EMC_SAME_FREQ_SHIFT = 27,
	MC_EMEM_ARB_MISC0_MC_EMC_SAME_FREQ_MASK = 1 << 27,

	MC_EMEM_CFG_ACCESS_CTRL_WRITE_ACCESS_DISABLED = 1,

	MC_TIMING_CONTROL_TIMING_UPDATE = 1,
};

check_member(tegra_mc_regs, sec_carveout_adr_hi, 0x9d4);

#endif	/* __SOC_NVIDIA_TEGRA124_MC_H__ */
