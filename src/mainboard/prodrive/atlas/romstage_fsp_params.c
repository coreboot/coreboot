/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg ddr5_mem_config = {
	.type = MEM_TYPE_DDR5,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistor */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = { 50, 30, 30, 30, 27 },
	},

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,

	.LpDdrDqDqsReTraining = 1,

	.ddr_config = {
		.dq_pins_interleaved = false,
	}
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *mem_config = &ddr5_mem_config;
	const bool half_populated = false;

	const struct mem_spd dimm_module_spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = {
				.addr_dimm[0] = 0x50,
			},
			[1] = {
				.addr_dimm[0] = 0x52,
			},
		},
	};

	memcfg_init(memupd, mem_config, &dimm_module_spd_info, half_populated);

	/* Enable Audio */
	memupd->FspmConfig.PchHdaAudioLinkHdaEnable = 1;
	memupd->FspmConfig.PchHdaSdiEnable[0] = 1;
	memupd->FspmConfig.PchHdaSdiEnable[1] = 1;

	// CPU rootports do not have a ClockReq connected on Atlas. If this is not done,
	// the following will happens:
	//    - FSP will enable power management for cpu rootport.
	//    - coreboot enables ASPM on CPU root port on pci enemuration
	//    - machine exception is thrown, when trying to access pci configuration space after
	//      enabling ASPM src/device/pciexp_device.c:pciexp_tune_dev().
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[0] = 0;
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[1] = 0;
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[2] = 0;
}
