/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <stddef.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/stages.h>
#include <arch/early_variables.h>
#include <boardid.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <elog.h>
#include <fsp/util.h>
#include <memory_info.h>
#include <reset.h>
#include <romstage_handoff.h>
#include <smbios.h>
#include <soc/intel/common/mrc_cache.h>
#include <soc/intel/common/util.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/spi.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <tpm.h>
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

	memset(&pei_data, 0, sizeof(pei_data));

	/* Display parameters */
	printk(BIOS_SPEW, "CONFIG_MMCONF_BASE_ADDRESS: 0x%08x\n",
		CONFIG_MMCONF_BASE_ADDRESS);
	printk(BIOS_INFO, "Using FSP 1.1");

	/* Display FSP banner */
	print_fsp_info(fih);

	/* Get power state */
	params.power_state = fill_power_state();

	/*
	 * Read and print board version.  Done after SOC romstage
	 * in case PCH needs to be configured to talk to the EC.
	 */
	if (IS_ENABLED(CONFIG_BOARD_ID_AUTO))
		printk(BIOS_INFO, "MLB: board version %d\n", board_id());

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
	const struct mrc_saved_data *cache;
	struct romstage_handoff *handoff;
	struct pei_data *pei_data;

	post_code(0x32);

	timestamp_add_now(TS_BEFORE_INITRAM);

	pei_data = params->pei_data;
	pei_data->boot_mode = params->power_state->prev_sleep_state;

#if IS_ENABLED(CONFIG_ELOG_BOOT_COUNT)
	if (params->power_state->prev_sleep_state != SLEEP_STATE_S3)
		boot_count_increment();
#endif

	/* Perform remaining SOC initialization */
	soc_pre_ram_init(params);
	post_code(0x33);

	/* Check recovery and MRC cache */
	params->pei_data->saved_data_size = 0;
	params->pei_data->saved_data = NULL;
	if (!params->pei_data->disable_saved_data) {
		if (recovery_mode_enabled()) {
			/* Recovery mode does not use MRC cache */
			printk(BIOS_DEBUG,
			       "Recovery mode: not using MRC cache.\n");
		} else if (!mrc_cache_get_current(&cache)) {
			/* MRC cache found */
			params->pei_data->saved_data_size = cache->size;
			params->pei_data->saved_data = &cache->data[0];
		} else if (params->pei_data->boot_mode == SLEEP_STATE_S3) {
			/* Waking from S3 and no cache. */
			printk(BIOS_DEBUG,
			       "No MRC cache found in S3 resume path.\n");
			post_code(POST_RESUME_FAILURE);
			hard_reset();
		} else {
			printk(BIOS_DEBUG, "No MRC cache found.\n");
			mainboard_check_ec_image(params);
		}
	}

	/* Initialize RAM */
	raminit(params);
	timestamp_add_now(TS_AFTER_INITRAM);

	/* Save MRC output */
	printk(BIOS_DEBUG, "MRC data at %p %d bytes\n", pei_data->data_to_save,
	       pei_data->data_to_save_size);
	if (params->pei_data->boot_mode != SLEEP_STATE_S3) {
		if (params->pei_data->data_to_save_size != 0 &&
		    params->pei_data->data_to_save != NULL) {
			mrc_cache_stash_data(params->pei_data->data_to_save,
				params->pei_data->data_to_save_size);
		}
	}

	/* Save DIMM information */
	mainboard_save_dimm_info(params);

	/* Create romstage handof information */
	handoff = romstage_handoff_find_or_add();
	if (handoff != NULL)
		handoff->s3_resume = (params->power_state->prev_sleep_state ==
				      SLEEP_STATE_S3);
	else {
		printk(BIOS_DEBUG, "Romstage handoff structure not added!\n");
		hard_reset();
	}

	if (IS_ENABLED(CONFIG_LPC_TPM))
		init_tpm(params->power_state->prev_sleep_state == SLEEP_STATE_S3);
}

void after_cache_as_ram_stage(void)
{
	/* Load the ramstage. */
	copy_and_run();
	die("ERROR - Failed to load ramstage!");
}

/* Initialize the power state */
__attribute__((weak)) struct chipset_power_state *fill_power_state(void)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return NULL;
}

__attribute__((weak)) void mainboard_check_ec_image(
	struct romstage_params *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	struct pei_data *pei_data;

	pei_data = params->pei_data;
	if (params->pei_data->boot_mode == SLEEP_STATE_S0) {
		/* Ensure EC is running RO firmware. */
		google_chromeec_check_ec_image(EC_IMAGE_RO);
	}
#endif
}

/* Board initialization before and after RAM is enabled */
__attribute__((weak)) void mainboard_romstage_entry(
	struct romstage_params *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);

	post_code(0x31);

	/* Initliaze memory */
	romstage_common(params);
}

/* Save the DIMM information for SMBIOS table 17 */
__attribute__((weak)) void mainboard_save_dimm_info(
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
__attribute__((weak)) void mainboard_add_dimm_info(
	struct romstage_params *params,
	struct memory_info *mem_info,
	int channel, int dimm, int index)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Get the memory configuration data */
__attribute__((weak)) int mrc_cache_get_current(
	const struct mrc_saved_data **cache)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return -1;
}

/* Save the memory configuration data */
__attribute__((weak)) int mrc_cache_stash_data(void *data, size_t size)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return -1;
}

/* Transition RAM from off or self-refresh to active */
__attribute__((weak)) void raminit(struct romstage_params *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	post_code(0x34);
	die("ERROR - No RAM initialization specified!\n");
}

void ramstage_cache_invalid(void)
{
	if (IS_ENABLED(CONFIG_RESET_ON_INVALID_RAMSTAGE_CACHE))
		/* Perform cold reset on invalid ramstage cache. */
		hard_reset();
}

/* Display the memory configuration */
__attribute__((weak)) void report_memory_config(void)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Choose top of stack and setup MTRRs */
__attribute__((weak)) void *setup_stack_and_mtrrs(void)
{
	printk(BIOS_ERR, "WEAK: %s/%s called\n", __FILE__, __func__);
	die("ERROR - Must specify top of stack!\n");
	return NULL;
}

/* SOC initialization after RAM is enabled */
__attribute__((weak)) void soc_after_ram_init(struct romstage_params *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* SOC initialization before RAM is enabled */
__attribute__((weak)) void soc_pre_ram_init(struct romstage_params *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
