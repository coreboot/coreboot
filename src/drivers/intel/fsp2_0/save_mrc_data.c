/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <mrc_cache.h>

void save_memory_training_data(void)
{
	size_t mrc_data_size;
	const void *mrc_data;
	uint32_t cbmem_id = CONFIG(MRC_CACHE_USING_MRC_VERSION) ? CBMEM_ID_MRC_VERSION :
					 CBMEM_ID_FSPM_VERSION;
	uint32_t *version;

	if (acpi_is_wakeup_s3())
		return;

	version = cbmem_find(cbmem_id);
	if (!version) {
		printk(BIOS_ERR, "Failed to read %s version from cbmem.\n",
				CONFIG(MRC_CACHE_USING_MRC_VERSION) ? "MRC" : "FSP-M");
		return;
	}

	mrc_data = fsp_find_nv_storage_data(&mrc_data_size);
	if (!mrc_data) {
		printk(BIOS_ERR, "FSP_NON_VOLATILE_STORAGE_HOB missing!\n");
		return;
	}

	/*
	 * Save MRC data to SPI flash. By always saving the data this forces
	 * a retrain after a trip through ChromeOS recovery path. The
	 * code which saves the data to flash doesn't write if the latest
	 * training data matches this one.
	 */
	if (mrc_cache_stash_data(MRC_TRAINING_DATA, *version, mrc_data,
				 mrc_data_size) < 0)
		printk(BIOS_ERR, "Failed to stash MRC data\n");
}
