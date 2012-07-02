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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,  MA 02110-1301 USA
 */

#ifndef S3_RESUME_H
#define S3_RESUME_H

#define S3_DATA_NONVOLATILE_POS	0xFFFF7000
#define S3_DATA_VOLATILE_POS	0xFFFF0000
#define S3_DATA_MTRR_POS	0xFFFF6000

typedef enum {
	S3DataTypeNonVolatile=0,            ///< NonVolatile Data Type
	S3DataTypeVolatile                  ///< Volatile Data Type
} S3_DATA_TYPE;

void restore_mtrr(void);
void s3_resume(void);
inline void *backup_resume(void);
void set_resume_cache(void);
void move_stack_high_mem(void);

u32 OemAgesaSaveS3Info (S3_DATA_TYPE S3DataType, u32 DataSize, void *Data);
void OemAgesaGetS3Info (S3_DATA_TYPE S3DataType, u32 *DataSize, void **Data);
void OemAgesaSaveMtrr (void);

#endif
