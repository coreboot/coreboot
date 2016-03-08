/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/io.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <fsp/util.h>
#include <inttypes.h>
#include <lib.h>
#include <string.h>

#define HOB_HEADER_LEN		8

struct hob_header {
	uint16_t type;
	uint16_t length;
} __attribute__((packed));

struct hob_resource {
	uint8_t owner_guid[16];
	uint32_t type;
	uint32_t attribute_type;
	uint64_t addr;
	uint64_t length;
} __attribute__((packed));

enum resource_type {
	EFI_RESOURCE_SYSTEM_MEMORY		= 0,
	EFI_RESOURCE_MEMORY_MAPPED_IO		= 1,
	EFI_RESOURCE_IO				= 2,
	EFI_RESOURCE_FIRMWARE_DEVICE		= 3,
	EFI_RESOURCE_MEMORY_MAPPED_IO_PORT	= 4,
	EFI_RESOURCE_MEMORY_RESERVED		= 5,
	EFI_RESOURCE_IO_RESERVED		= 6,
	EFI_RESOURCE_MAX_MEMORY_TYPE		= 7,
};

static const char *resource_names[] = {
	[EFI_RESOURCE_SYSTEM_MEMORY]		= "SYSTEM_MEMORY",
	[EFI_RESOURCE_MEMORY_MAPPED_IO]		= "MMIO",
	[EFI_RESOURCE_IO]			= "IO",
	[EFI_RESOURCE_FIRMWARE_DEVICE]		= "FIRMWARE_DEVICE",
	[EFI_RESOURCE_MEMORY_MAPPED_IO_PORT]	= "MMIO_PORT",
	[EFI_RESOURCE_MEMORY_RESERVED]		= "MEMORY_RESERVED",
	[EFI_RESOURCE_IO_RESERVED]		= "IO_RESERVED",
};

enum hob_type {
	HOB_TYPE_HANDOFF			= 0x0001,
	HOB_TYPE_MEMORY_ALLOCATION		= 0x0002,
	HOB_TYPE_RESOURCE_DESCRIPTOR		= 0x0003,
	HOB_TYPE_GUID_EXTENSION			= 0x0004,
	HOB_TYPE_FV				= 0x0005,
	HOB_TYPE_CPU				= 0x0006,
	HOB_TYPE_MEMORY_POOL			= 0x0007,
	HOB_TYPE_FV2				= 0x0009,
	HOB_TYPE_LOAD_PEIM_UNUSED		= 0x000A,
	HOB_TYPE_UCAPSULE			= 0x000B,
	HOB_TYPE_UNUSED				= 0xFFFE,
	HOB_TYPE_END_OF_HOB_LIST		= 0xFFFF,
};

/* UUIDs (GUIDs) in little-endian, so they can be used with memcmp() */
static const uint8_t uuid_owner_bootloader_tolum[16] = {
	0x56, 0x4f, 0xff, 0x73, 0x8e, 0xaa, 0x51, 0x44,
	0xb3, 0x16, 0x36, 0x35, 0x36, 0x67, 0xad, 0x44,
};

static const uint8_t uuid_owner_fsp[16] = {
	0x59, 0x97, 0xa7, 0x69, 0x73, 0x13, 0x67, 0x43,
	0xa6, 0xc4, 0xc7, 0xf5, 0x9e, 0xfd, 0x98, 0x6e,
};

static const uint8_t uuid_owner_tseg[16] = {
	0x7c, 0x74, 0x38, 0xd0, 0x0c, 0xd0, 0x80, 0x49,
	0xb3, 0x19, 0x49, 0x01, 0x99, 0xa4, 0x7d, 0x55
};

static const uint8_t empty_uuid[16] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const struct uuid_name_map {
	const void *uuid;
	const char *name;
} uuid_names[] = {
	{ uuid_owner_bootloader_tolum,	"BOOTLOADER_TOLUM" },
	{ uuid_owner_fsp,		"FSP_RESERVED_MEMORY" },
	{ uuid_owner_tseg,		"TSEG" },
};

static const char *resource_name(enum resource_type type)
{
	if (type >= ARRAY_SIZE(resource_names))
		return "UNKNOWN";
	return resource_names[type];
}

/*
 * Utilities for walking HOBs
 */

static bool uuid_compare(const uint8_t uuid1[16], const uint8_t uuid2[16])
{
	return !memcmp(uuid1, uuid2, 16);
}

static const char *uuid_name(const uint8_t uuid[16])
{
	size_t i;
	const struct uuid_name_map *owner_entry;

	for (i = 0; i < ARRAY_SIZE(uuid_names); i++) {
		owner_entry = uuid_names + i;
		if (uuid_compare(uuid, owner_entry->uuid))
			return owner_entry->name;
	}
	return "UNKNOWN";
}

static const struct hob_header *next_hob(const struct hob_header *parent)
{
	union {
		const struct hob_header *hob;
		uintptr_t addr;
	} hob_walker;

	hob_walker.hob = parent;
	hob_walker.addr += parent->length;
	return hob_walker.hob;
}

static const void *hob_header_to_struct(const struct hob_header *hob)
{
	union {
		const struct hob_header *hob_hdr;
		const void *hob_descr;
		uintptr_t addr;
	} hob_walker;

	hob_walker.hob_hdr = hob;
	hob_walker.addr += HOB_HEADER_LEN;
	return hob_walker.hob_descr;
}

static const void *hob_header_to_extension_hob(const struct hob_header *hob)
{
	union {
		const struct hob_header *hob_hdr;
		const void *hob_descr;
		uintptr_t addr;
	} hob_walker;

	hob_walker.hob_hdr = hob;
	hob_walker.addr += HOB_HEADER_LEN + 16; /* header and 16-byte UUID */
	return hob_walker.hob_descr;
}

static const
struct hob_resource *hob_header_to_resource(const struct hob_header *hob)
{
	return hob_header_to_struct(hob);
}

/*
 * Utilities for locating and identifying HOBs
 */

void fsp_save_hob_list(void *hob_list_ptr)
{
	void **cbmem_loc;
	cbmem_loc = cbmem_add(CBMEM_ID_FSP_RUNTIME, sizeof(*hob_list_ptr));
	*cbmem_loc = hob_list_ptr;
}

const void *fsp_get_hob_list(void)
{
	void **list_loc = cbmem_find(CBMEM_ID_FSP_RUNTIME);

	return (list_loc) ? (*list_loc) : NULL;
}

static const
struct hob_resource *find_resource_hob_by_uuid(const struct hob_header *hob,
					       const uint8_t uuid[16])
{
	const struct hob_resource *res;

	for ( ; hob->type != HOB_TYPE_END_OF_HOB_LIST; hob = next_hob(hob)) {

		if (hob->type != HOB_TYPE_RESOURCE_DESCRIPTOR)
			continue;

		res = hob_header_to_resource(hob);
		if (uuid_compare(res->owner_guid, uuid))
			return res;
	}
	return NULL;
}

void fsp_find_reserved_memory(struct range_entry *re, const void *hob_list)
{
	const struct hob_resource *fsp_mem;

	range_entry_init(re, 0, 0, 0);

	fsp_mem = find_resource_hob_by_uuid(hob_list, uuid_owner_fsp);

	if (!fsp_mem) {
		return;
	}

	range_entry_init(re, fsp_mem->addr, fsp_mem->addr + fsp_mem->length, 0);
}

/*
 * Utilities for printing HOB information
 */

static void print_guid(const void *base)
{
	uint32_t big;
	uint16_t mid[2];

	const uint8_t *id = base;
	big = read32(id + 0);
	mid[0] = read16(id + 4);
	mid[1] = read16(id + 6);

	printk(BIOS_DEBUG, "%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x",
	       big, mid[0], mid[1],
	       id[8], id[9], id[10], id[11], id[12], id[13], id[14], id[15]);
}

static void print_resource_descriptor(const void *base)
{
	const struct hob_resource *res;

	res = hob_header_to_resource(base);

	printk(BIOS_DEBUG, "Resource %s, attribute %x\n",
			   resource_name(res->type), res->attribute_type);
	printk(BIOS_DEBUG, "\t0x%08llx + 0x%08llx\n", res->addr, res->length);
	if (!uuid_compare(res->owner_guid, empty_uuid)) {
		printk(BIOS_DEBUG, "\tOwner GUID: ");
		print_guid(res->owner_guid);
		printk(BIOS_DEBUG, " (%s)\n", uuid_name(res->owner_guid));
	}
}


void fsp_print_memory_resource_hobs(const void *hob_list)
{
	const struct hob_header *hob = hob_list;

	for ( ; hob->type != HOB_TYPE_END_OF_HOB_LIST; hob = next_hob(hob)) {
		if (hob->type == HOB_TYPE_RESOURCE_DESCRIPTOR)
			print_resource_descriptor(hob);
	}
}

const void *fsp_find_extension_hob_by_uuid(const uint8_t *uuid, size_t *size)
{
	const uint8_t *hob_uuid;
	const struct hob_header *hob = fsp_get_hob_list();

	if (!hob)
		return NULL;

	for ( ; hob->type != HOB_TYPE_END_OF_HOB_LIST; hob = next_hob(hob)) {

		if (hob->type != HOB_TYPE_GUID_EXTENSION)
			continue;

		hob_uuid = hob_header_to_struct(hob);
		if (uuid_compare(hob_uuid, uuid)) {
			*size = hob->length - (HOB_HEADER_LEN + 16);
			return hob_header_to_extension_hob(hob);
		}
	}

	return NULL;
}
