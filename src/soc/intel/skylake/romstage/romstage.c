/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/smbus.h>
#include <memory_info.h>
#include <smbios.h>
#include <soc/intel/common/smbios.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>

#define FSP_SMBIOS_MEMORY_INFO_GUID	\
{	\
	0xd4, 0x71, 0x20, 0x9b, 0x54, 0xb0, 0x0c, 0x4e,	\
	0x8d, 0x09, 0x11, 0xcf, 0x8b, 0x9f, 0x03, 0x23	\
}

/* Memory Channel Present Status */
enum {
	CHANNEL_NOT_PRESENT,
	CHANNEL_DISABLED,
	CHANNEL_PRESENT
};

/* Save the DIMM information for SMBIOS table 17 */
static void save_dimm_info(void)
{
	int channel, dimm, dimm_max, index;
	size_t hob_size;
	uint8_t ddr_type;
	const CONTROLLER_INFO *ctrlr_info;
	const CHANNEL_INFO *channel_info;
	const DIMM_INFO *src_dimm;
	struct dimm_info *dest_dimm;
	struct memory_info *mem_info;
	const MEMORY_INFO_DATA_HOB *memory_info_hob;
	const uint8_t smbios_memory_info_guid[16] =
			FSP_SMBIOS_MEMORY_INFO_GUID;

	/* Locate the memory info HOB, presence validated by raminit */
	memory_info_hob =
		fsp_find_extension_hob_by_guid(smbios_memory_info_guid,
						&hob_size);
	if (memory_info_hob == NULL || hob_size == 0) {
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

	/* Describe the first N DIMMs in the system */
	index = 0;
	dimm_max = ARRAY_SIZE(mem_info->dimm);
	ctrlr_info = &memory_info_hob->Controller[0];
	for (channel = 0; channel < MAX_CH && index < dimm_max; channel++) {
		channel_info = &ctrlr_info->ChannelInfo[channel];
		if (channel_info->Status != CHANNEL_PRESENT)
			continue;
		for (dimm = 0; dimm < MAX_DIMM && index < dimm_max; dimm++) {
			src_dimm = &channel_info->DimmInfo[dimm];
			dest_dimm = &mem_info->dimm[index];

			if (src_dimm->Status != DIMM_PRESENT)
				continue;

			switch (memory_info_hob->MemoryType) {
			case MRC_DDR_TYPE_DDR4:
				ddr_type = MEMORY_TYPE_DDR4;
				break;
			case MRC_DDR_TYPE_DDR3:
				ddr_type = MEMORY_TYPE_DDR3;
				break;
			case MRC_DDR_TYPE_LPDDR3:
				ddr_type = MEMORY_TYPE_LPDDR3;
				break;
			default:
				ddr_type = MEMORY_TYPE_UNKNOWN;
				break;
			}
			u8 memProfNum = memory_info_hob->MemoryProfile;

			/* Populate the DIMM information */
			dimm_info_fill(dest_dimm,
				src_dimm->DimmCapacity,
				ddr_type,
				memory_info_hob->ConfiguredMemoryClockSpeed,
				src_dimm->RankInDimm,
				channel_info->ChannelId,
				src_dimm->DimmId,
				(const char *)src_dimm->ModulePartNum,
				sizeof(src_dimm->ModulePartNum),
				src_dimm->SpdSave + SPD_SAVE_OFFSET_SERIAL,
				memory_info_hob->DataWidth,
				memory_info_hob->VddVoltage[memProfNum],
				memory_info_hob->EccSupport,
				src_dimm->MfgId,
				src_dimm->SpdModuleType);
			index++;
		}
	}
	mem_info->dimm_cnt = index;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}

void mainboard_romstage_entry(void)
{
	bool s3wake;
	struct chipset_power_state *ps;

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();
	/* Program SMBus base address and enable it */
	smbus_common_init();
	/* initialize Heci interface */
	cse_init(HECI1_BASE_ADDRESS);
	ps = pmc_get_power_state();
	s3wake = pmc_fill_power_state(ps) == ACPI_S3;
	fsp_memory_init(s3wake);
	pmc_set_disb();
	if (!s3wake)
		save_dimm_info();
}
