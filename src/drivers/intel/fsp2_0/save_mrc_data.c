/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <mrc_cache.h>

static void save_mrc_data(void *unused)
{
	size_t mrc_data_size;
	const void *mrc_data;
	uint32_t *fspm_version;

	if (acpi_is_wakeup_s3())
		return;

	fspm_version = cbmem_find(CBMEM_ID_FSPM_VERSION);
	if (!fspm_version) {
		printk(BIOS_ERR, "Failed to read FSP-M version from cbmem.\n");
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
	if (mrc_cache_stash_data(MRC_TRAINING_DATA, *fspm_version, mrc_data,
				 mrc_data_size) < 0)
		printk(BIOS_ERR, "Failed to stash MRC data\n");
}

/*
 * Should be done before ramstage_cse_fw_sync() to avoid traning memory twice on
 * a cold boot after a full firmware update.
 */
BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, save_mrc_data, NULL);
