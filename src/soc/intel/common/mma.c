/*
 * This file is part of the coreboot project.
 *
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

#include <boot/coreboot_tables.h>
#include <bootstate.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <lib.h>
#include "mma.h"
#include <soc/romstage.h>
#include <string.h>

#define MMA_TEST_METADATA_FILENAME      "mma_test_metadata.bin"
#define MMA_TEST_NAME_TAG               "MMA_TEST_NAME"
#define MMA_TEST_PARAM_TAG              "MMA_TEST_PARAM"
#define TEST_NAME_MAX_SIZE              30
#define TEST_PARAM_MAX_SIZE             100
#define FSP_MMA_RESULTS_GUID            { 0x8f4e928, 0xf5f, 0x46d4, \
                { 0x84, 0x10, 0x47, 0x9f, 0xda, 0x27, 0x9d, 0xb6 } }
#define MMA_DATA_SIGNATURE      (('M'<<0)|('M'<<8)|('A'<<16)|('D'<<24))

struct mma_data_container {
        u32     mma_signature;  // "MMAD"
        u8      mma_data[0];    // Variable size, platform/run time dependent.
} __attribute__ ((packed));

/*
Format of the MMA test metadata file, stored under CBFS
MMA_TEST_NAME=xxxxxx.efi;MMA_TEST_PARAM=xxxxxx.bin;
*/

/* Returns index in haystack after 'LABEL=' string is found, < 0 on error. */
static int find_label(const char *haystack, size_t haystack_sz,
		const char *label)
{
	size_t label_sz;
	size_t i;
	size_t search_sz;

	label_sz = strlen(label);

	if (label_sz + 1 >= haystack_sz)
		return -1;

	/* Handle '=' follow label. i.e. LABEL= */
	search_sz = haystack_sz - label_sz - 1;
	for (i = 0; i < search_sz; i++) {
		if (!strncmp(&haystack[i], label, label_sz))
			break;
	}

	if (i == search_sz)
		return -1;

	if (haystack[i + label_sz] != '=')
		return -1;

	return i + label_sz + 1;
}
/*
 * Fill in value in dest field located by LABEL=.
 *	Returns 0 on success, < 0 on  error.
 */
static int label_value(const char *haystack, size_t haystack_sz,
			const char *label, char *dest, size_t dest_sz)
{
	size_t val_begin;
	size_t val_end;
	size_t val_sz;
	int val_index;

	memset(dest, 0, dest_sz);

	/* Allow for NULL termination. */
	dest_sz--;
	val_index = find_label(haystack, haystack_sz, label);
	if (val_index < 0)
		return -1;

	val_begin = val_index;
	val_end = val_begin;
	val_sz = 0;

	for (val_end = val_begin; val_end < haystack_sz; val_end++) {
		if (haystack[val_end] == ';') {
			val_sz = val_end - val_begin;
			break;
		}
	}

	if (val_end == haystack_sz)
		return -1;

	if (dest_sz < val_sz)
		return -1;

	memcpy(dest, &haystack[val_begin], val_sz);

	return 0;
}

void setup_mma(MEMORY_INIT_UPD *memory_params)
{
	void *mma_test_metadata, *mma_test_content, *mma_test_param;
	size_t mma_test_metadata_file_len, mma_test_content_file_len,
	       mma_test_param_file_len;
	char test_filename[TEST_NAME_MAX_SIZE],
	     test_param_filename[TEST_PARAM_MAX_SIZE];

	printk(BIOS_DEBUG, "Entry setup_mma\n");

	memory_params->MmaTestContentPtr = 0;
	memory_params->MmaTestContentSize = 0;
	memory_params->MmaTestConfigPtr = 0;
	memory_params->MmaTestConfigSize = 0;

	mma_test_metadata = cbfs_boot_map_with_leak(MMA_TEST_METADATA_FILENAME,
			CBFS_TYPE_MMA , &mma_test_metadata_file_len);

	if (!mma_test_metadata) {
		printk(BIOS_DEBUG, "MMA setup failed: Failed to read %s\n",
				MMA_TEST_METADATA_FILENAME);
		return;
	}

	if (label_value(mma_test_metadata, mma_test_metadata_file_len,
			MMA_TEST_NAME_TAG, test_filename, TEST_NAME_MAX_SIZE)) {
			printk(BIOS_DEBUG, "MMA setup failed : Failed to get %s",
					MMA_TEST_NAME_TAG);
			return;
	}

	if (label_value(mma_test_metadata, mma_test_metadata_file_len,
			MMA_TEST_PARAM_TAG, test_param_filename,
			TEST_PARAM_MAX_SIZE)) {
		printk(BIOS_DEBUG, "MMA setup failed : Failed to get %s",
			MMA_TEST_PARAM_TAG);
		return;
	}

	printk(BIOS_DEBUG, "Got MMA_TEST_NAME=%s MMA_TEST_PARAM=%s\n",
			test_filename, test_param_filename);

	mma_test_content = cbfs_boot_map_with_leak(test_filename,
				CBFS_TYPE_EFI , &mma_test_content_file_len);
	if (!mma_test_content) {
		printk(BIOS_DEBUG, "MMA setup failed: Failed to read %s.\n",
		test_filename);
		return;
	}

	mma_test_param = cbfs_boot_map_with_leak(test_param_filename,
				CBFS_TYPE_MMA , &mma_test_param_file_len);
	if (!mma_test_param) {
		printk(BIOS_DEBUG, "MMA setup failed: Failed to read %s.\n",
				test_param_filename);
		return;
	}

	memory_params->MmaTestContentPtr = (uintptr_t) mma_test_content;
	memory_params->MmaTestContentSize = mma_test_content_file_len;
	memory_params->MmaTestConfigPtr = (uintptr_t) mma_test_param;
	memory_params->MmaTestConfigSize = mma_test_param_file_len;
	memory_params->MrcFastBoot = 0x00;

	printk(BIOS_DEBUG, "MMA Test name %s\n", test_filename);
	printk(BIOS_DEBUG, "MMA Test Config name %s\n", test_param_filename);
	printk(BIOS_DEBUG, "MMA passing following memory_params\n");
	printk(BIOS_DEBUG, "memory_params->MmaTestContentPtr = %0x\n",
			memory_params->MmaTestContentPtr);
	printk(BIOS_DEBUG, "memory_params->MmaTestContentSize = %d\n",
			memory_params->MmaTestContentSize);
	printk(BIOS_DEBUG, "memory_params->MmaTestConfigPtr = %0x\n",
			memory_params->MmaTestConfigPtr);
	printk(BIOS_DEBUG, "memory_params->MmaTestConfigSize = %d\n",
			memory_params->MmaTestConfigSize);
	printk(BIOS_DEBUG, "memory_params->MrcFastBoot = %d\n",
			memory_params->MrcFastBoot);
	printk(BIOS_DEBUG, "MMA setup successfully\n");
}

static void save_mma_results_data(void *unused)
{
	void *mma_results_hob;
	u32 mma_hob_size;
	u32 *mma_hob_data;
	struct mma_data_container *mma_data;
	int cbmem_size;

	const EFI_GUID mma_results_guid = FSP_MMA_RESULTS_GUID;

	printk(BIOS_DEBUG, "Entry save_mma_results_data MMA save data.\n");

	mma_results_hob = get_first_guid_hob(&mma_results_guid);
	if (mma_results_hob == NULL) {
		printk(BIOS_DEBUG,
				"MMA results data Hob not present\n");
		return;
	}

	mma_hob_data = GET_GUID_HOB_DATA(mma_results_hob);
	mma_hob_size = GET_HOB_LENGTH(mma_results_hob);
	cbmem_size = ALIGN(mma_hob_size, 16) +
			sizeof(struct mma_data_container);
	mma_data = cbmem_add(CBMEM_ID_MMA_DATA, cbmem_size);

	if (mma_data == NULL) {
		printk(BIOS_DEBUG,
			"CBMEM was not available to save the MMA data.\n");
		return;
	}

	/*clear the mma_data before coping the actual data */
	memset(mma_data, 0, cbmem_size);

	printk(BIOS_DEBUG,
		"Copy MMA DATA to HOB(src addr %p, dest addr %p, %u bytes)\n",
			mma_hob_data, mma_data, mma_hob_size);

	mma_data->mma_signature = MMA_DATA_SIGNATURE;
	memcpy(mma_data->mma_data, mma_hob_data, mma_hob_size);

	printk(BIOS_DEBUG, "write MMA results data to cbmem success\n");
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY,
			save_mma_results_data, NULL);
