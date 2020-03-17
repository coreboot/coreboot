/*
 * This file is part of the coreboot project.
 *
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

#ifndef _DRAMC_COMMON_H_
#define _DRAMC_COMMON_H_

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
