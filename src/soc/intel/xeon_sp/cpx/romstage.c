/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <arch/romstage.h>
#include <cbmem.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <soc/ddr.h>
#include <soc/romstage.h>
#include <soc/pci_devs.h>
#include <soc/intel/common/smbios.h>
#include <soc/soc_util.h>
#include <static.h>
#include <string.h>

#include "chip.h"

void __weak mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Default weak implementation */
}

uint8_t get_error_correction_type(const uint8_t RasModesEnabled)
{
	switch (RasModesEnabled) {
	case CH_INDEPENDENT:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	case FULL_MIRROR_1LM:
	case PARTIAL_MIRROR_1LM:
	case FULL_MIRROR_2LM:
	case PARTIAL_MIRROR_2LM:
		return MEMORY_ARRAY_ECC_MULTI_BIT;
	case RK_SPARE:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	case CH_LOCKSTEP:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	default:
		return MEMORY_ARRAY_ECC_MULTI_BIT;
	}
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
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
	m_cfg->ThermalDeviceEnable = is_devfn_enabled(PCH_DEVFN_THERMAL);

	/* Enable VT-d according to DTB */
	m_cfg->VtdSupport = config->vtd_support;
	m_cfg->X2apic = config->x2apic;

	/* Disable ISOC */
	m_cfg->isocEn = 0;

	mainboard_memory_init_params(mupd);

	/* Adjust the "cold boot required" flag in CMOS. */
	soc_set_mrc_cold_boot_flag(!mupd->FspmArchUpd.NvsBufferPtr);
}

uint32_t get_max_capacity_mib(void)
{
	/* According to Dear Customer Letter it's 1.12 TB per processor. */
	return 1.12 * MiB * CONFIG_MAX_SOCKET;
}

uint8_t get_max_dimm_count(void)
{
	return MAX_IMC;
}

uint8_t get_dram_type(const struct SystemMemoryMapHob *hob)
{
	return MEMORY_TYPE_DDR4;
}
