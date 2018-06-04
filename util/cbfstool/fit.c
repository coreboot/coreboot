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
#include <compiler.h>

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
} __packed;

struct fit_table {
	struct fit_entry header;
	struct fit_entry entries[];
} __packed;

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
} __packed;

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

static int fit_table_verified(struct fit_table *table)
{
	/* Check that the address field has the proper signature. */
	if (strncmp((const char *)&table->header.address, FIT_HEADER_ADDRESS,
			sizeof(table->header.address)))
		return 0;

	if (table->header.version != FIT_HEADER_VERSION)
		return 0;

	if (fit_entry_type(&table->header) != FIT_TYPE_HEADER)
		return 0;

	/* Assume that the FIT table only contains the header */
	if (fit_entry_size_bytes(&table->header) != sizeof(struct fit_entry))
		return 0;

	return 1;
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
	if (!fit_table_verified(table))
		return NULL;
	else
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

static void update_fit_ucode_entry(struct fit_table *fit,
				struct fit_entry *entry, uint64_t mcu_addr)
{
	entry->address = mcu_addr;
	/*
	 * While loading MCU, its size is not referred from FIT and
	 * rather from the MCU header, hence we can assign zero here
	 */
	entry->size_reserved = 0x0000;
	/* Checksum valid should be cleared for MCU */
	entry->type_checksum_valid = 0;
	entry->version = FIT_MICROCODE_VERSION;
	entry->checksum = 0;
	fit_entry_add_size(&fit->header, sizeof(struct fit_entry));
}

static void add_microcodde_entries(struct fit_table *fit,
				   const struct cbfs_image *image,
				   int num_mcus, struct microcode_entry *mcus,
				   fit_offset_converter_t offset_helper,
				   uint32_t first_mcu_addr)
{
	int i = 0;
	/*
	 * Check if an entry has to be forced into the FIT at index 0.
	 * first_mcu_addr is an address (in ROM) that will point to a
	 * microcode patch.
	 */
	if (first_mcu_addr) {
		struct fit_entry *entry = &fit->entries[0];
		update_fit_ucode_entry(fit, entry, first_mcu_addr);
		i = 1;
	}

	struct microcode_entry *mcu = &mcus[0];
	for (; i < num_mcus; i++) {
		struct fit_entry *entry = &fit->entries[i];
		update_fit_ucode_entry(fit, entry, offset_to_ptr(offset_helper,
						&image->buffer, mcu->offset));
		mcu++;
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
				struct microcode_entry *mcus,
				int total_entries, int *mcus_found)
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

		/* Newer microcode updates include a size field, whereas older
		 * containers set it at 0 and are exactly 2048 bytes long */
		uint32_t total_size = mcu_header->total_size
			? mcu_header->total_size : 2048;

		/* Quickly sanity check a prospective microcode update. */
		if (total_size < sizeof(*mcu_header))
			break;

		/* FIXME: Should the checksum be validated? */
		mcus[num_mcus].offset = current_offset;
		mcus[num_mcus].size = total_size;

		/* Proceed to next payload. */
		current_offset += mcus[num_mcus].size;
		file_length -= mcus[num_mcus].size;
		num_mcus++;

		/* Reached limit of FIT entries. */
		if (num_mcus == total_entries)
			break;
		if (file_length < sizeof(struct microcode_header))
			break;
	}

	/* Update how many microcode updates we found. */
	*mcus_found = num_mcus;

	return 0;
}

int fit_update_table(struct buffer *bootblock, struct cbfs_image *image,
		     const char *microcode_blob_name, int empty_entries,
		     fit_offset_converter_t offset_fn, uint32_t topswap_size,
			uint32_t first_mcu_addr)
{
	struct fit_table *fit, *fit2;
	struct cbfs_file *mcode_file;
	struct microcode_entry *mcus;
	int mcus_found;

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

	if (parse_microcode_blob(image, mcode_file, mcus, empty_entries,
						&mcus_found)) {
		ERROR("Couldn't parse microcode blob.\n");
		ret = 1;
		goto out;
	}

	add_microcodde_entries(fit, image, mcus_found, mcus, offset_fn, 0);

	update_fit_checksum(fit);

	/* A second fit is exactly topswap size away from the bottom one */
	if (topswap_size) {

		fit2 = (struct fit_table *)((uintptr_t)fit - topswap_size);

		if (!fit_table_verified(fit2)) {
			ERROR("second FIT is invalid\n");
			ret = 1;
			goto out;
		}
		/* Check if we have room for first entry */
		if (first_mcu_addr) {
			if (mcus_found >= empty_entries) {
				ERROR("No room, blob mcus = %d, total entries = %d\n",
					mcus_found, empty_entries);
				ret = 1;
				goto out;
			}
			/* Add 1 for the first entry */
			mcus_found++;
		}
		/* Add entries in the second FIT */
		add_microcodde_entries(fit2, image, mcus_found, mcus,
						offset_fn, first_mcu_addr);
		update_fit_checksum(fit2);
	}
out:
	free(mcus);
	return ret;
}
