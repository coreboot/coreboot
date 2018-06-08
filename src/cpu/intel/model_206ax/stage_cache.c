/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google, Inc.
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

#include <cbmem.h>
#include <stage_cache.h>
#include "model_206ax.h"

void stage_cache_external_region(void **base, size_t *size)
{
	/*
	 * The ramstage cache lives in the TSEG region at RESERVED_SMM_OFFSET.
	 * The top of RAM is defined to be the TSEG base address.
	 */
	*size = RESERVED_SMM_SIZE;
	*base = (void *)((uintptr_t)cbmem_top() + RESERVED_SMM_OFFSET);
}
