/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg ddr4_mem_config = {
	.type = MEM_TYPE_DDR4,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistor FIXME */
		.resistor = 100,

		/* Baseboard Rcomp target values FIXME */
		.targets = { 50, 20, 25, 25, 25 },
	},

	.UserBd = BOARD_TYPE_DESKTOP_2DPC, /* FIXME */

	.ddr_config = {
		.dq_pins_interleaved = false, /* FIXME */
	},
};

static const struct mem_spd dimm_module_spd_info = {
	.topo = MEM_TOPO_DIMM_MODULE,
	.smbus = {
		[0] = {
			.addr_dimm[0] = 0x50,
			.addr_dimm[1] = 0x51,
		},
		[1] = {
			.addr_dimm[0] = 0x52,
			.addr_dimm[1] = 0x53,
		},
	},
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	memcfg_init(memupd, &ddr4_mem_config, &dimm_module_spd_info, false);
}
