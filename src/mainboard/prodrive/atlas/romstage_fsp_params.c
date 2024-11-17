/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

#include "vpd.h"

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
	FSP_M_CONFIG *mcfg = &memupd->FspmConfig;
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

	/* Apply profile-specific settings */
	switch (get_emi_eeprom_vpd()->profile) {
	case ATLAS_PROF_REALTIME_PERFORMANCE:
		mcfg->HyperThreading = 0;
		mcfg->DisPgCloseIdleTimeout = 1;
		mcfg->PowerDownMode = 0;
		mcfg->DisableStarv2medPrioOnNewReq = 1;
		break;
	}

	/* Enable Audio */
	mcfg->PchHdaAudioLinkHdaEnable = 1;
	mcfg->PchHdaSdiEnable[0] = 1;
	mcfg->PchHdaSdiEnable[1] = 1;

	// CPU rootports do not have a ClockReq connected on Atlas. If this is not done,
	// the following will happens:
	//    - FSP will enable power management for cpu rootport.
	//    - coreboot enables ASPM on CPU root port on pci enemuration
	//    - machine exception is thrown, when trying to access pci configuration space after
	//      enabling ASPM src/device/pciexp_device.c:pciexp_tune_dev().
	mcfg->CpuPcieRpClockReqMsgEnable[0] = 0;
	mcfg->CpuPcieRpClockReqMsgEnable[1] = 0;
	mcfg->CpuPcieRpClockReqMsgEnable[2] = 0;

	// shared clock
	mcfg->PcieClkSrcUsage[0] = 0x80;
	mcfg->PcieClkSrcClkReq[0] = 0xFF;
	// i225
	mcfg->PcieClkSrcUsage[1] = 9; // RP 10
	mcfg->PcieClkSrcClkReq[1] = 1;

	// FIX Apparently Rootports don't like the idea of not having a clksrc and clkreq
	// attached to it. For example if we set PcieClkSrcClkReq[1] above to 0xFF (unused)
	// it will not come back out of L1. You can easily test this on windows by trying to
	// update the i225 driver in device manager or use setpci in Linux to set Device in D3.
	// The same applies to all other rootports no matter which devices are connected to it.
	// Therefore we put each rootport (that does not have a clkreq, clksrc)
	// to a not connected (not routed out) clksrc and clkreq. That seems to be a current FSP Bug.
	// workaround and will be removed as soon as FSP is fixed.
	mcfg->PcieClkSrcUsage[2] = 4; // Rootport 5
	mcfg->PcieClkSrcClkReq[2] = 0;
	mcfg->PcieClkSrcUsage[3] = 5; // Rootport 6
	mcfg->PcieClkSrcClkReq[3] = 0;
	mcfg->PcieClkSrcUsage[4] = 8; // Rootport 9
	mcfg->PcieClkSrcClkReq[4] = 0;
	mcfg->PcieClkSrcUsage[5] = 6; // Rootport 7
	mcfg->PcieClkSrcClkReq[5] = 0;
	mcfg->PcieClkSrcUsage[6] = 7; // Rootport 8
	mcfg->PcieClkSrcClkReq[6] = 0;
}
