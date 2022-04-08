/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ROMSTAGE_H_
#define _SOC_ROMSTAGE_H_

#include <fsp/api.h>
#include <stddef.h>

void mainboard_memory_init_params(FSPM_UPD *memupd);
void systemagent_early_init(void);

/* Board type */
enum board_type {
	BOARD_TYPE_MOBILE  = 0,
	BOARD_TYPE_DESKTOP_1DPC = 1,
	BOARD_TYPE_DESKTOP_2DPC = 2,
	BOARD_TYPE_DESKTOP_2DPC_T_TOP_ASYM = 3,
	BOARD_TYPE_DESKTOP_2DPC_T_TOP = 4,
	BOARD_TYPE_ULT_ULX = 5,
	BOARD_TYPE_ULT_ULX_T4 = 6,
	BOARD_TYPE_SERVER  = 8
};

#endif /* _SOC_ROMSTAGE_H_ */
