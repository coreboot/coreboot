/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/meminit.h>
#include <string.h>

#include "gpio.h"

#define FSP_CLK_NOTUSED			0xFF
#define FSP_CLK_FREE_RUNNING		0x80

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

static void disable_pcie_clock_requests(FSP_M_CONFIG *m_cfg)
{
	memset(m_cfg->PcieClkSrcUsage, FSP_CLK_NOTUSED, sizeof(m_cfg->PcieClkSrcUsage));
	memset(m_cfg->PcieClkSrcClkReq, FSP_CLK_NOTUSED, sizeof(m_cfg->PcieClkSrcClkReq));

	/* PCIe CLK SRCes as per devicetree.cb */
	m_cfg->PcieClkSrcUsage[0]  = FSP_CLK_FREE_RUNNING;
	m_cfg->PcieClkSrcUsage[8]  = FSP_CLK_FREE_RUNNING;
	m_cfg->PcieClkSrcUsage[9]  = FSP_CLK_FREE_RUNNING;
	m_cfg->PcieClkSrcUsage[10] = FSP_CLK_FREE_RUNNING;
	m_cfg->PcieClkSrcUsage[12] = FSP_CLK_FREE_RUNNING;
	m_cfg->PcieClkSrcUsage[13] = FSP_CLK_FREE_RUNNING;
	m_cfg->PcieClkSrcUsage[14] = FSP_CLK_FREE_RUNNING;
	m_cfg->PcieClkSrcUsage[15] = FSP_CLK_FREE_RUNNING;
	m_cfg->PcieClkSrcUsage[17] = FSP_CLK_FREE_RUNNING;

	gpio_configure_pads(clkreq_disabled_table, ARRAY_SIZE(clkreq_disabled_table));
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[0] = CONFIG(PCIEXP_CLK_PM);
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[1] = CONFIG(PCIEXP_CLK_PM);
	memupd->FspmConfig.CpuPcieRpClockReqMsgEnable[2] = CONFIG(PCIEXP_CLK_PM);
	memupd->FspmConfig.DmiMaxLinkSpeed = 4; // Gen4 speed, undocumented
	memupd->FspmConfig.DmiAspm = 0;
	memupd->FspmConfig.DmiAspmCtrl = 0;
	memupd->FspmConfig.SkipExtGfxScan = 0;

	memupd->FspmConfig.PchHdaSdiEnable[0] = 1;

	if (CONFIG(BOARD_MSI_Z690_A_PRO_WIFI_DDR4))
		memcfg_init(memupd, &ddr4_mem_config, &dimm_module_spd_info, false);
	if (CONFIG(BOARD_MSI_Z690_A_PRO_WIFI_DDR5))
		memcfg_init(memupd, &ddr5_mem_config, &dimm_module_spd_info, false);

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	if (!CONFIG(PCIEXP_CLK_PM))
		disable_pcie_clock_requests(&memupd->FspmConfig);
}
