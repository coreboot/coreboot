/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <cbmem.h>
#include "haswell.h"

static uintptr_t smm_region_start(void)
{
	/*
	 * Base of TSEG is top of usable DRAM below 4GiB. The register has
	 * 1 MiB alignement.
	 */
	uintptr_t tom = pci_read_config32(PCI_DEV(0,0,0), TSEG);
	return tom & ~((1 << 20) - 1);
}

void *cbmem_top(void)
{
	return (void *)smm_region_start();
}
