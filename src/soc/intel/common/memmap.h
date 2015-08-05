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
 * Foundation, Inc.
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

/* Fills in the arguments for the entire SMM region covered by chipset
 * protections. e.g. TSEG. */
void smm_region(void **start, size_t *size);

enum {
	/* SMM handler area. */
	SMM_SUBREGION_HANDLER,
	/* SMM cache region. */
	SMM_SUBREGION_CACHE,
	/* Chipset specific area. */
	SMM_SUBREGION_CHIPSET,
	/* Total sub regions supported. */
	SMM_SUBREGION_NUM,
};

/* Fills in the start and size for the requested SMM subregion. Returns
 * 0 on susccess, < 0 on failure. */
int smm_subregion(int sub, void **start, size_t *size);

#endif /* _COMMON_MEMMAP_H_ */
