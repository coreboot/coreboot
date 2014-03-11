/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 ChromeOS Authors
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef RMODULE_H
#define RMODULE_H

#include <stdint.h>
#include <stddef.h>
#include <rmodule-defs.h>

enum {
	RMODULE_TYPE_SMM,
	RMODULE_TYPE_SIPI_VECTOR,
	RMODULE_TYPE_STAGE,
	RMODULE_TYPE_VBOOT,
};

struct rmodule;

/* Public API for loading rmdoules. */
int rmodule_parse(void *ptr, struct rmodule *m);
void *rmodule_parameters(const struct rmodule *m);
void *rmodule_entry(const struct rmodule *m);
int rmodule_entry_offset(const struct rmodule *m);
int rmodule_memory_size(const struct rmodule *m);
int rmodule_load(void *loc, struct rmodule *m);
int rmodule_load_alignment(const struct rmodule *m);
/* rmodule_calc_region() calculates the region size, offset to place an
 * rmodule in memory, and load address offset based off of a region allocator
 * with an alignment of region_alignment. This function helps place an rmodule
 * in the same location in ram it will run from. The offset to place the
 * rmodule into the region allocated of size region_size is returned. The
 * load_offset is the address to load and relocate the rmodule.
 * region_alignment must be a power of 2. */
int rmodule_calc_region(unsigned int region_alignment, size_t rmodule_size,
                        size_t *region_size, int *load_offset);

#define DEFINE_RMODULE_HEADER(name_, entry_, type_) \
	struct rmodule_header name_ \
	__attribute__ ((section (".module_header"))) = \
	RMODULE_HEADER(entry_, type_)

/* Support for loading rmodule stages. This API is only available when
 * using dynamic cbmem because it uses the dynamic cbmem API to obtain
 * the backing store region for the stage. */
#if CONFIG_DYNAMIC_CBMEM
struct cbfs_stage;
struct cbmem_entry;

struct rmod_stage_load {
	/* Inputs */
	uint32_t cbmem_id;
	const char *name;
	/* Outputs */
	const struct cbmem_entry *cbmem_entry;
	void *entry;
};

/* Both of the following functions return 0 on success, -1 on error. */
int rmodule_stage_load(struct rmod_stage_load *rsl, struct cbfs_stage *stage);
int rmodule_stage_load_from_cbfs(struct rmod_stage_load *rsl);
#endif

struct rmodule {
	void *location;
	struct rmodule_header *header;
	const void *payload;
	int payload_size;
	void *relocations;
};

/* These are the symbols assumed that every module contains. The linker script
 * provides these symbols. */
extern char _relocations_begin_offset[];
extern char _relocations_end_offset[];
extern char _payload_end_offset[];
extern char _payload_begin_offset[];
extern char _bss[];
extern char _ebss[];
extern char _module_program_size[];
extern char _module_link_start_addr[];
extern char _module_params_begin[];
extern char _module_params_end[];

#endif /* RMODULE_H */
