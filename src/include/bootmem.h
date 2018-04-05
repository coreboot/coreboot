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
 */

#ifndef BOOTMEM_H
#define BOOTMEM_H

#include <memrange.h>
#include <stdint.h>
#include <boot/coreboot_tables.h>

/**
 * Bootmem types match to LB_MEM tags.
 * Start at 0x10000 to make sure that the caller doesn't provide LB_MEM tags.
 */
enum bootmem_type {
	BM_MEM_FIRST = 0x10000,	/* First entry in this list */
	BM_MEM_RAM,		/* Memory anyone can use */
	BM_MEM_RESERVED,	/* Don't use this memory region */
	BM_MEM_ACPI,		/* ACPI Tables */
	BM_MEM_NVS,		/* ACPI NVS Memory */
	BM_MEM_UNUSABLE,	/* Unusable address space */
	BM_MEM_VENDOR_RSVD,	/* Vendor Reserved */
	BM_MEM_TABLE,		/* Ram configuration tables are kept in */
	BM_MEM_LAST,		/* Last entry in this list */
};

/* Write memory coreboot table. Current resource map is serialized into
 * memtable (LB_MEM_* types). bootmem library is unusable until this function
 * is called first in the write tables path before payload is loaded. */
void bootmem_write_memory_table(struct lb_memory *mem);

/* Architecture hook to add bootmem areas the architecture controls when
 * bootmem_write_memory_table() is called. */
void bootmem_arch_add_ranges(void);

/* Add a range of a given type to the bootmem address space. */
void bootmem_add_range(uint64_t start, uint64_t size,
		       const enum bootmem_type tag);

/* Print current range map of boot memory. */
void bootmem_dump_ranges(void);

/* Return 1 if region targets usable RAM, 0 otherwise. */
int bootmem_region_targets_usable_ram(uint64_t start, uint64_t size);

/* Allocate a temporary buffer from the unused RAM areas. */
void *bootmem_allocate_buffer(size_t size);

#endif /* BOOTMEM_H */
