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
#ifndef RMODULE_DEFS_H
#define RMODULE_DEFS_H

#include <stdint.h>
#include <stddef.h>

#define RMODULE_MAGIC 0xf8fe
#define RMODULE_VERSION_1 1

/* All fields with '_offset' in the name are byte offsets into the flat blob.
 * The linker and the linker script takes are of assigning the values.  */
struct rmodule_header {
	uint16_t magic;
	uint8_t  version;
	uint8_t  type;
	/* The payload represents the program's loadable code and data. */
	uint32_t payload_begin_offset;
	uint32_t payload_end_offset;
	/* Begin and of relocation information about the program module. */
	uint32_t relocations_begin_offset;
	uint32_t relocations_end_offset;
	/* The starting address of the linked program. This address is vital
	 * for determining relocation offsets as the relocation info and other
	 * symbols (bss, entry point) need this value as a basis to calculate
	 * the offsets.
	 */
	uint32_t module_link_start_address;
	/* The module_program_size is the size of memory used while running
	 * the program. The program is assumed to consume a contiguous amount
	 * of memory. */
	uint32_t module_program_size;
	/* This is program's execution entry point. */
	uint32_t module_entry_point;
	/* Optional parameter structure that can be used to pass data into
	 * the module. */
	uint32_t parameters_begin;
	uint32_t parameters_end;
	/* BSS section information so the loader can clear the bss. */
	uint32_t bss_begin;
	uint32_t bss_end;
	/* Add some room for growth. */
	uint32_t padding[4];
} __attribute__ ((packed));

#endif /* RMODULE_DEFS_H */
