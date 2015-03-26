/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#ifndef S3_RESUME_H
#define S3_RESUME_H

typedef enum {
	S3DataTypeNonVolatile=0,	///< NonVolatile Data Type
	S3DataTypeVolatile,		///< Volatile Data Type
	S3DataTypeMTRR			///< MTRR storage
} S3_DATA_TYPE;

void restore_mtrr(void);
void prepare_for_resume(void);

u32 OemAgesaSaveS3Info (S3_DATA_TYPE S3DataType, u32 DataSize, void *Data);
void OemAgesaGetS3Info (S3_DATA_TYPE S3DataType, u32 *DataSize, void **Data);
void OemAgesaSaveMtrr (void);

void spi_SaveS3info(u32 pos, u32 size, u8 *buf, u32 len);

#endif
