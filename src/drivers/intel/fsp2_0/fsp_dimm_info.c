/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbmem.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memory_info.h>
#include <soc/intel/common/smbios.h>
#include <string.h>

/*
 * Processes a single DIMM slot, checks for presence, fills in the
 * dimm_info structure, and prepares for SMBIOS table creation.
 *
 * @param src_dimm: Source DIMM info from FSP HOB.
 * @param dest_dimm: Destination DIMM info in CBMEM.
 * @param channel_info: Channel info from FSP HOB.
 * @param meminfo_hob: Overall memory info HOB.
 * @param node: Memory controller node index.
 * @param dram_part_num_ptr: Pointer to the current DRAM part number string.
 * @param dram_part_num_len_ptr: Pointer to the current DRAM part number length.
 * @return 1 if a DIMM was processed (index incremented), 0 otherwise.
 */
static int fsp_process_dimm_slot(const DIMM_INFO *src_dimm,
				 struct dimm_info *dest_dimm,
				 const CHANNEL_INFO *channel_info,
				 const MEMORY_INFO_DATA_HOB *meminfo_hob,
				 int node,
				 const char **dram_part_num_ptr,
				 size_t *dram_part_num_len_ptr)
{
	const char *dram_part_num = *dram_part_num_ptr;
	size_t dram_part_num_len = *dram_part_num_len_ptr;
	const uint8_t *serial_num;
	uint8_t mem_profile_number;
	struct dimm_fill_args fill_args;

	if (src_dimm->Status != DIMM_PRESENT)
		return 0;

	/* If there is no DRAM part number overridden by
	 * mainboard then use original one. */
	if (!dram_part_num) {
		dram_part_num_len = sizeof(src_dimm->ModulePartNum);
		dram_part_num = (const char *)&src_dimm->ModulePartNum[0];
	}

	mem_profile_number = meminfo_hob->MemoryProfile;
	serial_num = src_dimm->SpdSave + SPD_SAVE_OFFSET_SERIAL;

	platform_fill_dimm_info_args(src_dimm, meminfo_hob, &fill_args);

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
		fill_args.data_width,
		meminfo_hob->VddVoltage[mem_profile_number],
		meminfo_hob->EccSupport,
		fill_args.mfg_id_arg,
		src_dimm->SpdModuleType,
		node,
		meminfo_hob->MaximumMemoryClockSpeed);

	/* Update the pointers in case the DRAM part number was set here */
	*dram_part_num_ptr = dram_part_num;
	*dram_part_num_len_ptr = dram_part_num_len;

	return 1;
}

void fsp_save_dimm_info(void)
{
	int node, channel, dimm_max;
	size_t hob_size;
	const CONTROLLER_INFO *ctrlr_info;
	const CHANNEL_INFO *channel_info;
	struct memory_info *mem_info;
	const MEMORY_INFO_DATA_HOB *meminfo_hob;
	const uint8_t smbios_memory_info_guid[] = {
		0xd4, 0x71, 0x20, 0x9b, 0x54, 0xb0, 0x0c, 0x4e,
		0x8d, 0x09, 0x11, 0xcf, 0x8b, 0x9f, 0x03, 0x23
	};
	const char *dram_part_num = NULL;
	size_t dram_part_num_len = 0;

	/* Locate the memory info HOB, presence validated by raminit */
	meminfo_hob = fsp_find_extension_hob_by_guid(smbios_memory_info_guid, &hob_size);

	if (!meminfo_hob || hob_size == 0) {
		printk(BIOS_ERR, "SMBIOS MEMORY_INFO_DATA_HOB not found\n");
		return;
	}

	/* Allocate CBMEM area for DIMM information... */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (!mem_info) {
		printk(BIOS_ERR, "CBMEM entry for DIMM info missing\n");
		return;
	}

	memset(mem_info, 0, sizeof(*mem_info));

	/* Allow mainboard to override DRAM part number. */
	dram_part_num = mainboard_get_dram_part_num();
	if (dram_part_num)
		dram_part_num_len = strlen(dram_part_num);

	/* Save available DIMM information */
	size_t dimm = 0;
	dimm_max = ARRAY_SIZE(mem_info->dimm);
	for (node = 0; node < MAX_NODE; node++) {
		ctrlr_info = &meminfo_hob->Controller[node];
		for (channel = 0; channel < MAX_CH && dimm < dimm_max; channel++) {
			channel_info = &ctrlr_info->ChannelInfo[channel];
			if (channel_info->Status != CHANNEL_PRESENT)
				continue;
			for (size_t index = 0; index < MAX_DIMM && dimm < dimm_max; index++)
				dimm += fsp_process_dimm_slot(&channel_info->DimmInfo[index],
							&mem_info->dimm[dimm],
							channel_info, meminfo_hob,
							node, &dram_part_num,
							&dram_part_num_len);
		}
	}

	mem_info->dimm_cnt = dimm;
	if (mem_info->dimm_cnt == 0)
		printk(BIOS_ERR, "No DIMMs found\n");
	else
		printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}
