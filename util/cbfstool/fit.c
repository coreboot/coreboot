/*
 * Firmware Interface Table support.
 *
 * Copyright (C) 2012 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "cbfs.h"
#include "cbfs_image.h"
#include "fit.h"

/* FIXME: This code assumes it is being executed on a little endian machine. */

#define FIT_POINTER_LOCATION 0xffffffc0
#define FIT_TABLE_LOWEST_ADDRESS ((uint32_t)(-(16 << 20)))
#define FIT_ENTRY_CHECKSUM_VALID 0x80
#define FIT_TYPE_HEADER 0x0
#define   FIT_HEADER_VERSION 0x0100
#define   FIT_HEADER_ADDRESS "_FIT_   "
#define FIT_TYPE_MICROCODE 0x1
#define   FIT_MICROCODE_VERSION 0x0100

struct fit_entry {
	uint64_t address;
	uint32_t size_reserved;
	uint16_t version;
	uint8_t  type_checksum_valid;
	uint8_t  checksum;
} __attribute__ ((packed));

struct fit_table {
	struct fit_entry header;
	struct fit_entry entries[0];
} __attribute__ ((packed));

struct microcode_header {
	uint32_t version;
	uint32_t revision;
	uint32_t date;
	uint32_t processor_signature;
	uint32_t checksum;
	uint32_t loader_revision;
	uint32_t processor_flags;
	uint32_t data_size;
	uint32_t total_size;
	uint8_t  reserved[12];
} __attribute__ ((packed));

struct microcode_entry {
	int offset;
	int size;
};

static inline void *rom_buffer_pointer(struct cbfs_image *image, int offset)
{
	return &image->buffer.data[offset];
}

static inline int fit_entry_size_bytes(struct fit_entry *entry)
{
	return (entry->size_reserved & 0xffffff) << 4;
}

static inline void fit_entry_update_size(struct fit_entry *entry,
					 int size_bytes)
{
	/* Size is multiples of 16 bytes. */
	entry->size_reserved = (size_bytes >> 4) & 0xffffff;
}

static inline void fit_entry_add_size(struct fit_entry *entry,
                                      int size_bytes)
{
	int size = fit_entry_size_bytes(entry);
	size += size_bytes;
	fit_entry_update_size(entry, size);
}

static inline int fit_entry_type(struct fit_entry *entry)
{
	return entry->type_checksum_valid & ~FIT_ENTRY_CHECKSUM_VALID;
}

/*
 * Get an offset from a host pointer. This function assumes the ROM is located
 * in the host address space at [4G - romsize -> 4G). It also assume all
 * pointers have values within this address range.
 */
static inline int ptr_to_offset(uint32_t romsize, uint32_t host_ptr)
{
	return (int)(romsize + host_ptr);
}

/*
 * Get a pointer from an offset. This function assumes the ROM is located
 * in the host address space at [4G - romsize -> 4G). It also assume all
 * pointers have values within this address range.
 */
static inline uint32_t offset_to_ptr(uint32_t romsize, int offset)
{
	return -(romsize - (uint32_t )offset);
}

static struct fit_table *locate_fit_table(struct cbfs_image *image)
{
	struct fit_table *table;
	uint32_t *fit_pointer;

	fit_pointer = rom_buffer_pointer(image,
                              ptr_to_offset(image->buffer.size, FIT_POINTER_LOCATION));

	/* Ensure pointer is below 4GiB and within 16MiB of 4GiB */
	if (fit_pointer[1] != 0 || fit_pointer[0] < FIT_TABLE_LOWEST_ADDRESS)
		return NULL;

	table = rom_buffer_pointer(image,
	                           ptr_to_offset(image->buffer.size, *fit_pointer));

	/* Check that the address field has the proper signature. */
	if (strncmp((const char *)&table->header.address, FIT_HEADER_ADDRESS,
	            sizeof(table->header.address)))
		return NULL;

	if (table->header.version != FIT_HEADER_VERSION)
		return NULL;

	if (fit_entry_type(&table->header) != FIT_TYPE_HEADER)
		return NULL;

	/* Assume that the FIT table only contains the header */
	if (fit_entry_size_bytes(&table->header) != sizeof(struct fit_entry))
		return NULL;

	return table;
}

static void update_fit_checksum(struct fit_table *fit)
{
	int size_bytes;
	uint8_t *buffer;
	uint8_t result;
	int i;

	fit->header.checksum = 0;
	size_bytes = fit_entry_size_bytes(&fit->header);
	result = 0;
	buffer = (void *)fit;
	for (i = 0; i < size_bytes; i++)
		result += buffer[i];
	fit->header.checksum = -result;
}

static void add_microcodde_entries(struct cbfs_image *image,
                                   struct fit_table *fit,
                                   struct microcode_entry *mcus, int num_mcus)
{
	int i;

	for (i = 0; i < num_mcus; i++) {
		struct fit_entry *entry = &fit->entries[i];
		struct microcode_entry *mcu = &mcus[i];

		entry->address = offset_to_ptr(image->buffer.size, mcu->offset);
		fit_entry_update_size(entry, mcu->size);
		entry->version = FIT_MICROCODE_VERSION;
		entry->type_checksum_valid = FIT_TYPE_MICROCODE;
		entry->checksum = 0;
		fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
	}
}

static int parse_microcode_blob(struct cbfs_image *image,
                                struct cbfs_file *mcode_file,
                                struct microcode_entry *mcus, int *total_mcus)
{
	int num_mcus;
	int current_offset;
	int file_length;

	current_offset = (int)((char *)mcode_file - image->buffer.data);
	current_offset += ntohl(mcode_file->offset);
	file_length = ntohl(mcode_file->len);

	num_mcus = 0;
	while (file_length > sizeof(struct microcode_header))
	{
		struct microcode_header *mcu_header;

		mcu_header = rom_buffer_pointer(image, current_offset);

		/* FIXME: Should the checksum be validated? */
		mcus[num_mcus].offset = current_offset;
		mcus[num_mcus].size = mcu_header->total_size;

		/* Proceed to next payload. */
		current_offset += mcus[num_mcus].size;
		num_mcus++;
		file_length -= mcus[num_mcus].size;

		/* Can't determine any more entries. */
		if (!mcu_header->total_size)
			break;

		/* Reached limit of FIT entries. */
		if (num_mcus == *total_mcus)
			break;
		if (file_length < sizeof(struct microcode_header))
			break;
	}

	/* Update how many microcode updates we found. */
	*total_mcus = num_mcus;

	return 0;
}

int fit_update_table(struct cbfs_image *image, int empty_entries,
                     const char *microcode_blob_name)
{
	struct fit_table *fit;
	struct cbfs_file *mcode_file;
	struct microcode_entry *mcus;
	int ret = 0;
	// struct rom_image image = { .rom = rom, .size = romsize, };

	fit = locate_fit_table(image);

	if (!fit) {
		ERROR("FIT not found.\n");
		return 1;
	}

	mcode_file = cbfs_get_entry(image, microcode_blob_name);
	if (!mcode_file) {
		ERROR("File '%s' not found in CBFS.\n",
		        microcode_blob_name);
		return 1;
	}

	mcus = malloc(sizeof(*mcus) * empty_entries);

	if (!mcus) {
		ERROR("Couldn't allocate memory for microcode update entries.\n");
		return 1;
	}

	if (parse_microcode_blob(image, mcode_file, mcus, &empty_entries)) {
		ERROR("Couldn't parse microcode blob.\n");
		ret = 1;
		goto out;
	}

	add_microcodde_entries(image, fit, mcus, empty_entries);
	update_fit_checksum(fit);

out:
	free(mcus);
	return ret;
}
