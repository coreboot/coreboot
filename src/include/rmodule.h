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

#define RMODULE_MAGIC 0xf8fe
#define RMODULE_VERSION_1 1

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

#define FIELD_ENTRY(x_) ((u32)&x_)
#define RMODULE_HEADER(entry_, type_)					\
{									\
	.magic = RMODULE_MAGIC,						\
	.version = RMODULE_VERSION_1,					\
	.type = type_,							\
	.payload_begin_offset = FIELD_ENTRY(_payload_begin_offset),	\
	.payload_end_offset = FIELD_ENTRY(_payload_end_offset),		\
	.relocations_begin_offset  =					\
		FIELD_ENTRY(_relocations_begin_offset),			\
	.relocations_end_offset =					\
		FIELD_ENTRY(_relocations_end_offset),			\
	.module_link_start_address =					\
		FIELD_ENTRY(_module_link_start_addr),			\
	.module_program_size = FIELD_ENTRY(_module_program_size),	\
	.module_entry_point = FIELD_ENTRY(entry_),			\
	.parameters_begin = FIELD_ENTRY(_module_params_begin),		\
	.parameters_end = FIELD_ENTRY(_module_params_end),		\
	.bss_begin = FIELD_ENTRY(_bss),					\
	.bss_end = FIELD_ENTRY(_ebss),					\
}

#define DEFINE_RMODULE_HEADER(name_, entry_, type_) \
	struct rmodule_header name_ \
	__attribute__ ((section (".module_header"))) = \
	RMODULE_HEADER(entry_, type_)


/* Private data structures below should not be used directly. */

/* All fields with '_offset' in the name are byte offsets into the flat blob.
 * The linker and the linker script takes are of assigning the values.  */
struct rmodule_header {
	u16 magic;
	u8  version;
	u8  type;
	/* The payload represents the program's loadable code and data. */
	u32 payload_begin_offset;
	u32 payload_end_offset;
	/* Begin and of relocation information about the program module. */
	u32 relocations_begin_offset;
	u32 relocations_end_offset;
	/* The starting address of the linked program. This address is vital
	 * for determining relocation offsets as the relocation info and other
	 * symbols (bss, entry point) need this value as a basis to calculate
	 * the offsets.
	 */
	u32 module_link_start_address;
	/* The module_program_size is the size of memory used while running
	 * the program. The program is assumed to consume a contiguous amount
	 * of memory. */
	u32 module_program_size;
	/* This is program's execution entry point. */
	u32 module_entry_point;
	/* Optional parameter structure that can be used to pass data into
	 * the module. */
	u32 parameters_begin;
	u32 parameters_end;
	/* BSS section information so the loader can clear the bss. */
	u32 bss_begin;
	u32 bss_end;
	/* Add some room for growth. */
	u32 padding[4];
} __attribute__ ((packed));

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
