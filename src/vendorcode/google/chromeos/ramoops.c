/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
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


#include <stddef.h>
#include <stdint.h>
#include <cbmem.h>
#include <device/device.h>
#include "chromeos.h"

#if CONFIG_CHROMEOS_RAMOOPS_DYNAMIC
void chromeos_reserve_ram_oops_dynamic(chromeos_acpi_t *chromeos)
{
	const size_t size = CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE;
	void *ram_oops;

	ram_oops = cbmem_add(CBMEM_ID_RAM_OOPS, size);

	chromeos->ramoops_base = (uintptr_t)ram_oops;
	chromeos->ramoops_len = size;
}
#else /* CONFIG_CHROMEOS_RAMOOPS_DYNAMIC */
void chromeos_reserve_ram_oops(struct device *dev, int idx)
{
	const unsigned long base = CONFIG_CHROMEOS_RAMOOPS_RAM_START >> 10;
	const unsigned long size = CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE >> 10;

	reserved_ram_resource(dev, idx, base, size);
}
#endif /* CONFIG_CHROMEOS_RAMOOPS_DYNAMIC */
