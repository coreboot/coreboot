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

#if CONFIG_HAVE_ACPI_RESUME

/* The size needs to be 4k aligned, which is the sector size of most flashes. */
#define S3_DATA_VOLATILE_SIZE	0x6000
#define S3_DATA_MTRR_SIZE	0x1000
#define S3_DATA_NONVOLATILE_SIZE	0x1000
#define S3_DATA_VOLATILE_POS	CONFIG_S3_DATA_POS
#define S3_DATA_MTRR_POS	(CONFIG_S3_DATA_POS + S3_DATA_VOLATILE_SIZE)
#define S3_DATA_NONVOLATILE_POS	(CONFIG_S3_DATA_POS + S3_DATA_VOLATILE_SIZE + S3_DATA_MTRR_SIZE)

#if (S3_DATA_VOLATILE_SIZE + S3_DATA_MTRR_SIZE + S3_DATA_NONVOLATILE_SIZE) > CONFIG_S3_DATA_SIZE
#error "Please increase the value of S3_DATA_SIZE"
#endif

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

#ifndef __PRE_RAM__
#include <spi_flash.h>
void write_mtrr(struct spi_flash *flash, u32 *p_nvram_pos, unsigned idx);
#endif

#endif

#endif
