/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <spi-generic.h>
#include <spi_flash.h>
#include <string.h>
#include <cbmem.h>
#include <cpu/amd/agesa/s3_resume.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <AGESA.h>

typedef enum {
	S3DataTypeNonVolatile=0,	///< NonVolatile Data Type
	S3DataTypeMTRR			///< MTRR storage
} S3_DATA_TYPE;

/* The size needs to be 4k aligned, which is the sector size of most flashes. */
#define S3_DATA_MTRR_SIZE			0x1000
#define S3_DATA_NONVOLATILE_SIZE	0x1000

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME) && \
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

#if defined(__PRE_RAM__)

AGESA_STATUS OemInitResume(AMD_RESUME_PARAMS *ResumeParams)
{
	AMD_S3_PARAMS *dataBlock = &ResumeParams->S3DataBlock;
	uintptr_t pos, size;

	get_s3nv_data(S3DataTypeNonVolatile, &pos, &size);

	/* TODO: Our NvStorage is really const. */
	dataBlock->NvStorageSize = *(UINT32 *) pos;
	dataBlock->NvStorage = (void *) (pos + sizeof(UINT32));
	return AGESA_SUCCESS;
}

AGESA_STATUS OemS3LateRestore(AMD_S3LATE_PARAMS *S3LateParams)
{
	AMD_S3_PARAMS *dataBlock = &S3LateParams->S3DataBlock;
	void *dst;
	size_t len;

	ResumeHeap(&dst, &len);
	dataBlock->VolatileStorageSize = len;
	dataBlock->VolatileStorage = dst;

	return AGESA_SUCCESS;
}

#else

static int spi_SaveS3info(u32 pos, u32 size, u8 *buf, u32 len)
{
#if IS_ENABLED(CONFIG_SPI_FLASH)
	struct spi_flash *flash;

	spi_init();
	flash = spi_flash_probe(0, 0);
	if (!flash)
		return -1;

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_claim_bus(flash->spi);

	flash->erase(flash, pos, size);
	flash->write(flash, pos, sizeof(len), &len);
	flash->write(flash, pos + sizeof(len), len, buf);

	flash->spi->rw = SPI_WRITE_FLAG;
	spi_release_bus(flash->spi);
	return 0;
#else
	return -1;
#endif
}

static u8 MTRRStorage[S3_DATA_MTRR_SIZE];

AGESA_STATUS OemS3Save(void *vS3SaveParams)
{
#if IS_ENABLED(CONFIG_CPU_AMD_PI_00660F01)
	AMD_RTB_PARAMS *S3SaveParams = (AMD_RTB_PARAMS *)vS3SaveParams;
	S3_DATA_BLOCK *dataBlock = &S3SaveParams->S3DataBlock;
#else
	AMD_S3SAVE_PARAMS *S3SaveParams = (AMD_S3SAVE_PARAMS *)vS3SaveParams;
	AMD_S3_PARAMS *dataBlock = &S3SaveParams->S3DataBlock;
#endif
	u32 MTRRStorageSize = 0;
	uintptr_t pos, size;

	if (HIGH_ROMSTAGE_STACK_SIZE)
		cbmem_add(CBMEM_ID_ROMSTAGE_RAM_STACK, HIGH_ROMSTAGE_STACK_SIZE);

	/* To be consumed in AmdInitResume. */
	get_s3nv_data(S3DataTypeNonVolatile, &pos, &size);
	if (size && dataBlock->NvStorageSize)
		spi_SaveS3info(pos, size, dataBlock->NvStorage,
			dataBlock->NvStorageSize);

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

const void *OemS3Saved_MTRR_Storage(void)
{
	uintptr_t pos, size;
	get_s3nv_data(S3DataTypeMTRR, &pos, &size);
	if (!size)
		return NULL;

	return (void*)(pos + sizeof(UINT32));
}

#endif
