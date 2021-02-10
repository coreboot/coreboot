/* SPDX-License-Identifier: GPL-2.0-only */


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

void chromeos_ram_oops_init(chromeos_acpi_t *chromeos)
{
	const size_t size = CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE;
	void *ram_oops;

	ram_oops = cbmem_add(CBMEM_ID_RAM_OOPS, size);

	set_ramoops(chromeos, ram_oops, size);
}

#else

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
