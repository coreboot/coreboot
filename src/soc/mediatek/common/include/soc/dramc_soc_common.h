/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_DRAMC_SOC_COMMON_H__
#define __SOC_MEDIATEK_DRAMC_SOC_COMMON_H__

/*
 * Internal CBT mode enum
 * 1. Calibration flow uses vGet_Dram_CBT_Mode to
 *    differentiate between mixed vs non-mixed LP4
 * 2. Declared as dram_cbt_mode[RANK_MAX] internally to
 *    store each rank's CBT mode type
 */
typedef enum {
	CBT_NORMAL_MODE = 0,
	CBT_BYTE_MODE1,
} DRAM_CBT_MODE_T;

#define DQS_NUMBER_LP4		2
#define DQS_BIT_NUMBER		8
#define DQ_DATA_WIDTH_LP4	16

#endif	/* __SOC_MEDIATEK_DRAMC_SOC_COMMON_H__ */
