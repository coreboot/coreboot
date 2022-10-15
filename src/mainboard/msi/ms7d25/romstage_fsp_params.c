/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

#include "gpio.h"

static const struct mb_cfg ddr4_mem_config = {
	.type = MEM_TYPE_DDR4,
	/* According to DOC #573387 rcomp values no longer have to be provided */
	/* DDR DIMM configuration does not need to set DQ/DQS maps */
	.UserBd = BOARD_TYPE_DESKTOP_2DPC,

	.ddr_config = {
		.dq_pins_interleaved = true,
	},
};

static const struct mb_cfg ddr5_mem_config = {
	.type = MEM_TYPE_DDR5,

	.ect = true, /* Early Command Training */

	/* According to DOC #573387 rcomp values no longer have to be provided */
	/* DDR DIMM configuration does not need to set DQ/DQS maps */
	.UserBd = BOARD_TYPE_DESKTOP_2DPC,

	.LpDdrDqDqsReTraining = 1,

	.ddr_config = {
		.dq_pins_interleaved = true,
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
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[0] = 1;
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[1] = 1;
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[2] = 0;
	memupd->FspmConfig.DmiMaxLinkSpeed = 4; // Gen4 speed, undocumented
	memupd->FspmConfig.SkipExtGfxScan = 0;

	memupd->FspmConfig.PchHdaAudioLinkHdaEnable = 1;
	memupd->FspmConfig.PchHdaSdiEnable[0] = 1;

	/*
	 * Let FSP configure virtual wires, CLKREQs, etc.
	 * Otherwise undefined behaviour occurs when coreboot enables ASPM on
	 * CPU PCIe root ports. This is caused by FSP reprogramming certain
	 * pads including CLKREQ pins, despite GpioOverride = 1.
	 */
	memupd->FspmConfig.GpioOverride = 0;

	if (CONFIG(BOARD_MSI_Z690_A_PRO_WIFI_DDR4))
		memcfg_init(memupd, &ddr4_mem_config, &dimm_module_spd_info, false);
	if (CONFIG(BOARD_MSI_Z690_A_PRO_WIFI_DDR5))
		memcfg_init(memupd, &ddr5_mem_config, &dimm_module_spd_info, false);

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
