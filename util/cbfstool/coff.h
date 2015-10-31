/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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

#define DOS_MAGIC 0x5a4d
typedef struct {
	 uint16_t signature;
	 uint16_t lastsize;
	 uint16_t nblocks;
	 uint16_t nreloc;
	 uint16_t hdrsize;
	 uint16_t minalloc;
	 uint16_t maxalloc;
	 uint16_t ss;
	 uint16_t sp;
	 uint16_t checksum;
	 uint16_t ip;
	 uint16_t cs;
	 uint16_t relocpos;
	 uint16_t noverlay;
	 uint16_t reserved1[4];
	 uint16_t oem_id;
	 uint16_t oem_info;
	 uint16_t reserved2[10];
	 uint32_t e_lfanew;
} dos_header_t;

#define MACHINE_TYPE_X86 0x014c
#define MACHINE_TYPE_X64 0x8664
typedef struct {
	uint8_t  signature[4];
	uint16_t machine;
	uint16_t num_sections;
	uint32_t timestamp;
	uint32_t symboltable;
	uint32_t num_symbols;
	uint16_t opt_header_size;
	uint16_t characteristics;
} coff_header_t;

#define PE_HDR_32_MAGIC 0x10b
typedef struct {
	uint16_t signature;
	uint8_t  major_linker_version;
	uint8_t  minor_linker_version;
	uint32_t code_size;
	uint32_t data_size;
	uint32_t bss_size;
	uint32_t entry_point;
	uint32_t code_offset;
	uint32_t data_offset;
	uint32_t image_addr;
	uint32_t section_alignment;
	uint32_t file_alignment;
	uint16_t major_os_version;
	uint16_t minor_os_version;
	uint16_t major_image_version;
	uint16_t minor_image_version;
	uint16_t major_subsystem_version;
	uint16_t minor_subsystem_version;
	uint32_t reserved;
	uint32_t image_size;
	uint32_t header_size;
	uint32_t checksum;
	uint16_t subsystem;
	uint16_t characteristics;
	uint32_t stack_reserve_size;
	uint32_t stack_commit_size;
	uint32_t heap_reserve_size;
	uint32_t heap_commit_size;
	uint32_t loader_flags;
	uint32_t number_of_va_and_sizes;
	/* data directory not needed */
} pe_opt_header_32_t;

#define PE_HDR_64_MAGIC 0x20b
typedef struct {
	uint16_t signature;
	uint8_t  major_linker_version;
	uint8_t  minor_linker_version;
	uint32_t code_size;
	uint32_t data_size;
	uint32_t bss_size;
	uint32_t entry_point;
	uint32_t code_offset;
	uint64_t image_addr;
	uint32_t section_alignment;
	uint32_t file_alignment;
	uint16_t major_os_version;
	uint16_t minor_os_version;
	uint16_t major_image_version;
	uint16_t minor_image_version;
	uint16_t major_subsystem_version;
	uint16_t minor_subsystem_version;
	uint32_t reserved;
	uint32_t image_size;
	uint32_t header_size;
	uint32_t checksum;
	uint16_t subsystem;
	uint16_t characteristics;
	uint64_t stack_reserve_size;
	uint64_t stack_commit_size;
	uint64_t heap_reserve_size;
	uint64_t heap_commit_size;
	uint32_t loader_flags;
	uint32_t number_of_va_and_sizes;
	/* data directory not needed */
} pe_opt_header_64_t;
