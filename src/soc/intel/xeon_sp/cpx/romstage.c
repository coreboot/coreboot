/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/pci_devs.h>
#include "chip.h"

void __weak mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Default weak implementation */
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSPM_CONFIG *m_cfg = &mupd->FspmConfig;
	const struct device *dev;
	const config_t *config = config_of_soc();

	/* ErrorLevel - 0 (disable) to 8 (verbose) */
	m_cfg->DebugPrintLevel = 8;

	/* BoardId 0x1D is for CooperCity reference platform */
	m_cfg->BoardId = 0x1D;

	/* Bitmask for valid sockets supported by the board */
	m_cfg->BoardTypeBitmask = 0x11111111;

	m_cfg->mmiohBase = 0x2000;

	/* default: 0x1 (enable), set to 0x2 (auto) */
	m_cfg->KtiPrefetchEn = 0x2;
	/* default: all 8 sockets enabled */
	for (int i = 2; i < 8; ++i)
		m_cfg->KtiFpgaEnable[i] = 0;
	/* default: 0x1 (enable), set to 0x0 (disable) */
	m_cfg->IsKtiNvramDataReady = 0x0;

	/*
	 * Sub Numa(Non-Uniform Memory Access) Clustering ID and NUMA memory Assignment
	 *  default: 0x1 (enable), set to 0x0 (disable)
	*/
	m_cfg->SncEn = 0x0;

	/* default: 0x1 (enable), set to 0x2 (auto) */
	m_cfg->DirectoryModeEn = 0x2;

	/* default: 0x1 (enable), set to 0x0 (disable) */
	m_cfg->WaSerializationEn = 0x0;

	/* default: 0x0 (disable), set to 0x2 (auto) */
	m_cfg->XptRemotePrefetchEn = 0x2;

	/* default: 0x0 (disable), set to 0x1 (enable) */
	m_cfg->highGap = 0x1;

	/* the wait time in units of 1000us for PBSP to check in */
	m_cfg->WaitTimeForPSBP = 0x7530;

	/* Needed to avoid FSP-M reset. The default value of 0x01 is for MinPlatform */
	m_cfg->PchAdrEn = 0x02;

	/* Make all IIO PCIe ports and port menus visible */
	m_cfg->PEXPHIDE = 0x0;
	m_cfg->HidePEXPMenu = 0x0;

	/* Enable PCH thermal device in FSP, the definition of ThermalDeviceEnable is
	   0: Disable, 1: Enabled in PCI mode, 2: Enabled in ACPI mode */
	dev = pcidev_path_on_root(PCH_DEVFN_THERMAL);
	m_cfg->ThermalDeviceEnable = dev && dev->enabled;

	/* Enable VT-d according to DTB */
	m_cfg->VtdSupport = config->vtd_support;
	m_cfg->X2apic = config->x2apic;

	mainboard_memory_init_params(mupd);
}
