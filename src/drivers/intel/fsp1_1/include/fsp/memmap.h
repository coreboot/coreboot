/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corporation.
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

#ifndef _COMMON_MEMMAP_H_
#define _COMMON_MEMMAP_H_

#include <types.h>

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
