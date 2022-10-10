/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <soc/intel/common/mma.h>
#include <string.h>

#define MMA_TEST_METADATA_FILENAME	"mma_test_metadata.bin"
#define MMA_TEST_NAME_TAG		"MMA_TEST_NAME"
#define MMA_TEST_PARAM_TAG		"MMA_TEST_PARAM"
#define TEST_NAME_MAX_SIZE		30
#define TEST_PARAM_MAX_SIZE		100
#define MMA_DATA_SIGNATURE		(('M' << 0) | ('M' << 8) | \
					('A' << 16) | ('D' << 24))

struct mma_data_container {
	uint32_t mma_signature; /* "MMAD" */
	uint8_t mma_data[0]; /* Variable size, platform/run time dependent. */
} __packed;

/*
 * Format of the MMA test metadata file, stored under CBFS
 * MMA_TEST_NAME=xxxxxx.efi;MMA_TEST_PARAM=xxxxxx.bin;
 */

/* Returns index in haystack after 'LABEL='
 * string is found, < 0 on error.
 */
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
 * Returns 0 on success, < 0 on  error.
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

int mma_map_param(struct mma_config_param *mma_cfg)
{
	void *mma_test_metadata;
	size_t mma_test_metadata_file_len;
	char test_filename[TEST_NAME_MAX_SIZE],
		test_param_filename[TEST_PARAM_MAX_SIZE];
	bool metadata_parse_flag = true;

	printk(BIOS_DEBUG, "MMA: Entry %s\n", __func__);

	mma_test_metadata = cbfs_ro_map(MMA_TEST_METADATA_FILENAME,
					&mma_test_metadata_file_len);
	if (!mma_test_metadata) {
		printk(BIOS_DEBUG, "MMA: Failed to map %s\n",
				MMA_TEST_METADATA_FILENAME);
		return -1;
	}

	if (label_value(mma_test_metadata, mma_test_metadata_file_len,
			MMA_TEST_NAME_TAG, test_filename,
				TEST_NAME_MAX_SIZE)) {
		printk(BIOS_DEBUG, "MMA: Failed to get %s\n",
				MMA_TEST_NAME_TAG);
		metadata_parse_flag = false;
	}

	if (metadata_parse_flag &&
		label_value(mma_test_metadata, mma_test_metadata_file_len,
			MMA_TEST_PARAM_TAG, test_param_filename,
				TEST_PARAM_MAX_SIZE)) {
		printk(BIOS_DEBUG, "MMA: Failed to get %s\n",
			MMA_TEST_PARAM_TAG);
		metadata_parse_flag = false;
	}

	cbfs_unmap(mma_test_metadata);

	if (!metadata_parse_flag)
		return -1;

	printk(BIOS_DEBUG, "MMA: Got MMA_TEST_NAME=%s MMA_TEST_PARAM=%s\n",
			test_filename, test_param_filename);

	mma_cfg->test_content = cbfs_ro_map(test_filename, &mma_cfg->test_content_size);
	if (!mma_cfg->test_content) {
		printk(BIOS_DEBUG, "MMA: Failed to map %s\n", test_filename);
		return -1;
	}

	mma_cfg->test_param = cbfs_ro_map(test_param_filename, &mma_cfg->test_param_size);
	if (!mma_cfg->test_param) {
		printk(BIOS_DEBUG, "MMA: Failed to map %s\n", test_param);
		return -1;
	}

	printk(BIOS_DEBUG, "MMA: %s exit success\n", __func__);

	return 0;
}

static void save_mma_results_data(void *unused)
{
	const void *mma_hob;
	size_t mma_hob_size;
	struct mma_data_container *mma_data;
	size_t mma_data_size;

	printk(BIOS_DEBUG, "MMA: Entry %s\n", __func__);

	if (fsp_locate_mma_results(&mma_hob, &mma_hob_size)) {
		printk(BIOS_DEBUG,
				"MMA: results data Hob not present\n");
		return;
	}

	mma_data_size = ALIGN_UP(mma_hob_size, 16) +
			sizeof(struct mma_data_container);

	mma_data = cbmem_add(CBMEM_ID_MMA_DATA, mma_data_size);

	if (mma_data == NULL) {
		printk(BIOS_DEBUG,
			"MMA: CBMEM was not available to save the MMA data.\n");
		return;
	}

	/*clear the mma_data before coping the actual data */
	memset(mma_data, 0, mma_data_size);

	printk(BIOS_DEBUG,
		"MMA: copy MMA data to CBMEM(src %p, dest %p, %u bytes)\n",
			mma_hob, mma_data, mma_hob_size);

	mma_data->mma_signature = MMA_DATA_SIGNATURE;
	memcpy(mma_data->mma_data, mma_hob, mma_hob_size);

	printk(BIOS_DEBUG, "MMA: %s exit successfully\n", __func__);
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY,
			save_mma_results_data, NULL);
