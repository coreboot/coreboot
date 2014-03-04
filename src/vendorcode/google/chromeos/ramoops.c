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
#include <bootstate.h>
#include <console/console.h>
#include <cbmem.h>
#include <device/device.h>
#include "chromeos.h"

static void set_ramoops(chromeos_acpi_t *chromeos, void *ram_oops, size_t size)
{
	if (chromeos == NULL) {
		printk(BIOS_DEBUG, "chromeos gnvs is NULL. ramoops not set.\n");
		return;
	}

	printk(BIOS_DEBUG, "Ramoops buffer: 0x%zx@0x%p.\n", size, ram_oops);
	chromeos->ramoops_base = (uintptr_t)ram_oops;
	chromeos->ramoops_len = size;
}

static void reserve_ram_oops_dynamic(chromeos_acpi_t *chromeos)
{
	const size_t size = CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE;
	void *ram_oops;

	if (!IS_ENABLED(CONFIG_CHROMEOS_RAMOOPS_DYNAMIC))
		return;

	ram_oops = cbmem_add(CBMEM_ID_RAM_OOPS, size);

	set_ramoops(chromeos, ram_oops, size);
}

#if CONFIG_CHROMEOS_RAMOOPS_DYNAMIC
static inline void set_global_chromeos_pointer(chromeos_acpi_t *chromeos) {}
#else /* !CONFIG_CHROMEOS_RAMOOPS_DYNAMIC */

static const unsigned long ramoops_base = CONFIG_CHROMEOS_RAMOOPS_RAM_START;
static const unsigned long ramoops_size = CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE;

/*
 * Save pointer to chromeos structure in memory. This is needed because the
 * memory reservation is not done when chromeos_init() is called. However,
 * the pointer to the chromeos_acpi_t structure is needed to update the
 * fields with the rserved base and size.
 */
static chromeos_acpi_t *g_chromeos;

static void set_global_chromeos_pointer(chromeos_acpi_t *chromeos)
{
	g_chromeos = chromeos;
}

static void update_gnvs(void *arg)
{
	chromeos_acpi_t **chromeos = arg;

	set_ramoops(*chromeos, (void *)ramoops_base, ramoops_size);
}

static BOOT_STATE_CALLBACK(bscb_ramoops, update_gnvs, &g_chromeos);

void chromeos_reserve_ram_oops(struct device *dev, int idx)
{
	const unsigned long base = ramoops_base >> 10;
	const unsigned long size = ramoops_size >> 10;

	reserved_ram_resource(dev, idx, base, size);

	boot_state_sched_on_exit(&bscb_ramoops, BS_WRITE_TABLES);
}
#endif /* CONFIG_CHROMEOS_RAMOOPS_DYNAMIC */

void chromeos_ram_oops_init(chromeos_acpi_t *chromeos)
{
	set_global_chromeos_pointer(chromeos);
	reserve_ram_oops_dynamic(chromeos);
}

