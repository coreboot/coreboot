/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#ifndef __AMD_S3_RESUME_H__
#define __AMD_S3_RESUME_H__

#include <stdint.h>

int save_s3_info(void *nv_base, size_t nv_size,
			void *vol_base, size_t vol_size);
void get_s3nv_info(void **base, size_t *size);
void get_s3vol_info(void **base, size_t *size);

#endif /* __AMD_S3_RESUME_H__ */
