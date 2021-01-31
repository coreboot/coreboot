/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>
#include "onboard.h"

void mainboard_memory_init_params(struct romstage_params *params,
	MEMORY_INIT_UPD *memory_params)
{
	memory_params->PcdMemoryTypeEnable = MEM_LPDDR3;
}
