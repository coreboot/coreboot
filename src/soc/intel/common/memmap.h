/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corporation.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _COMMON_MEMMAP_H_
#define _COMMON_MEMMAP_H_

#include <types.h>

/*
 * mmap_region_granluarity must to return a size which is a positive non-zero
 * integer multiple of the SMM size when SMM is in use.  When not using SMM,
 * this value should be set to 8 MiB.
 */
size_t mmap_region_granluarity(void);
void smm_region(void **start, size_t *size);

#endif /* _COMMON_MEMMAP_H_ */
