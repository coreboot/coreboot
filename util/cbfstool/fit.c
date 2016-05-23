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
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	struct fit_entry entries[];
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

static inline void *rom_buffer_pointer(struct buffer *buffer, int offset)
{
	return &buffer->data[offset];
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
static inline int ptr_to_offset(fit_offset_converter_t helper,
				const struct buffer *region, uint32_t host_ptr)
{
	return helper(region, -host_ptr);
}

/*
 * Get a pointer from an offset. This function assumes the ROM is located
 * in the host address space at [4G - romsize -> 4G). It also assume all
 * pointers have values within this address range.
 */
static inline uint32_t offset_to_ptr(fit_offset_converter_t helper,
				     const struct buffer *region, int offset)
{
	return -helper(region, offset);
}

static struct fit_table *locate_fit_table(fit_offset_converter_t offset_helper,
					  struct buffer *buffer)
{
	struct fit_table *table;
	uint32_t *fit_pointer;

	fit_pointer = rom_buffer_pointer(buffer,
			ptr_to_offset(offset_helper, buffer,
			FIT_POINTER_LOCATION));

	/* Ensure pointer is below 4GiB and within 16MiB of 4GiB */
	if (fit_pointer[1] != 0 || fit_pointer[0] < FIT_TABLE_LOWEST_ADDRESS)
		return NULL;

	table = rom_buffer_pointer(buffer,
			   ptr_to_offset(offset_helper, buffer, *fit_pointer));

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

static void add_microcodde_entries(struct fit_table *fit,
				   const struct cbfs_image *image,
				   int num_mcus, struct microcode_entry *mcus,
				   fit_offset_converter_t offset_helper)
{
	int i;

	for (i = 0; i < num_mcus; i++) {
		struct fit_entry *entry = &fit->entries[i];
		struct microcode_entry *mcu = &mcus[i];

		entry->address = offset_to_ptr(offset_helper, &image->buffer,
								mcu->offset);
		fit_entry_update_size(entry, mcu->size);
		entry->version = FIT_MICROCODE_VERSION;
		entry->type_checksum_valid = FIT_TYPE_MICROCODE;
		entry->checksum = 0;
		fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
	}
}

static void cbfs_file_get_header(struct buffer *buf, struct cbfs_file *file)
{
	bgets(buf, &file->magic, sizeof(file->magic));
	file->len = xdr_be.get32(buf);
	file->type = xdr_be.get32(buf);
	file->attributes_offset = xdr_be.get32(buf);
	file->offset = xdr_be.get32(buf);
}

static int fit_header(void *ptr, uint32_t *current_offset, uint32_t *file_length)
{
	struct buffer buf;
	struct cbfs_file header;
	buf.data = ptr;
	buf.size = sizeof(header);
	cbfs_file_get_header(&buf, &header);
	*current_offset = header.offset;
	*file_length = header.len;
	return 0;
}

static int parse_microcode_blob(struct cbfs_image *image,
                                struct cbfs_file *mcode_file,
                                struct microcode_entry *mcus, int *total_mcus)
{
	int num_mcus;
	uint32_t current_offset;
	uint32_t file_length;

	fit_header(mcode_file, &current_offset, &file_length);
	current_offset += (int)((char *)mcode_file - image->buffer.data);

	num_mcus = 0;
	while (file_length > sizeof(struct microcode_header))
	{
		const struct microcode_header *mcu_header;

		mcu_header = rom_buffer_pointer(&image->buffer, current_offset);

		/* Quickly sanity check a prospective microcode update. */
		if (mcu_header->total_size < sizeof(*mcu_header))
			break;

		/* FIXME: Should the checksum be validated? */
		mcus[num_mcus].offset = current_offset;
		mcus[num_mcus].size = mcu_header->total_size;

		/* Proceed to next payload. */
		current_offset += mcus[num_mcus].size;
		file_length -= mcus[num_mcus].size;
		num_mcus++;

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

int fit_update_table(struct buffer *bootblock, struct cbfs_image *image,
		     const char *microcode_blob_name, int empty_entries,
		     fit_offset_converter_t offset_fn)
{
	struct fit_table *fit;
	struct cbfs_file *mcode_file;
	struct microcode_entry *mcus;
	int ret = 0;
	// struct rom_image image = { .rom = rom, .size = romsize, };

	fit = locate_fit_table(offset_fn, bootblock);

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

	add_microcodde_entries(fit, image, empty_entries, mcus, offset_fn);
	update_fit_checksum(fit);

out:
	free(mcus);
	return ret;
}
