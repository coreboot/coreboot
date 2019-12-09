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
 */


#include <stddef.h>
#include <stdint.h>
#include <boot/coreboot_tables.h>
#include <bootstate.h>
#include <console/console.h>
#include <cbmem.h>
#include <device/device.h>
#include "chromeos.h"

#if CONFIG(HAVE_ACPI_TABLES)

static void set_ramoops(chromeos_acpi_t *chromeos, void *ram_oops, size_t size)
{
	if (chromeos == NULL) {
		printk(BIOS_DEBUG, "chromeos gnvs is NULL. ramoops not set.\n");
		return;
	}

	printk(BIOS_DEBUG, "Ramoops buffer: 0x%zx@%p.\n", size, ram_oops);
	chromeos->ramoops_base = (uintptr_t)ram_oops;
	chromeos->ramoops_len = size;
}

static void reserve_ram_oops_dynamic(chromeos_acpi_t *chromeos)
{
	const size_t size = CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE;
	void *ram_oops;

	if (!CONFIG(CHROMEOS_RAMOOPS_DYNAMIC))
		return;

	ram_oops = cbmem_add(CBMEM_ID_RAM_OOPS, size);

	set_ramoops(chromeos, ram_oops, size);
}

#if CONFIG(CHROMEOS_RAMOOPS_DYNAMIC)
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

#elif CONFIG(CHROMEOS_RAMOOPS_NON_ACPI)

static void ramoops_alloc(void *arg)
{
	const size_t size = CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE;

	if (size == 0)
		return;

	if (cbmem_add(CBMEM_ID_RAM_OOPS, size) == NULL)
		printk(BIOS_ERR, "Could not allocate RAMOOPS buffer\n");
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, ramoops_alloc, NULL);

#endif

void lb_ramoops(struct lb_header *header)
{
	void *buffer = cbmem_find(CBMEM_ID_RAM_OOPS);

	if (buffer == NULL)
		return;

	struct lb_range *ramoops;
	ramoops = (struct lb_range *)lb_new_record(header);
	ramoops->tag = LB_TAG_RAM_OOPS;
	ramoops->size = sizeof(*ramoops);
	ramoops->range_start = (uintptr_t)buffer;
	ramoops->range_size = CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE;
}
