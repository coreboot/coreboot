/*
 * Copyright (c) 2010 - 2013, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef _TEGRA124_MC_H_
#define _TEGRA124_MC_H_

#include <stdint.h>

// Memory Controller registers we need/care about

struct tegra_mc_regs {
	u32 reserved0[4];
	u32 mc_smmu_config;
	u32 mc_smmu_tlb_config;
	u32 mc_smmu_ptc_config;
	u32 mc_smmu_ptb_asid;
	u32 mc_smmu_ptb_data;
	u32 reserved1[3];
	u32 mc_smmu_tlb_flush;
	u32 mc_smmu_ptc_flush;
	u32 reserved2[6];
	u32 mc_emem_cfg;
	u32 mc_emem_adr_cfg;
	u32 mc_emem_adr_cfg_dev0;
	u32 mc_emem_adr_cfg_dev1;
	u32 reserved3[12];
	u32 mc_emem_arb_reserved[28];
	u32 reserved4[338];
	u32 mc_vpr_bom;
	u32 mc_vpr_size;
	u32 mc_vpr_ctrl;
};

#endif	/* _TEGRA124_MC_H_ */
