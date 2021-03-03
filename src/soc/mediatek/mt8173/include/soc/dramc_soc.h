/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRAMC_SOC_H_
#define _DRAMC_SOC_H_

enum {
	CHANNEL_A = 0,
	CHANNEL_B,
	CHANNEL_NUM
};

enum {
	GW_PARAM_COARSE = 0,
	GW_PARAM_FINE,
	GW_PARAM_NUM
};

enum {
	DUAL_RANKS = 2,
	CATRAINING_NUM = 10
};

enum {
	DQ_DATA_WIDTH = 32,
	DQS_BIT_NUMBER = 8,
	DQS_NUMBER = (DQ_DATA_WIDTH / DQS_BIT_NUMBER)
};

#endif   /* _DRAMC_COMMON_H_ */
