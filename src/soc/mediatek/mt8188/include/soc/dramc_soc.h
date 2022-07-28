/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 3.7
 */

#ifndef __SOC_MEDIATEK_MT8188_DRAMC_SOC_H__
#define __SOC_MEDIATEK_MT8188_DRAMC_SOC_H__

#include <soc/dramc_soc_common.h>
#include <stdint.h>

typedef enum {
	CHANNEL_A = 0,
	CHANNEL_B,
	CHANNEL_C,
	CHANNEL_D,
	CHANNEL_MAX,
} DRAM_CHANNEL_T;

typedef enum {
	RANK_0 = 0,
	RANK_1,
	RANK_MAX,
} DRAM_RANK_T;

typedef enum {
	SRAM_SHU0 = 0,
	SRAM_SHU1,
	SRAM_SHU2,
	SRAM_SHU3,
	SRAM_SHU4,
	SRAM_SHU5,
	SRAM_SHU6,
	SRAM_SHU7,
	DRAM_DFS_SRAM_MAX,
} DRAM_DFS_SRAM_SHU_T;

typedef enum {
	DRVP = 0,
	DRVN,
	ODTP,
	ODTN,
	NTODT,
	IMP_DRV_MAX,
} DRAM_IMP_DRV_T;

#define DRAM_DFS_SHU_MAX	DRAM_DFS_SRAM_MAX

#endif	/* __SOC_MEDIATEK_MT8188_DRAMC_SOC_H__ */
