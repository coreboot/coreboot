/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stddef.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/early_variables.h>
#include <assert.h>
#include <compiler.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/mtrr.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <elog.h>
#include <fsp/romstage.h>
#include <mrc_cache.h>
#include <reset.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <smbios.h>
#include <stage_cache.h>
#include <string.h>
#include <timestamp.h>
#include <vendorcode/google/chromeos/chromeos.h>

asmlinkage void *romstage_main(FSP_INFO_HEADER *fih)
{
	void *top_of_stack;
	struct pei_data pei_data;
	struct romstage_params params = {
		.pei_data = &pei_data,
		.chipset_context = fih,
	};

	post_code(0x30);

	timestamp_add_now(TS_START_ROMSTAGE);

	/* Load microcode before RAM init */
	if (IS_ENABLED(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS))
		intel_update_microcode_from_cbfs();

	memset(&pei_data, 0, sizeof(pei_data));

	/* Display parameters */
	if (!IS_ENABLED(CONFIG_NO_MMCONF_SUPPORT))
		printk(BIOS_SPEW, "CONFIG_MMCONF_BASE_ADDRESS: 0x%08x\n",
			CONFIG_MMCONF_BASE_ADDRESS);
	printk(BIOS_INFO, "Using FSP 1.1\n");

	/* Display FSP banner */
	print_fsp_info(fih);

	/* Stash FSP version. */
	params.fsp_version = fsp_version(fih);

	/* Get power state */
	params.power_state = fill_power_state();

	/* Call into mainboard. */
	mainboard_romstage_entry(&params);
	soc_after_ram_init(&params);
	post_code(0x38);

	top_of_stack = setup_stack_and_mtrrs();

	printk(BIOS_DEBUG, "Calling FspTempRamExit API\n");
	timestamp_add_now(TS_FSP_TEMP_RAM_EXIT_START);
	return top_of_stack;
}

void *cache_as_ram_stage_main(FSP_INFO_HEADER *fih)
{
	return romstage_main(fih);
}

/* Entry from the mainboard. */
void romstage_common(struct romstage_params *params)
{
	bool s3wake;
	struct region_device rdev;
	struct pei_data *pei_data;

	post_code(0x32);

	timestamp_add_now(TS_BEFORE_INITRAM);

	pei_data = params->pei_data;
	pei_data->boot_mode = params->power_state->prev_sleep_state;
	s3wake = params->power_state->prev_sleep_state == ACPI_S3;

	if (IS_ENABLED(CONFIG_ELOG_BOOT_COUNT) && !s3wake)
		boot_count_increment();

	/* Perform remaining SOC initialization */
	soc_pre_ram_init(params);
	post_code(0x33);

	/* Check recovery and MRC cache */
	params->pei_data->saved_data_size = 0;
	params->pei_data->saved_data = NULL;
	if (!params->pei_data->disable_saved_data) {
		if (vboot_recovery_mode_enabled()) {
			/* Recovery mode does not use MRC cache */
			printk(BIOS_DEBUG,
			       "Recovery mode: not using MRC cache.\n");
		} else if (IS_ENABLED(CONFIG_CACHE_MRC_SETTINGS)
			&& (!mrc_cache_get_current(MRC_TRAINING_DATA,
							params->fsp_version,
							&rdev))) {
			/* MRC cache found */
			params->pei_data->saved_data_size =
				region_device_sz(&rdev);
			params->pei_data->saved_data = rdev_mmap_full(&rdev);
			/* Assum boot device is memory mapped. */
			assert(IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED));
		} else if (params->pei_data->boot_mode == ACPI_S3) {
			/* Waking from S3 and no cache. */
			printk(BIOS_DEBUG,
			       "No MRC cache found in S3 resume path.\n");
			post_code(POST_RESUME_FAILURE);
			hard_reset();
		} else {
			printk(BIOS_DEBUG, "No MRC cache found.\n");
		}
	}

	/* Initialize RAM */
	raminit(params);
	timestamp_add_now(TS_AFTER_INITRAM);

	/* Save MRC output */
	if (IS_ENABLED(CONFIG_CACHE_MRC_SETTINGS)) {
		printk(BIOS_DEBUG, "MRC data at %p %d bytes\n",
			pei_data->data_to_save, pei_data->data_to_save_size);
		if ((params->pei_data->boot_mode != ACPI_S3)
			&& (params->pei_data->data_to_save_size != 0)
			&& (params->pei_data->data_to_save != NULL))
			mrc_cache_stash_data(MRC_TRAINING_DATA,
				params->fsp_version,
				params->pei_data->data_to_save,
				params->pei_data->data_to_save_size);
	}

	/* Save DIMM information */
	if (!s3wake)
		mainboard_save_dimm_info(params);

	/* Create romstage handof information */
	if (romstage_handoff_init(
			params->power_state->prev_sleep_state == ACPI_S3) < 0)
		hard_reset();
}

void after_cache_as_ram_stage(void)
{
	/* Load the ramstage. */
	run_ramstage();
	die("ERROR - Failed to load ramstage!");
}

/* Initialize the power state */
__weak struct chipset_power_state *fill_power_state(void)
{
	return NULL;
}

/* Board initialization before and after RAM is enabled */
__weak void mainboard_romstage_entry(
	struct romstage_params *params)
{
	post_code(0x31);

	/* Initliaze memory */
	romstage_common(params);
}

/* Save the DIMM information for SMBIOS table 17 */
__weak void mainboard_save_dimm_info(
	struct romstage_params *params)
{
	int channel;
	CHANNEL_INFO *channel_info;
	int dimm;
	DIMM_INFO *dimm_info;
	int dimm_max;
	void *hob_list_ptr;
	EFI_HOB_GUID_TYPE *hob_ptr;
	int index;
	struct memory_info *mem_info;
	FSP_SMBIOS_MEMORY_INFO *memory_info_hob;
	const EFI_GUID memory_info_hob_guid = FSP_SMBIOS_MEMORY_INFO_GUID;

	/* Locate the memory info HOB, presence validated by raminit */
	hob_list_ptr = fsp_get_hob_list();
	hob_ptr = get_next_guid_hob(&memory_info_hob_guid, hob_list_ptr);
	memory_info_hob = (FSP_SMBIOS_MEMORY_INFO *)(hob_ptr + 1);

	/* Display the data in the FSP_SMBIOS_MEMORY_INFO HOB */
	if (IS_ENABLED(CONFIG_DISPLAY_HOBS)) {
		printk(BIOS_DEBUG, "FSP_SMBIOS_MEMORY_INFO HOB\n");
		printk(BIOS_DEBUG, "    0x%02x: Revision\n",
			memory_info_hob->Revision);
		printk(BIOS_DEBUG, "    0x%02x: MemoryType\n",
			memory_info_hob->MemoryType);
		printk(BIOS_DEBUG, "    %d: MemoryFrequencyInMHz\n",
			memory_info_hob->MemoryFrequencyInMHz);
		printk(BIOS_DEBUG, "    %d: DataWidth in bits\n",
			memory_info_hob->DataWidth);
		printk(BIOS_DEBUG, "    0x%02x: ErrorCorrectionType\n",
			memory_info_hob->ErrorCorrectionType);
		printk(BIOS_DEBUG, "    0x%02x: ChannelCount\n",
			memory_info_hob->ChannelCount);
		for (channel = 0; channel < memory_info_hob->ChannelCount;
			channel++) {
			channel_info = &memory_info_hob->ChannelInfo[channel];
			printk(BIOS_DEBUG, "  Channel %d\n", channel);
			printk(BIOS_DEBUG, "      0x%02x: ChannelId\n",
				channel_info->ChannelId);
			printk(BIOS_DEBUG, "      0x%02x: DimmCount\n",
				channel_info->DimmCount);
			for (dimm = 0; dimm < channel_info->DimmCount;
				dimm++) {
				dimm_info = &channel_info->DimmInfo[dimm];
				printk(BIOS_DEBUG, "   DIMM %d\n", dimm);
				printk(BIOS_DEBUG, "      0x%02x: DimmId\n",
					dimm_info->DimmId);
				printk(BIOS_DEBUG, "      %d: SizeInMb\n",
					dimm_info->SizeInMb);
			}
		}
	}

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	printk(BIOS_DEBUG, "CBMEM entry for DIMM info: 0x%p\n", mem_info);
	if (mem_info == NULL)
		return;
	memset(mem_info, 0, sizeof(*mem_info));

	/* Describe the first N DIMMs in the system */
	index = 0;
	dimm_max = ARRAY_SIZE(mem_info->dimm);
	for (channel = 0; channel < memory_info_hob->ChannelCount; channel++) {
		if (index >= dimm_max)
			break;
		channel_info = &memory_info_hob->ChannelInfo[channel];
		for (dimm = 0; dimm < channel_info->DimmCount; dimm++) {
			if (index >= dimm_max)
				break;
			dimm_info = &channel_info->DimmInfo[dimm];

			/* Populate the DIMM information */
			if (dimm_info->SizeInMb) {
				mem_info->dimm[index].dimm_size =
					dimm_info->SizeInMb;
				mem_info->dimm[index].ddr_type =
					memory_info_hob->MemoryType;
				mem_info->dimm[index].ddr_frequency =
					memory_info_hob->MemoryFrequencyInMHz;
				mem_info->dimm[index].channel_num =
					channel_info->ChannelId;
				mem_info->dimm[index].dimm_num =
					dimm_info->DimmId;
				switch (memory_info_hob->DataWidth) {
				default:
				case 8:
					mem_info->dimm[index].bus_width =
						MEMORY_BUS_WIDTH_8;
					break;

				case 16:
					mem_info->dimm[index].bus_width =
						MEMORY_BUS_WIDTH_16;
					break;

				case 32:
					mem_info->dimm[index].bus_width =
						MEMORY_BUS_WIDTH_32;
					break;

				case 64:
					mem_info->dimm[index].bus_width =
						MEMORY_BUS_WIDTH_64;
					break;

				case 128:
					mem_info->dimm[index].bus_width =
						MEMORY_BUS_WIDTH_128;
					break;
				}

				/* Add any mainboard specific information */
				mainboard_add_dimm_info(params, mem_info,
							channel, dimm, index);
				index++;
			}
		}
	}
	mem_info->dimm_cnt = index;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}

/* Add any mainboard specific information */
__weak void mainboard_add_dimm_info(
	struct romstage_params *params,
	struct memory_info *mem_info,
	int channel, int dimm, int index)
{
}

/* Get the memory configuration data */
__weak int mrc_cache_get_current(int type, uint32_t version,
				struct region_device *rdev)
{
	return -1;
}

/* Save the memory configuration data */
__weak int mrc_cache_stash_data(int type, uint32_t version,
					const void *data, size_t size)
{
	return -1;
}

/* Transition RAM from off or self-refresh to active */
__weak void raminit(struct romstage_params *params)
{
	post_code(0x34);
	die("ERROR - No RAM initialization specified!\n");
}

/* Display the memory configuration */
__weak void report_memory_config(void)
{
}

/* Choose top of stack and setup MTRRs */
__weak void *setup_stack_and_mtrrs(void)
{
	die("ERROR - Must specify top of stack!\n");
	return NULL;
}

/* SOC initialization after RAM is enabled */
__weak void soc_after_ram_init(struct romstage_params *params)
{
}

/* SOC initialization before RAM is enabled */
__weak void soc_pre_ram_init(struct romstage_params *params)
{
}
