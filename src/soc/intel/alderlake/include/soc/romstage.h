/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ROMSTAGE_H_
#define _SOC_ROMSTAGE_H_

#include <fsp/api.h>

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

/*
 * Default implementation indicates that the mainboard does not expect another reset.
 * Mainboards can override the default implementation to indicate whether they expect
 * another reset eg. FW Sync for another component on the mainboard. Some silicon init
 * code eg. disabling UFS, can use this hint to suppress any redundant resets that they
 * trigger. If the mainboard does not expect another reset, then the silicon init code
 * can trigger their required reset.
 *
 * Return: true when the mainboard expects another reset, false otherwise.
 */
bool mainboard_expects_another_reset(void);
#endif /* _SOC_ROMSTAGE_H_ */
