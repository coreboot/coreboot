/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <arch/romstage.h>
#include <console/console.h>
#include <cbmem.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <soc/ddr.h>
#include <soc/romstage.h>
#include <soc/pci_devs.h>
#include <soc/intel/common/smbios.h>
#include <string.h>

#include "chip.h"

void __weak mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Default weak implementation */
}

static const struct SystemMemoryMapHob *get_system_memory_map(void)
{
	size_t hob_size;
	const uint8_t mem_hob_guid[16] = FSP_SYSTEM_MEMORYMAP_HOB_GUID;
	const struct SystemMemoryMapHob **memmap_addr;

	memmap_addr = (const struct SystemMemoryMapHob **)
		fsp_find_extension_hob_by_guid(mem_hob_guid, &hob_size);
	/* hob_size is the size of the 8-byte address not the hob data */
	assert(memmap_addr != NULL && hob_size != 0);
	/* assert the pointer to the hob is not NULL */
	assert(*memmap_addr != NULL);

	return *memmap_addr;
}

/* Save the DIMM information for SMBIOS table 17 */
void save_dimm_info(void)
{
	struct dimm_info *dest_dimm;
	struct memory_info *mem_info;
	const struct SystemMemoryMapHob *hob;
	MEMMAP_DIMM_DEVICE_INFO_STRUCT src_dimm;
	int dimm_max, index = 0;
	uint32_t vdd_voltage;

	hob = get_system_memory_map();
	assert(hob != NULL);

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (mem_info == NULL) {
		printk(BIOS_ERR, "CBMEM entry for DIMM info missing\n");
		return;
	}
	memset(mem_info, 0, sizeof(*mem_info));
	dimm_max = ARRAY_SIZE(mem_info->dimm);
	vdd_voltage = get_ddr_voltage(hob->DdrVoltage);
	/* For now only implement for one socket and hard-coded for DDR4 */
	for (int ch = 0; ch < MAX_CH; ch++) {
		for (int dimm = 0; dimm < MAX_IMC; dimm++) {
			src_dimm = hob->Socket[0].ChannelInfo[ch].DimmInfo[dimm];
			if (src_dimm.Present) {
				if (index >= dimm_max) {
					printk(BIOS_WARNING, "Too many DIMMs info for %s.\n",
						__func__);
					return;
				}
				dest_dimm = &mem_info->dimm[index];
				dest_dimm->max_speed_mts =
					get_max_memory_speed(src_dimm.commonTck);
				dest_dimm->configured_speed_mts = hob->memFreq;
				dimm_info_fill(dest_dimm,
					src_dimm.DimmSize << 6,
					0x1a, /* hard-coded memory device type as DDR4 */
					hob->memFreq, /* replaced by configured_speed_mts */
					src_dimm.NumRanks,
					ch, /* for mainboard locator string override */
					dimm, /* for mainboard locator string override */
					(const char *)&src_dimm.PartNumber[0],
					sizeof(src_dimm.PartNumber),
					(const uint8_t *)&src_dimm.serialNumber[0],
					64, /* hard-coded for DDR4 data width */
					vdd_voltage,
					true, /* hard-coded as ECC supported */
					src_dimm.VendorID,
					src_dimm.actKeyByte2);
				index++;
			}
		}
	}

	/* Save available DIMM information */
	mem_info->dimm_cnt = index;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
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

	/* Disable ISOC */
	m_cfg->isocEn = 0;

	mainboard_memory_init_params(mupd);
}
