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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <cbmem.h>
#include "haswell.h"

unsigned long get_top_of_ram(void)
{
	/*
	 * Base of TSEG is top of usable DRAM below 4GiB. The register has
	 * 1 MiB alignement.
	 */
	u32 tom = pci_read_config32(PCI_DEV(0,0,0), TSEG);
	return (unsigned long) tom & ~((1 << 20) - 1);
}
