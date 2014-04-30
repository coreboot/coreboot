/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <arch/io.h>
#include <cbmem.h>
#include <device/pci.h>
#include <broadwell/pci_devs.h>
#include <broadwell/systemagent.h>

static unsigned long get_top_of_ram(void)
{
	/*
	 * Base of TSEG is top of usable DRAM below 4GiB. The register has
	 * 1 MiB alignement.
	 */
	u32 tom = pci_read_config32(SA_DEV_ROOT, TSEG);
	return (unsigned long) tom & ~((1 << 20) - 1);
}

void *cbmem_top(void)
{
	return (void *)get_top_of_ram();
}
