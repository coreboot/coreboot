/* SPDX-License-Identifier: GPL-2.0-only */

#include <spi-generic.h>
#include <spi_flash.h>
#include <string.h>
#include <cbmem.h>
#include <console/console.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <AGESA.h>
#include <northbridge/amd/agesa/agesa_helper.h>

typedef enum {
	S3DataTypeNonVolatile = 0,	///< NonVolatile Data Type
	S3DataTypeMTRR			///< MTRR storage
} S3_DATA_TYPE;

/* The size needs to be 4k aligned, which is the sector size of most flashes. */
#define S3_DATA_MTRR_SIZE			0x1000
#define S3_DATA_NONVOLATILE_SIZE	0x1000

#if CONFIG(HAVE_ACPI_RESUME) && \
	(S3_DATA_MTRR_SIZE + S3_DATA_NONVOLATILE_SIZE) > CONFIG_S3_DATA_SIZE
#error "Please increase the value of S3_DATA_SIZE"
#endif

static void get_s3nv_data(S3_DATA_TYPE S3DataType, uintptr_t *pos, uintptr_t *len)
{
	/* FIXME: Find file from CBFS. */
	u32 s3_data = CONFIG_S3_DATA_POS;

	switch (S3DataType) {
	case S3DataTypeMTRR:
		*pos = s3_data;
		*len = S3_DATA_MTRR_SIZE;
		break;
	case S3DataTypeNonVolatile:
		*pos = s3_data + S3_DATA_MTRR_SIZE;
		*len = S3_DATA_NONVOLATILE_SIZE;
		break;
	default:
		*pos = 0;
		*len = 0;
		break;
	}
}

AGESA_STATUS OemInitResume(AMD_S3_PARAMS *dataBlock)
{
	uintptr_t pos, size;
	get_s3nv_data(S3DataTypeNonVolatile, &pos, &size);

	u32 len = *(u32*)pos;

	/* Test for uninitialized s3nv data in SPI. */
	if (len == 0 || len == (u32)-1ULL)
		return AGESA_FATAL;

	dataBlock->NvStorageSize = len;
	dataBlock->NvStorage = (void *) (pos + sizeof(u32));
	return AGESA_SUCCESS;
}

AGESA_STATUS OemS3LateRestore(AMD_S3_PARAMS *dataBlock)
{
	char *heap = cbmem_find(CBMEM_ID_RESUME_SCRATCH);
	if (heap == NULL)
		return AGESA_FATAL;

	printk(BIOS_DEBUG, "Using resume HEAP at %08x\n",
		(unsigned int)(uintptr_t) heap);

	/* Return allocated CBMEM size, we do not keep track of
	 * how much was actually used.
	 */
	dataBlock->VolatileStorageSize = HIGH_MEMORY_SCRATCH;
	dataBlock->VolatileStorage = heap;
	return AGESA_SUCCESS;
}

#if ENV_RAMSTAGE

static int spi_SaveS3info(u32 pos, u32 size, u8 *buf, u32 len)
{
#if CONFIG(SPI_FLASH)
	struct spi_flash flash;

	spi_init();
	if (spi_flash_probe(0, 0, &flash))
		return -1;

	spi_flash_volatile_group_begin(&flash);

	spi_flash_erase(&flash, pos, size);
	spi_flash_write(&flash, pos, sizeof(len), &len);
	spi_flash_write(&flash, pos + sizeof(len), len, buf);

	spi_flash_volatile_group_end(&flash);
	return 0;
#else
	return -1;
#endif
}

__aligned((sizeof(msr_t))) static u8 MTRRStorage[S3_DATA_MTRR_SIZE];

AGESA_STATUS OemS3Save(AMD_S3_PARAMS *dataBlock)
{
	u32 MTRRStorageSize = 0;
	uintptr_t pos, size;

	/* To be consumed in AmdInitResume. */
	get_s3nv_data(S3DataTypeNonVolatile, &pos, &size);
	if (size && dataBlock->NvStorageSize)
		spi_SaveS3info(pos, size, dataBlock->NvStorage,
			dataBlock->NvStorageSize);
	else
		printk(BIOS_EMERG,
			"Error: Cannot store memory training results in SPI.\n"
			"Error: S3 resume will not be possible.\n"
		);

	/* To be consumed in AmdS3LateRestore. */
	char *heap = cbmem_add(CBMEM_ID_RESUME_SCRATCH, HIGH_MEMORY_SCRATCH);
	if (heap) {
		memset(heap, 0, HIGH_MEMORY_SCRATCH);
		memcpy(heap, dataBlock->VolatileStorage, dataBlock->VolatileStorageSize);
	}

	/* Collect MTRR setup. */
	backup_mtrr(MTRRStorage, &MTRRStorageSize);

	/* To be consumed in restore_mtrr, CPU enumeration in ramstage. */
	get_s3nv_data(S3DataTypeMTRR, &pos, &size);
	if (size && MTRRStorageSize)
		spi_SaveS3info(pos, size, MTRRStorage, MTRRStorageSize);

	return AGESA_SUCCESS;
}

#endif /* ENV_RAMSTAGE */

const void *OemS3Saved_MTRR_Storage(void)
{
	uintptr_t pos, size;
	get_s3nv_data(S3DataTypeMTRR, &pos, &size);
	if (!size)
		return NULL;

	return (void *)(pos + sizeof(UINT32));
}
