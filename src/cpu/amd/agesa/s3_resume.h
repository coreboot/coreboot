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
 */

#ifndef S3_RESUME_H
#define S3_RESUME_H

void restore_mtrr(void);
void prepare_for_resume(void);
void set_resume_cache(void);

void backup_mtrr(void *mtrr_store, u32 *mtrr_store_size);
const void *OemS3Saved_MTRR_Storage(void);

#endif
