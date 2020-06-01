/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRAMC_COMMON_MT8183_H_
#define _DRAMC_COMMON_MT8183_H_

enum {
	DRAM_DFS_SHUFFLE_1 = 0,
	DRAM_DFS_SHUFFLE_2,
	DRAM_DFS_SHUFFLE_3,
	DRAM_DFS_SHUFFLE_MAX
};

enum {
	CHANNEL_A = 0,
	CHANNEL_B,
	CHANNEL_MAX
};

enum {
	RANK_0 = 0,
	RANK_1,
	RANK_MAX
};

enum dram_odt_type {
	ODT_OFF = 0,
	ODT_ON,
	ODT_MAX
};

enum {
	CA_NUM_LP4 = 6,
	DQ_DATA_WIDTH = 16,
	DQS_BIT_NUMBER = 8,
	DQS_NUMBER = (DQ_DATA_WIDTH / DQS_BIT_NUMBER)
};

/*
 * Internal CBT mode enum
 * 1. Calibration flow uses vGet_Dram_CBT_Mode to
 *    differentiate between mixed vs non-mixed LP4
 * 2. Declared as dram_cbt_mode[RANK_MAX] internally to
 *    store each rank's CBT mode type
 */
enum {
	CBT_NORMAL_MODE = 0,
	CBT_BYTE_MODE1
};

enum {
	FSP_0 = 0,
	FSP_1,
	FSP_MAX
};

#endif   /* _DRAMC_COMMON_MT8183_H_ */
