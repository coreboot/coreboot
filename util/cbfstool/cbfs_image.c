/*
 * CBFS Image Manipulation
 *
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "cbfs_image.h"

/* The file name align is not defined in CBFS spec -- only a preference by
 * (old) cbfstool. */
#define CBFS_FILENAME_ALIGN	(16)

/* To make CBFS more friendly to ROM, fill -1 (0xFF) instead of zero. */
#define CBFS_CONTENT_DEFAULT_VALUE	(-1)

static uint32_t align_up(uint32_t value, uint32_t align) {
	if (value % align)
		value += align - (value % align);
	return value;
}

/* Type and format */

struct typedesc_t {
	uint32_t type;
	const char *name;
};

static struct typedesc_t types_cbfs_entry[] = {
	{CBFS_COMPONENT_STAGE, "stage"},
	{CBFS_COMPONENT_PAYLOAD, "payload"},
	{CBFS_COMPONENT_OPTIONROM, "optionrom"},
	{CBFS_COMPONENT_BOOTSPLASH, "bootsplash"},
	{CBFS_COMPONENT_RAW, "raw"},
	{CBFS_COMPONENT_VSA, "vsa"},
	{CBFS_COMPONENT_MBI, "mbi"},
	{CBFS_COMPONENT_MICROCODE, "microcode"},
	{CBFS_COMPONENT_CMOS_DEFAULT, "cmos_default"},
	{CBFS_COMPONENT_CMOS_LAYOUT, "cmos_layout"},
	{CBFS_COMPONENT_DELETED, "deleted"},
	{CBFS_COMPONENT_NULL, "null"},
	{0, NULL},
};

static struct typedesc_t types_cbfs_compression[] = {
	{CBFS_COMPRESS_NONE, "none"},
	{CBFS_COMPRESS_LZMA, "LZMA"},
	{0, NULL},
};

uint32_t lookup_type_by_name(struct typedesc_t *desc, const char *name,
			     uint32_t default_value) {
	int i;
	for (i = 0; desc[i].name; i++)
		if (strcmp(desc[i].name, name) == 0)
			return desc[i].type;
	return default_value;
}

const char *lookup_name_by_type(struct typedesc_t *desc, uint32_t type,
				const char *default_value) {
	int i;
	for (i = 0; desc[i].name; i++)
		if (desc[i].type == type)
			return desc[i].name;
	return default_value;
}

uint32_t get_cbfs_entry_type(const char *name, uint32_t default_value) {
	return lookup_type_by_name(types_cbfs_entry, name, default_value);
}

const char *get_cbfs_entry_type_name(uint32_t type) {
	return lookup_name_by_type(types_cbfs_entry, type, "(unknown)");
}

uint32_t get_cbfs_compression(const char *name, uint32_t unknown) {
	return lookup_type_by_name(types_cbfs_compression, name, unknown);
}

/* CBFS image */

static int cbfs_calculate_file_header_size(const char *name) {
	return (sizeof(struct cbfs_file) +
		align_up(strlen(name) + 1, CBFS_FILENAME_ALIGN));
}

int cbfs_image_from_file(struct cbfs_image *image, const char *filename) {
	if (buffer_from_file(&image->buffer, filename) != 0)
		return -1;
	DEBUG("read_cbfs_image: %s (%zd bytes)\n", image->buffer.name,
	      image->buffer.size);
	image->header = cbfs_find_header(image->buffer.data,
					 image->buffer.size);
	if (!image->header) {
		ERROR("%s does not have CBFS master header.\n", filename);
		cbfs_image_delete(image);
		return -1;
	}

	return 0;
}

int cbfs_image_write_file(struct cbfs_image *image, const char *filename) {
	assert(image && image->buffer.data);
	return buffer_write_file(&image->buffer, filename);
}

int cbfs_image_delete(struct cbfs_image *image) {
	buffer_delete(&image->buffer);
	image->header = NULL;
	return 0;
}

struct cbfs_file *cbfs_get_entry(struct cbfs_image *image, const char *name) {
	struct cbfs_file *entry;
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		if (strcasecmp(CBFS_NAME(entry), name) == 0) {
			DEBUG("cbfs_get_entry: found %s\n", name);
			return entry;
		}
	}
	return NULL;
}

int cbfs_export_entry(struct cbfs_image *image, const char *entry_name,
		      const char *filename) {
	struct cbfs_file *entry = cbfs_get_entry(image, entry_name);
	struct buffer buffer;
	if (!entry) {
		ERROR("File not found: %s\n", entry_name);
		return -1;
	}
	LOG("Found file %.30s at 0x%x, type %.12s, size %d\n",
	    entry_name, cbfs_get_entry_addr(image, entry),
	    get_cbfs_entry_type_name(ntohl(entry->type)), ntohl(entry->len));

	if (ntohl(entry->type) != CBFS_COMPONENT_RAW) {
		WARN("Only 'raw' files are safe to extract.\n");
	}

	buffer.data = CBFS_SUBHEADER(entry);
	buffer.size = ntohl(entry->len);
	buffer.name = "(cbfs_export_entry)";
	if (buffer_write_file(&buffer, filename) != 0) {
		ERROR("Failed to write %s into %s.\n",
		      entry_name, filename);
		return -1;
	}
	INFO("Successfully dumped the file to: %s\n", filename);
	return 0;
}

int cbfs_remove_entry(struct cbfs_image *image, const char *name) {
	struct cbfs_file *entry, *next;
	size_t len;
	entry = cbfs_get_entry(image, name);
	if (!entry) {
		ERROR("CBFS file %s not found.\n", name);
		return -1;
	}
	next = cbfs_find_next_entry(image, entry);
	assert(next);
	DEBUG("cbfs_remove_entry: Removed %s @ 0x%x\n",
	      CBFS_NAME(entry), cbfs_get_entry_addr(image, entry));
	entry->type = htonl(CBFS_COMPONENT_DELETED);
	len = (cbfs_get_entry_addr(image, next) -
	       cbfs_get_entry_addr(image, entry));
	entry->offset = htonl(cbfs_calculate_file_header_size(""));
	entry->len = htonl(len - ntohl(entry->offset));
	memset(CBFS_NAME(entry), 0, ntohl(entry->offset) - sizeof(*entry));
	memset(CBFS_SUBHEADER(entry), CBFS_CONTENT_DEFAULT_VALUE,
	       ntohl(entry->len));
	return 0;
}

int cbfs_print_header_info(struct cbfs_image *image) {
	assert(image && image->header);
	printf("%s: %zd kB, bootblocksize %d, romsize %d, offset 0x%x\n"
	       "alignment: %d bytes\n\n",
	       simple_basename(image->buffer.name),
	       image->buffer.size / 1024,
	       ntohl(image->header->bootblocksize),
	       ntohl(image->header->romsize),
	       ntohl(image->header->offset),
	       ntohl(image->header->align));
	return 0;
}

static int cbfs_print_stage_info(struct cbfs_stage *stage, FILE* fp) {
	fprintf(fp,
		"    %s compression, entry: 0x%" PRIx64 ", load: 0x%" PRIx64 ", "
		"length: %d/%d\n",
		lookup_name_by_type(types_cbfs_compression,
				    stage->compression, "(unknown)"),
		stage->entry,
		stage->load,
		stage->len,
		stage->memlen);
	return 0;
}

static int cbfs_print_payload_segment_info(struct cbfs_payload_segment *payload,
					   FILE *fp)
{
	switch(payload->type) {
		case PAYLOAD_SEGMENT_CODE:
		case PAYLOAD_SEGMENT_DATA:
			fprintf(fp, "    %s (%s compression, offset: 0x%x, "
				"load: 0x%" PRIx64 ", length: %d/%d)\n",
				(payload->type == PAYLOAD_SEGMENT_CODE ?
				 "code " : "data"),
				lookup_name_by_type(types_cbfs_compression,
						    ntohl(payload->compression),
						    "(unknown)"),
				ntohl(payload->offset),
				ntohll(payload->load_addr),
				ntohl(payload->len), ntohl(payload->mem_len));
			break;

		case PAYLOAD_SEGMENT_ENTRY:
			fprintf(fp, "    entry (0x%" PRIx64 ")\n",
				ntohll(payload->load_addr));
			break;

		case PAYLOAD_SEGMENT_BSS:
			fprintf(fp, "    BSS (address 0x%016" PRIx64 ", "
				"length 0x%x)\n",
				ntohll(payload->load_addr),
				ntohl(payload->len));
			break;

		case PAYLOAD_SEGMENT_PARAMS:
			fprintf(fp, "    parameters\n");
			break;

		default:
			fprintf(fp, "   0x%x (%s compression, offset: 0x%x, "
				"load: 0x%" PRIx64 ", length: %d/%d\n",
				payload->type,
				lookup_name_by_type(types_cbfs_compression,
						    payload->compression,
						    "(unknown)"),
				ntohl(payload->offset),
				ntohll(payload->load_addr),
				ntohl(payload->len),
				ntohl(payload->mem_len));
			break;
	}
	return 0;
}

int cbfs_print_entry_info(struct cbfs_image *image, struct cbfs_file *entry,
			  void *arg) {
	const char *name = CBFS_NAME(entry);
	struct cbfs_payload_segment *payload;
	FILE *fp = (FILE *)arg;

	if (!cbfs_is_valid_entry(entry)) {
		ERROR("cbfs_print_entry_info: Invalid entry at 0x%x\n",
		      cbfs_get_entry_addr(image, entry));
		return -1;
	}
	if (!fp)
		fp = stdout;

	fprintf(fp, "%-30s 0x%-8x %-12s %d\n",
		*name ? name : "(empty)",
		cbfs_get_entry_addr(image, entry),
		get_cbfs_entry_type_name(ntohl(entry->type)),
		ntohl(entry->len));

	if (!verbose)
		return 0;

	DEBUG(" cbfs_file=0x%x, offset=0x%x, content_address=0x%x+0x%x\n",
	      cbfs_get_entry_addr(image, entry), ntohl(entry->offset),
	      cbfs_get_entry_addr(image, entry) + ntohl(entry->offset),
	      ntohl(entry->len));

	/* note the components of the subheader may be in host order ... */
	switch (ntohl(entry->type)) {
		case CBFS_COMPONENT_STAGE:
			cbfs_print_stage_info((struct cbfs_stage *)
					      CBFS_SUBHEADER(entry), fp);
			break;

		case CBFS_COMPONENT_PAYLOAD:
			payload  = (struct cbfs_payload_segment *)
					CBFS_SUBHEADER(entry);
			while (payload) {
				cbfs_print_payload_segment_info(payload, fp);
				if (payload->type == PAYLOAD_SEGMENT_ENTRY)
					break;
				else
					payload ++;
			}
			break;
		default:
			break;
	}
	return 0;
}

int cbfs_print_directory(struct cbfs_image *image) {
	cbfs_print_header_info(image);
	printf("%-30s %-10s %-12s Size\n", "Name", "Offset", "Type");
	cbfs_walk(image, cbfs_print_entry_info, NULL);
	return 0;
}

int cbfs_walk(struct cbfs_image *image, cbfs_entry_callback callback,
	      void *arg) {
	int count = 0;
	struct cbfs_file *entry;
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		count ++;
		if (callback(image, entry, arg) != 0)
			break;
	}
	return count;
}

struct cbfs_header *cbfs_find_header(char *data, size_t size) {
	size_t offset;
	int found = 0;
	uint32_t x86sig;
	struct cbfs_header *header, *result = NULL;

	// Try x86 style (check signature in bottom) header first.
	x86sig = *(uint32_t *)(data + size - sizeof(uint32_t));
	offset = (x86sig + (uint32_t)size);
	DEBUG("x86sig: 0x%x, offset: 0x%zx\n", x86sig, offset);
	if (offset >= size - sizeof(*header) ||
	    ntohl(((struct cbfs_header *)(data + offset))->magic) !=
	    CBFS_HEADER_MAGIC)
		offset = 0;

	for (; offset + sizeof(*header) < size; offset++) {
		header = (struct cbfs_header *)(data + offset);
		if (ntohl(header->magic) !=(CBFS_HEADER_MAGIC))
		    continue;
		if (ntohl(header->version) != CBFS_HEADER_VERSION1 &&
		    ntohl(header->version) != CBFS_HEADER_VERSION2) {
			// Probably not a real CBFS header?
			continue;
		}
		found++;
		result = header;
	}
	if (found > 1) {
		ERROR("multiple (%d) CBFS headers found!\n",
		       found);
		result = NULL;
	}
	return result;
}


struct cbfs_file *cbfs_find_first_entry(struct cbfs_image *image) {
	assert(image && image->header);
	return (struct cbfs_file *)(image->buffer.data +
				   ntohl(image->header->offset));
}

struct cbfs_file *cbfs_find_next_entry(struct cbfs_image *image,
				       struct cbfs_file *entry) {
	uint32_t addr = cbfs_get_entry_addr(image, entry);
	int align = ntohl(image->header->align);
	assert(entry && cbfs_is_valid_entry(entry));
	addr += ntohl(entry->offset) + ntohl(entry->len);
	addr = align_up(addr, align);
	return (struct cbfs_file *)(image->buffer.data + addr);
}

uint32_t cbfs_get_entry_addr(struct cbfs_image *image, struct cbfs_file *entry) {
	assert(image && image->buffer.data && entry);
	return (int32_t)((char *)entry - image->buffer.data);
}

int cbfs_is_valid_entry(struct cbfs_file *entry) {
	return (entry &&memcmp(entry->magic, CBFS_FILE_MAGIC,
			       sizeof(entry->magic)) == 0);
}

