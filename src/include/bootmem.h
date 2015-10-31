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

/*
 * Initialize the memory address space prior to payload loading. The bootmem
 * serves as the source for the lb_mem table.
 */
void bootmem_init(void);

/* Add a range of a given type to the bootmem address space. */
void bootmem_add_range(uint64_t start, uint64_t size, uint32_t type);

/* Write memory coreboot table. */
void bootmem_write_memory_table(struct lb_memory *mem);

/* Print current range map of boot memory. */
void bootmem_dump_ranges(void);

/* Return 1 if region targets usable RAM, 0 otherwise. */
int bootmem_region_targets_usable_ram(uint64_t start, uint64_t size);

/* Allocate a temporary buffer from the unused RAM areas. */
void *bootmem_allocate_buffer(size_t size);

#endif /* BOOTMEM_H */
