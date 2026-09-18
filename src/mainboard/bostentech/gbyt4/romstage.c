/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>

void mainboard_memory_init_params(struct memory_init_params *memory_init_params)
{
	memory_init_params->dram_type = DRAM_TYPE_DDR3L;
	memory_init_params->dram_is_slotted = true;

	memory_init_params->spd_cfgs[0].src = SPD_SRC_SMBUS;
	memory_init_params->spd_cfgs[0].addr = 0x50;
}
