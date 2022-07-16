/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <cbmem.h>
#include <console/console.h>
#include <mrc_cache.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <AGESA.h>
#include <northbridge/amd/agesa/agesa_helper.h>

/* Training data versioning is not supported or tracked. */
#define DEFAULT_MRC_VERSION 0

AGESA_STATUS OemInitResume(AMD_S3_PARAMS *dataBlock)
{
	void *nv_storage = NULL;
	size_t nv_storage_size = 0;

	nv_storage = mrc_cache_current_mmap_leak(MRC_TRAINING_DATA, DEFAULT_MRC_VERSION,
						 &nv_storage_size);

	if (nv_storage == NULL || nv_storage_size == 0) {
		printk(BIOS_ERR, "%s: No valid MRC cache!\n", __func__);
		return AGESA_CRITICAL;
	}

	dataBlock->NvStorage = nv_storage;
	dataBlock->NvStorageSize = nv_storage_size;

	return AGESA_SUCCESS;
}

AGESA_STATUS OemS3LateRestore(AMD_S3_PARAMS *dataBlock)
{
	char *heap = cbmem_find(CBMEM_ID_RESUME_SCRATCH);
	if (heap == NULL)
		return AGESA_FATAL;

	printk(BIOS_DEBUG, "Using resume HEAP at %08x\n",
		(unsigned int)(uintptr_t)heap);

	/* Return allocated CBMEM size, we do not keep track of
	 * how much was actually used.
	 */
	dataBlock->VolatileStorageSize = HIGH_MEMORY_SCRATCH;
	dataBlock->VolatileStorage = heap;
	return AGESA_SUCCESS;
}

AGESA_STATUS OemS3Save(AMD_S3_PARAMS *dataBlock)
{
	if (mrc_cache_stash_data(MRC_TRAINING_DATA, DEFAULT_MRC_VERSION,
				 dataBlock->NvStorage, dataBlock->NvStorageSize) < 0) {
		printk(BIOS_ERR, "%s: Failed to stash MRC data\n", __func__);
		return AGESA_CRITICAL;
	}

	/* To be consumed in AmdS3LateRestore. */
	char *heap = cbmem_add(CBMEM_ID_RESUME_SCRATCH, HIGH_MEMORY_SCRATCH);
	if (heap) {
		memset(heap, 0, HIGH_MEMORY_SCRATCH);
		memcpy(heap, dataBlock->VolatileStorage, dataBlock->VolatileStorageSize);
	}

	/* Collect MTRR setup. */
	backup_mtrr();

	return AGESA_SUCCESS;
}
