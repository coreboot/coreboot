/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Silverback, ltd.
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

#ifndef __AMD_IMAGE_H__
#define __AMD_IMAGE_H__

#include <stdint.h>

void *amd_find_image(const void *start_address, const void *end_address,
			uint32_t alignment, const char name[8]);

#endif /* __AMD_IMAGE_H__ */
