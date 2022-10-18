/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cfg.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/smbus.h>
#include <memory_info.h>
#include <soc/intel/common/smbios.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>

#define FSP_SMBIOS_MEMORY_INFO_GUID	\
{	\
	0xd4, 0x71, 0x20, 0x9b, 0x54, 0xb0, 0x0c, 0x4e,	\
	0x8d, 0x09, 0x11, 0xcf, 0x8b, 0x9f, 0x03, 0x23	\
}

/* Save the DIMM information for SMBIOS table 17 */
static void save_dimm_info(void)
{
	int node, channel, dimm, dimm_max, index;
	size_t hob_size;
	const CONTROLLER_INFO *ctrlr_info;
	const CHANNEL_INFO *channel_info;
	const DIMM_INFO *src_dimm;
	struct dimm_info *dest_dimm;
	struct memory_info *mem_info;
	const MEMORY_INFO_DATA_HOB *meminfo_hob;
	const uint8_t smbios_memory_info_guid[16] =
			FSP_SMBIOS_MEMORY_INFO_GUID;
	const uint8_t *serial_num;
	const char *dram_part_num = NULL;
	size_t dram_part_num_len = 0;
	bool is_dram_part_overridden = false;

	/* Locate the memory info HOB, presence validated by raminit */
	meminfo_hob = fsp_find_extension_hob_by_guid(
						smbios_memory_info_guid,
						&hob_size);
	if (meminfo_hob == NULL || hob_size == 0) {
		printk(BIOS_ERR, "SMBIOS MEMORY_INFO_DATA_HOB not found\n");
		return;
	}

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

	/* Allow mainboard to override DRAM part number. */
	dram_part_num = mainboard_get_dram_part_num();
	if (dram_part_num) {
		dram_part_num_len = strlen(dram_part_num);
		is_dram_part_overridden = true;
	}

	/* Save available DIMM information */
	index = 0;
	dimm_max = ARRAY_SIZE(mem_info->dimm);
	for (node = 0; node < MAX_NODE; node++) {
		ctrlr_info = &meminfo_hob->Controller[node];
		for (channel = 0; channel < MAX_CH && index < dimm_max;
			channel++) {
			channel_info = &ctrlr_info->ChannelInfo[channel];
			if (channel_info->Status != CHANNEL_PRESENT)
				continue;

			for (dimm = 0; dimm < MAX_DIMM && index < dimm_max;
				dimm++) {
				src_dimm = &channel_info->DimmInfo[dimm];
				dest_dimm = &mem_info->dimm[index];
				if (src_dimm->Status != DIMM_PRESENT)
					continue;

				/* If there is no DRAM part number overridden by
				 * mainboard then use original one. */
				if (!is_dram_part_overridden) {
					dram_part_num_len = sizeof(src_dimm->ModulePartNum);
					dram_part_num = (const char *)
								&src_dimm->ModulePartNum[0];
				}

				u8 memProfNum = meminfo_hob->MemoryProfile;
				serial_num = src_dimm->SpdSave +
						SPD_SAVE_OFFSET_SERIAL;

				/* Populate the DIMM information */
				dimm_info_fill(dest_dimm,
					src_dimm->DimmCapacity,
					meminfo_hob->MemoryType,
					meminfo_hob->ConfiguredMemoryClockSpeed,
					src_dimm->RankInDimm,
					channel_info->ChannelId,
					src_dimm->DimmId,
					dram_part_num,
					dram_part_num_len,
					serial_num,
					meminfo_hob->DataWidth,
					meminfo_hob->VddVoltage[memProfNum],
					meminfo_hob->EccSupport,
					src_dimm->MfgId,
					src_dimm->SpdModuleType,
					node);
				index++;
			}
		}
	}
	mem_info->dimm_cnt = index;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}

void mainboard_romstage_entry(void)
{
	bool s3wake;
	struct chipset_power_state *ps = pmc_get_power_state();

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();
	/* Program SMBus base address and enable it */
	smbus_common_init();
	/* initialize Heci interface */
	cse_init(HECI1_BASE_ADDRESS);

	s3wake = pmc_fill_power_state(ps) == ACPI_S3;
	fsp_memory_init(s3wake);
	pmc_set_disb();
	if (!s3wake) {
		/*
		 * cse_fw_sync() must be called after DRAM initialization as
		 * HMRFPO_ENABLE HECI command (which is used by cse_fw_sync())
		 * is expected to be executed after DRAM initialization.
		 */

		if (CONFIG(SOC_INTEL_CSE_LITE_SKU))
			cse_fw_sync();

		save_dimm_info();
	}

}
