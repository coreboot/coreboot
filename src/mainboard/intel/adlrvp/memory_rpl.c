/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/romstage.h>

#include "board_id.h"

void rpl_memory_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *mem_cfg = &memupd->FspmConfig;

	int board_id = get_board_id();

	switch (board_id) {
	case ADL_P_LP4_1:
	case ADL_P_LP4_2:
	case ADL_P_DDR4_1:
	case ADL_P_DDR4_2:
	case ADL_P_DDR5_1:
	case ADL_P_DDR5_2:
	default:
		return;
	case ADL_P_LP5_1:
	case ADL_P_LP5_2:
		mem_cfg->Lp5BankMode = 1;
		return;
	}
}
