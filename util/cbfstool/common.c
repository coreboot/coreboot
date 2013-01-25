/*
 * common utility functions for cbfstool
 *
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 * Copyright (C) 2012 Google, Inc.
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

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "elf.h"

/* Utilities */

/* Small, OS/libc independent runtime check for endianess */
int is_big_endian(void)
{
	static const uint32_t inttest = 0x12345678;
	uint8_t inttest_lsb = *(uint8_t *)&inttest;
	if (inttest_lsb == 0x12) {
		return 1;
	}
	return 0;
}

uint32_t align_up(uint32_t value, uint32_t align) {
	if (value % align)
		value += align - (value % align);
	return value;
}

/* Buffer and file I/O */

int buffer_create(struct buffer *buffer, size_t size, const char *name) {
	buffer->name = strdup(name);
	buffer->size = size;
	buffer->data = (char*)malloc(buffer->size);
	return (buffer->data == NULL);
}

int buffer_from_file(struct buffer *buffer, const char *filename) {
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		perror(filename);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	buffer->size = ftell(fp);
	buffer->name = strdup(filename);
	rewind(fp);
	buffer->data = (char*)malloc(buffer->size);
	assert(buffer->data);
	if (fread(buffer->data, 1, buffer->size, fp) != buffer->size) {
		ERROR("incomplete read: %s\n", filename);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int buffer_write_file(struct buffer *buffer, const char *filename) {
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		perror(filename);
		return -1;
	}
	assert(buffer && buffer->data);
	if (fwrite(buffer->data, 1, buffer->size, fp) != buffer->size) {
		ERROR("incomplete write: %s\n", filename);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int buffer_delete(struct buffer *buffer) {
	assert(buffer);
	if (buffer->name) {
		free(buffer->name);
		buffer->name = NULL;
	}
	if (buffer->data) {
		free(buffer->data);
		buffer->data = NULL;
	}
	buffer->size = 0;
	return 0;
}

/* Type and format */

int is_elf_object(const void *input) {
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)input;
	return !memcmp(ehdr->e_ident, ELFMAG, 4);
}

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

void print_all_cbfs_entry_types(void) {
	size_t i, total = ARRAY_SIZE(types_cbfs_entry);

	for (i = 0; i < total; i++) {
		printf(" %s%c", types_cbfs_entry[i].name,
		       (i + 1 == total) ? '\n' : ',');
		if ((i + 1) % 8 == 0)
			printf("\n");
	}
}

/* CBFS image processing */

/* The file name align is not defined in CBFS spec -- only a preference by
 * (old) cbfstool. */
#define CBFS_FILENAME_ALIGN	(16)

/* To make CBFS more friendly to ROM, fill -1 (0xFF) instead of zero. */
#define CBFS_CONTENT_DEFAULT_VALUE	(-1)

int cbfs_image_create(struct cbfs_image *image,
		      size_t size,
		      uint32_t align,
		      struct buffer *bootblock,
		      int32_t bootblock_offset,
		      int32_t header_offset,
		      int32_t entries_offset)
{
	struct cbfs_header *header;
	struct cbfs_file *entry;
	uint32_t cbfs_len;

	DEBUG("cbfs_create_image: bootblock=0x%x+0x%zx, "
	      "header=0x%x+0x%zx, entries_offset=0x%x\n",
	      bootblock_offset, bootblock->size,
	      header_offset, sizeof(*header), entries_offset);

	if (buffer_create(&image->buffer, size, "(new)") != 0) {
		ERROR("Cannot allocate memory for %zd bytes.\n", size);
		return -1;
	}
	image->header = NULL;
	memset(image->buffer.data, CBFS_CONTENT_DEFAULT_VALUE, size);

	// Adjust legcay top-aligned address to ROM offset.
	if (IS_TOP_ALIGNED_ADDRESS(entries_offset))
		entries_offset += (int32_t)size;
	if (IS_TOP_ALIGNED_ADDRESS(bootblock_offset))
		bootblock_offset += (int32_t)size;
	if (IS_TOP_ALIGNED_ADDRESS(header_offset))
		header_offset += (int32_t) size;

	DEBUG("cbfs_create_image: (real offset) bootblock=0x%x, "
	      "header=0x%x, entries_offset=0x%x\n",
	      bootblock_offset, header_offset, entries_offset);

	if (align == 0)
		align = 64;  // default align size.

	// Prepare bootblock
	if (bootblock_offset + bootblock->size > size) {
		ERROR("Bootblock (0x%x+0x%zx) exceed ROM size (0x%zx)\n",
		      bootblock_offset, bootblock->size, size);
		return -1;
	}
	memcpy(image->buffer.data + bootblock_offset, bootblock->data,
	       bootblock->size);

	// Prepare header
	if (header_offset + sizeof(*header) > size) {
		ERROR("Header (0x%x+0x%zx) exceed ROM size (0x%zx)\n",
		      header_offset, sizeof(*header), size);
		return -1;
	}
	header = (struct cbfs_header*)(image->buffer.data + header_offset);
	image->header = header;
	header->magic = htonl(CBFS_HEADER_MAGIC);
	header->version = htonl(CBFS_HEADER_VERSION);
	header->romsize = htonl(size);
	header->bootblocksize = htonl(bootblock->size);
	header->align = htonl(align);
	header->offset = htonl(entries_offset);

	// Prepare entries
	if (align_up(entries_offset, align) != entries_offset) {
		ERROR("Offset (0x%x) must be aligned to 0x%x.\n",
		      entries_offset, align);
		return -1;
	}
	if (entries_offset + sizeof(*entry) > size) {
		ERROR("Offset (0x%x+0x%zx) exceed ROM size(0x%zx)\n",
		      entries_offset, sizeof(*entry), size);
		return -1;
	}
	entry = (struct cbfs_file*)(image->buffer.data + entries_offset);
	// To calculate available length, find
	//   e = min(bootblock, header, size) where e > entries_offset.
	cbfs_len = size;
	if (bootblock_offset > entries_offset && bootblock_offset < cbfs_len)
		cbfs_len = bootblock_offset;
	if (header_offset > entries_offset && header_offset < cbfs_len)
		cbfs_len = header_offset;
	cbfs_len -= entries_offset + align;
	cbfs_create_entry(image, entry, cbfs_len, "");
	LOG("Created CBFS image (capacity = %d bytes)\n", cbfs_len);
	return 0;
}

static int cbfs_fix_legacy_size(struct cbfs_image *image) {
	// A bug in old CBFStool may produce extra few bytes (by alignment) and
	// cause cbfstool to overwrite things after free space -- which is
	// usually CBFS header on x86. We need to workaround that.

	struct cbfs_file *entry, *first = NULL, *last = NULL;
	for (first = entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		last = entry;
	}
	if ((char*)first < (char*)image->header &&
	    (char*)entry > (char*)image->header) {
		WARN("CBFS image was created with old cbfstool with size bug. "
		     "Fixing size in last entry...\n");
		last->len = htonl(ntohl(last->len) -
				  ntohl(image->header->align));
		DEBUG("Last entry has been changed from 0x%x to 0x%x.\n",
		      cbfs_get_entry_addr(image, entry),
		      cbfs_get_entry_addr(image,
					  cbfs_find_next_entry(image, last)));
	}
	return 0;
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

	cbfs_fix_legacy_size(image);
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

/* Splits an empty entry at given offset. */
int cbfs_split_empty_entry(struct cbfs_image *image,
			   struct cbfs_file *entry,
			   uint32_t offset) {
	struct cbfs_file *next = cbfs_find_next_entry(image, entry);
	uint32_t addr = cbfs_get_entry_addr(image, entry),
		 addr_next = cbfs_get_entry_addr(image, next);
	size_t min_entry_size = sizeof(*entry) + CBFS_FILENAME_ALIGN;
	size_t len;

	assert(addr < offset && addr_next <= offset);
	if (offset - addr < min_entry_size) {
		ERROR("No space between previous entry 0x%x and offset 0x%x.\n",
		      addr, offset);
		return -1;
	}
	if (addr_next - offset < min_entry_size) {
		ERROR("No space between offset 0x%x and entry 0x%x\n",
		      offset, addr_next);
		return -1;
	}
	cbfs_create_entry(image, entry, 0, "");
	len = offset - addr - ntohl(entry->offset);
	entry->len = htonl(len);
	if (verbose > 1)
		cbfs_print_entry_info(image, entry, stderr);

	entry = cbfs_find_next_entry(image, entry);
	addr = cbfs_get_entry_addr(image, entry);
	cbfs_create_entry(image, entry, 0, "");
	len = addr_next - offset - ntohl(entry->offset);
	entry->len = htonl(len);
	if (verbose > 1)
		cbfs_print_entry_info(image, entry, stderr);
	assert(cbfs_get_entry_addr(image, cbfs_find_next_entry(image, entry)) ==
	       addr_next);
	return 0;
}

int cbfs_add_entry(struct cbfs_image *image, struct buffer *buffer,
		   const char *name, uint32_t type, uint32_t offset) {
	uint32_t entry_type, entry_capacity;
	uint32_t addr;
	struct cbfs_file *entry, *next;
	uint32_t need_size, new_size;

	need_size = (sizeof(*entry) + buffer->size +
		     align_up(strlen(name) + 1, CBFS_FILENAME_ALIGN));
	DEBUG("cbfs_add_entry('%s', %zd) => need_size = %u\n",
	      name, buffer->size, need_size);

	// Check if offset is valid.
	if (offset) {
		uint32_t align = ntohl(image->header->align),
			 romsize = ntohl(image->header->romsize);
		// legacy cbfstool takes top-aligned address.
		if (IS_TOP_ALIGNED_ADDRESS(offset)) {
			INFO("Converting top-aligned address 0x%x to offset: "
			     " 0x%x\n", offset, offset - romsize);
			offset += romsize;
		}
		if (type == CBFS_COMPONENT_STAGE && offset % align) {
			INFO("Got address from locate-stage.\n");
			offset -= sizeof(*entry) + sizeof(struct cbfs_stage) +
					align_up(strlen(name) + 1,
						 CBFS_FILENAME_ALIGN);
		}
		if (offset % align != 0) {
			ERROR("Target address (0x%x) is not aligned to CBFS "
			      "image align (0x%x). It's impossible to add entry "
			      "in that location.\n", offset, align);
			return -1;
		}
	}

	// Merge empty entries.
	DEBUG("(trying to merge empty entries...)\n");
	cbfs_walk(image, cbfs_merge_empty_entry, NULL);

	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(entry);
	     entry = cbfs_find_next_entry(image, entry)) {

		entry_type = ntohl(entry->type);
		if (entry_type != CBFS_COMPONENT_NULL)
			continue;

		addr = cbfs_get_entry_addr(image, entry);
		if (offset && addr > offset) {
			ERROR("Not available to add component in 0x%x.\n",
			      offset);
			return -1;
		}

		next = cbfs_find_next_entry(image, entry);
		entry_capacity = cbfs_get_entry_addr(image, next) - addr;
		if (offset && (addr + entry_capacity) <= offset) {
			DEBUG("Skip entry 0x%x before specified offset 0x%x.\n",
			      addr, offset);
			continue;
		}

		DEBUG("cbfs_add_entry: space %s at 0x%x, %d bytes\n",
		      get_cbfs_entry_type_name(entry_type), addr,
		      entry_capacity);
		if (entry_capacity < need_size)
			continue;

		// Should we create an empty partition before target offset?
		if (offset && addr != offset) {
			DEBUG("Split empty entry at 0x%x\n", addr);
			if (cbfs_split_empty_entry(image, entry, offset) != 0) {
				ERROR("Can't split empty entry for 0x%x\n",
				      offset);
				return -1;
			}
			assert(cbfs_get_entry_addr(
					image,
					cbfs_find_next_entry(image, entry)) ==
			       addr);
			continue;
		}

		DEBUG("Filling new entry data (%zd bytes).\n", buffer->size);
		cbfs_create_entry(image, entry, buffer->size, name);
		entry->type = htonl(type);
		memcpy(CBFS_SUBHEADER(entry), buffer->data, buffer->size);
		if (verbose)
			cbfs_print_entry_info(image, entry, stderr);

		// setup new entry
		DEBUG("Seting new empty entry.\n");
		entry = cbfs_find_next_entry(image, entry);
		new_size = (cbfs_get_entry_addr(image, next) -
			    cbfs_get_entry_addr(image, entry));
		new_size -= sizeof(*entry) + CBFS_FILENAME_ALIGN;
		DEBUG("new size: %d\n", new_size);
		cbfs_create_entry(image, entry, new_size, "");
		if (verbose)
			cbfs_print_entry_info(image, entry, stderr);
		return 0;
	}

	ERROR("Could not add [%s, %zd bytes (%zd KB)]; probably too big?\n",
	      buffer->name, buffer->size, buffer->size / 1024);
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
	entry->offset = htonl(sizeof(*entry) + CBFS_FILENAME_ALIGN);
	entry->len = htonl(len - ntohl(entry->offset));
	memset(CBFS_NAME(entry), 0, CBFS_FILENAME_ALIGN);
	memset(CBFS_SUBHEADER(entry), CBFS_CONTENT_DEFAULT_VALUE,
	       ntohl(entry->len));
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

/* basename(3) may modify buffer, so we want a tiny alternative. */
static const char *simple_basename(const char *name) {
	const char *slash = strrchr(name, '/');
	if (slash)
		return slash + 1;
	else
		return name;
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
						    payload->compression,
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
	FILE *fp = (FILE*)arg;

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

	/* note the components of the subheader may be in host order ... */
	switch (ntohl(entry->type)) {
		case CBFS_COMPONENT_STAGE:
			cbfs_print_stage_info((struct cbfs_stage*)
					      CBFS_SUBHEADER(entry), fp);
			break;

		case CBFS_COMPONENT_PAYLOAD:
			payload  = (struct cbfs_payload_segment*)
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

int cbfs_merge_empty_entry(struct cbfs_image *image, struct cbfs_file *entry,
			   void *arg) {
	struct cbfs_file *next, *last;
	uint32_t type;
	size_t newlen;

	type = ntohl(entry->type);
	if (type == CBFS_COMPONENT_DELETED) {
		// Ready to be recycled.
		type = CBFS_COMPONENT_NULL;
		entry->type = htonl(type);
	}
	if (type != CBFS_COMPONENT_NULL)
		return 0;

	next = cbfs_find_next_entry(image, entry);
	if (!next || !cbfs_is_valid_entry(next))
		return -1;
	type = ntohl(next->type);
	if (type == CBFS_COMPONENT_DELETED) {
		type = CBFS_COMPONENT_NULL;
		next->type = htonl(type);
	}
	if (type != CBFS_COMPONENT_NULL)
		return 0;

	last = cbfs_find_next_entry(image, next);
	assert(last);

	// Now, we find two deleted/empty entries; try to merge now.
	DEBUG("cbfs_join_empty_entry: combine 0x%x+0x%x and 0x%x+0x%x.\n",
	      cbfs_get_entry_addr(image, entry), ntohl(entry->len),
	      cbfs_get_entry_addr(image, next), ntohl(next->len));
	entry->type = htonl(CBFS_COMPONENT_NULL);
	newlen = (cbfs_get_entry_addr(image, last) -
		  cbfs_get_entry_addr(image, entry) - ntohl(entry->offset));
	entry->len = htonl(newlen);
	DEBUG("new empty entry: length=0x%zx\n", newlen);
	memset(CBFS_SUBHEADER(entry), CBFS_CONTENT_DEFAULT_VALUE, newlen);
	// TODO we should try again until no more entries are found...
	// cbfs_merge_empty_entry(image, entry, arg);
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
	x86sig = *(uint32_t*)(data + size - sizeof(uint32_t));
	offset = (x86sig + (uint32_t)size);
	DEBUG("x86sig: 0x%x, offset: 0x%zx\n", x86sig, offset);
	if (offset >= size - sizeof(*header) ||
	    ntohl(((struct cbfs_header*)(data + offset))->magic) !=
	    CBFS_HEADER_MAGIC)
		offset = 0;

	for (; offset + sizeof(*header) < size; offset++) {
		header = (struct cbfs_header*)(data + offset);
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
	return (struct cbfs_file*)(image->buffer.data +
				   ntohl(image->header->offset));
}

struct cbfs_file *cbfs_find_next_entry(struct cbfs_image *image,
				       struct cbfs_file *entry) {
	uint32_t addr = cbfs_get_entry_addr(image, entry);
	int align = ntohl(image->header->align);
	assert(entry && cbfs_is_valid_entry(entry));
	addr += ntohl(entry->offset) + ntohl(entry->len);
	addr = align_up(addr, align);
	return (struct cbfs_file*)(image->buffer.data + addr);
}

uint32_t cbfs_get_entry_addr(struct cbfs_image *image, struct cbfs_file *entry) {
	assert(image && image->buffer.data && entry);
	return (int32_t)((char*)entry - image->buffer.data);
}

int cbfs_is_valid_entry(struct cbfs_file *entry) {
	return (entry &&memcmp(entry->magic, CBFS_FILE_MAGIC,
			       sizeof(entry->magic)) == 0);
}

int cbfs_init_entry(struct cbfs_file *entry,
		    struct buffer *buffer) {
	memset(entry, 0, sizeof(*entry));
	memcpy(entry->magic, CBFS_FILE_MAGIC, sizeof(entry->magic));
	entry->len = htonl(buffer->size);
	entry->offset = htonl(sizeof(*entry) + strlen(buffer->name) + 1);
	return 0;
}

int cbfs_create_entry(struct cbfs_image *image, struct cbfs_file *entry,
		      size_t len, const char *name) {
	memset(entry, CBFS_CONTENT_DEFAULT_VALUE, sizeof(*entry));
	memcpy(entry->magic, CBFS_FILE_MAGIC, sizeof(entry->magic));
	entry->type = htonl(CBFS_COMPONENT_NULL);
	entry->len = htonl(len);
	entry->checksum = 0;  // TODO Build a checksum algorithm.
	entry->offset = htonl(sizeof(*entry) + align_up(strlen(name) + 1,
							CBFS_FILENAME_ALIGN));
	memset(CBFS_NAME(entry), 0, ntohl(entry->offset) - sizeof(*entry));
	strcpy(CBFS_NAME(entry), name);
	memset(CBFS_SUBHEADER(entry), CBFS_CONTENT_DEFAULT_VALUE, len);
	return 0;
}

/* Finds a place to hold whole stage data in same memory page.
 */
static int is_in_same_page(uint32_t start, uint32_t size, uint32_t page) {
	if (!page)
		return 1;
	return (start / page) == (start + size - 1) / page;
}

int32_t cbfs_locate_stage(struct cbfs_image *image, const char *name,
			  uint32_t size, uint32_t page_size) {
	struct cbfs_file *entry;
	size_t need_len;
	uint32_t addr, addr_next, addr2, addr3, header_len;
	assert(size < page_size);

	header_len = sizeof(*entry) + sizeof(struct cbfs_stage) +
			align_up(strlen(name) + 1, CBFS_FILENAME_ALIGN);
	need_len = header_len + size;

	// Merge empty entries to build get max available pages.
	cbfs_walk(image, cbfs_merge_empty_entry, NULL);

	/* Three cases (content fit, content before, or content after page):
	 * case 1.
	 *          |  PAGE 1  |   PAGE 2  |
	 *          |   <header><content>  | Fit. Return start of content.
	 * case 2.
	 *          |  PAGE 1  |   PAGE 2  |
	 *          | <header><content>    | Can't fit. If we can fit in PAGE2,
	 *          |          |           | return PAGE2 (not start of content)
	 * case 3.
	 *          When we move (align) to PAGE 2 and content fills entire
	 *          page, the distance is less than header (so we still can't
	 *          fit). Try PAGE 3.
	 */
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(entry);
	     entry = cbfs_find_next_entry(image, entry)) {

		uint32_t type = ntohl(entry->type);
		if (type != CBFS_COMPONENT_NULL)
			continue;

		addr = cbfs_get_entry_addr(image, entry);
		addr_next = cbfs_get_entry_addr(image, cbfs_find_next_entry(
				image, entry));
		if (addr_next - addr < need_len)
			continue;
		if (is_in_same_page(addr + header_len, size, page_size)) {
			DEBUG("cbfs_locate_stage: FIT (PAGE1).");
			return addr + header_len;
		}

		addr2 = align_up(addr, page_size);
		if (addr2 < addr_next && addr_next - addr2 >= size &&
		    addr2 - addr >= header_len) {
			DEBUG("cbfs_locate_stage: OVERLAP (PAGE2).");
			return addr2;
		}

		addr3 = addr2 + page_size;
		if (addr3 < addr_next && addr_next - addr3 >= size &&
		    addr3 - addr >= header_len) {
			DEBUG("cbfs_locate_stage: OVERLAP+ (PAGE3).");
			return addr3;
		}
	}
	return 0;
}
