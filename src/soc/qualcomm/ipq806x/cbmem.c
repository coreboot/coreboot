/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
#include <soc/soc_services.h>

static int cbmem_backing_store_ready;

void ipq_cbmem_backing_store_ready(void)
{
	cbmem_backing_store_ready = 1;
}

void *cbmem_top_chipset(void)
{
	/*
	 * In romstage, make sure that cbmem backing store is ready before
	 * returning the pointer to cbmem top. Otherwise, it could lead to
	 * issues with components that utilize cbmem in romstage
	 * (e.g. vboot_locator for loading ipq blobs before DRAM is
	 * initialized).
	 */
	if (cbmem_backing_store_ready == 0)
		return NULL;

	return _memlayout_cbmem_top;
}
