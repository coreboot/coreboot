/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg ddr5_mem_config = {
	.type = MEM_TYPE_DDR5,
	/* According to DOC #573387 rcomp values no longer have to be provided */
	/* DDR DIMM configuration does not need to set DQ/DQS maps */
	.ect = true, /* Early Command Training */
	.UserBd = BOARD_TYPE_MOBILE,
	.LpDdrDqDqsReTraining = 1
};

static const struct mem_spd dimm_module_spd_info = {
	.topo = MEM_TOPO_DIMM_MODULE,
	.smbus[0] = { .addr_dimm[0] = 0x52, },
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const bool half_populated = true;
	memcfg_init(memupd, &ddr5_mem_config, &dimm_module_spd_info, half_populated);
}
