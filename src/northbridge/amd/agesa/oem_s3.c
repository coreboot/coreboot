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
#include <cpu/amd/agesa/s3_resume.h>

/* The size needs to be 4k aligned, which is the sector size of most flashes. */
#define S3_DATA_VOLATILE_SIZE		0x6000
#define S3_DATA_MTRR_SIZE			0x1000
#define S3_DATA_NONVOLATILE_SIZE	0x1000

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME) && \
	(S3_DATA_VOLATILE_SIZE + S3_DATA_MTRR_SIZE + S3_DATA_NONVOLATILE_SIZE) > CONFIG_S3_DATA_SIZE
#error "Please increase the value of S3_DATA_SIZE"
#endif

void get_s3nv_data(S3_DATA_TYPE S3DataType, u32 *pos, u32 *len)
{
	/* FIXME: Find file from CBFS. */
	u32 s3_data = CONFIG_S3_DATA_POS;

	switch (S3DataType) {
	case S3DataTypeVolatile:
		*pos = s3_data;
		*len = S3_DATA_VOLATILE_SIZE;
		break;
	case S3DataTypeMTRR:
		*pos = s3_data + S3_DATA_VOLATILE_SIZE;
		*len = S3_DATA_MTRR_SIZE;
		break;
	case S3DataTypeNonVolatile:
		*pos = s3_data + S3_DATA_VOLATILE_SIZE + S3_DATA_MTRR_SIZE;
		*len = S3_DATA_NONVOLATILE_SIZE;
		break;
	default:
		*pos = 0;
		*len = 0;
		break;
	}
}

int spi_SaveS3info(u32 pos, u32 size, u8 *buf, u32 len)
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
