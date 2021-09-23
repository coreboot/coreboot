/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <stage_cache.h>
#include <mrc_cache.h>
#include <reset.h>
#include <console/console.h>
#include <soc/southbridge.h>
#include <amdblocks/s3_resume.h>
#include <amdblocks/acpi.h>

/* Training data versioning is not supported or tracked. */
#define DEFAULT_MRC_VERSION 0

static void __noreturn reboot_from_resume(const char *message)
{
	printk(BIOS_ERR, "%s", message);
	set_pm1cnt_s5();
	board_reset();
}

AGESA_STATUS OemInitResume(S3_DATA_BLOCK *dataBlock)
{
	void *base;
	size_t size;
	int i;
	uint32_t erased = 0xffffffff;

	base = mrc_cache_current_mmap_leak(MRC_TRAINING_DATA,
					   DEFAULT_MRC_VERSION,
					   &size);
	if (!base || !size)
		reboot_from_resume("Error: S3 NV data not found, rebooting.\n");

	/* Read 16 bytes to infer if the NV has been erased from flash. */
	for (i = 0; i < 4; i++)
		erased &= read32((uint32_t *)base + i);
	if (erased == 0xffffffff)
		reboot_from_resume("Error: S3 NV data invalid, rebooting.\n");

	dataBlock->NvStorage = base;
	dataBlock->NvStorageSize = size;
	printk(BIOS_SPEW, "S3 NV data @%p, 0x%0zx bytes\n",
		dataBlock->NvStorage, (size_t)dataBlock->NvStorageSize);

	return AGESA_SUCCESS;
}

AGESA_STATUS OemS3LateRestore(S3_DATA_BLOCK *dataBlock)
{
	void *base = NULL;
	size_t size = 0;

	stage_cache_get_raw(STAGE_S3_DATA, &base, &size);
	if (!base || !size) {
		printk(BIOS_ERR, "Error: S3 volatile data not found\n");
		return AGESA_FATAL;
	}

	dataBlock->VolatileStorage = base;
	dataBlock->VolatileStorageSize = size;
	printk(BIOS_SPEW, "S3 volatile data @%p, 0x%0zx bytes\n",
		dataBlock->VolatileStorage, (size_t)dataBlock->VolatileStorageSize);

	return AGESA_SUCCESS;
}

AGESA_STATUS OemS3Save(S3_DATA_BLOCK *dataBlock)
{
	if (mrc_cache_stash_data(MRC_TRAINING_DATA, DEFAULT_MRC_VERSION,
			dataBlock->NvStorage, dataBlock->NvStorageSize) < 0) {
		printk(BIOS_ERR, "Failed to stash MRC data\n");
		return AGESA_CRITICAL;
	}

	stage_cache_add_raw(STAGE_S3_DATA, dataBlock->VolatileStorage,
		dataBlock->VolatileStorageSize);

	return AGESA_SUCCESS;
}
