/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_DRAMC_SOC_H__
#define __SOC_MEDIATEK_DRAMC_SOC_H__

typedef enum {
	CHANNEL_A = 0,
	CHANNEL_B,
	CHANNEL_MAX
} DRAM_CHANNEL_T;

typedef enum {
	RANK_0 = 0,
	RANK_1,
	RANK_MAX
} DRAM_RANK_T;

typedef enum {
	DRAM_DFS_SHUFFLE_1 = 0,
	DRAM_DFS_SHUFFLE_2,
	DRAM_DFS_SHUFFLE_3,
	DRAM_DFS_SHUFFLE_4,
	DRAM_DFS_SHUFFLE_5,
	DRAM_DFS_SHUFFLE_6,
	DRAM_DFS_SHUFFLE_7,
	DRAM_DFS_SHUFFLE_MAX
} DRAM_DFS_SHUFFLE_TYPE_T; // DRAM SHUFFLE RG type

/*
 * Internal CBT mode enum
 * 1. Calibration flow uses vGet_Dram_CBT_Mode to
 *    differentiate between mixed vs non-mixed LP4
 * 2. Declared as dram_cbt_mode[RANK_MAX] internally to
 *    store each rank's CBT mode type
 */
typedef enum {
	CBT_NORMAL_MODE = 0,
	CBT_BYTE_MODE1
} DRAM_CBT_MODE_T, dram_cbt_mode;

#define DRAM_DFS_SHU_MAX	DRAM_DFS_SHUFFLE_MAX

#define DQS_NUMBER_LP4              2
#define DQS_BIT_NUMBER              8
#define DQ_DATA_WIDTH_LP4           16

#endif	/* __SOC_MEDIATEK_DRAMC_SOC_H__ */
