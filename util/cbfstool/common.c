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

#define CBFS_FILENAME_ALIGN	(16)

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

static struct filetypes_t {
	uint32_t type;
	const char *name;
} cbfs_filetypes[] = {
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
	{CBFS_COMPONENT_NULL, "null"}
};

uint32_t get_cbfs_entry_type(const char *name) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(cbfs_filetypes); i++)
		if (strcmp(cbfs_filetypes[i].name, name) == 0)
			return cbfs_filetypes[i].type;
	return -1;
}

const char * get_cbfs_entry_type_name(uint32_t type) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(cbfs_filetypes); i++)
		if (cbfs_filetypes[i].type == type)
			return cbfs_filetypes[i].name;
	return "(unknown)";
}

void print_all_cbfs_entry_types(void) {
	size_t i, total = ARRAY_SIZE(cbfs_filetypes);

	for (i = 0; i < total; i++) {
		printf(" %s%c", cbfs_filetypes[i].name,
		       (i + 1 == total) ? '\n' : ',');
		if ((i + 1) % 8 == 0)
			printf("\n");
	}
}

/* CBFS image processing */
int cbfs_image_create(struct cbfs_image *image, size_t size) {
	if (buffer_create(&image->buffer, size, "(new)") != 0) {
		ERROR("Cannot allocate memory for %zd bytes.\n", size);
		return -1;
	}
	image->header = NULL;
	memset(image->buffer.data, -1, size);
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

static uint32_t align_up(uint32_t value, uint32_t align) {
	if (value % align)
		value += align - (value % align);
	return value;
}

int cbfs_add_entry(struct cbfs_image *image, const char *name,
		   uint32_t type, struct buffer *buffer) {
	uint32_t entry_type, entry_offset, entry_capacity;
	uint32_t addr;
	struct cbfs_file *entry, old_entry;
	uint32_t need_size, new_size;

	need_size = align_up(strlen(name) + 1, CBFS_FILENAME_ALIGN);
	need_size += sizeof(*entry);
	need_size += buffer->size;
	need_size = align_up(need_size, ntohl(image->header->align));

	DEBUG("cbfs_add_entry('%s', %zd) => need_size = %u\n",
	      name, buffer->size, need_size);

	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(entry);
	     entry = cbfs_find_next_entry(image, entry)) {

		entry_type = ntohl(entry->type);
		if (// entry_type != CBFS_COMPONENT_DELETED &&
		    entry_type != CBFS_COMPONENT_NULL)
			continue;

		addr = cbfs_get_entry_addr(image, entry);
		entry_offset = ntohl(entry->offset);
		entry_capacity = entry_offset + ntohl(entry->len);

		DEBUG("cbfs_add_entry: space %s at 0x%x, %d bytes\n",
		      get_cbfs_entry_type_name(entry_type), addr,
		      entry_capacity);

		DEBUG("%d %d\n", entry_capacity, need_size);

		if (entry_capacity < need_size)
			continue;

		DEBUG("start creating new entry.\n");

		// fill entry
		memcpy(&old_entry, entry, sizeof(old_entry));
		entry->len = htonl(buffer->size);
		entry->type = htonl(type);
		entry->offset = htonl(align_up(strlen(name) + 1,
					       CBFS_FILENAME_ALIGN));
		strcpy(CBFS_NAME(entry), name);
		memcpy(CBFS_SUBHEADER(entry), buffer->data, buffer->size);
		cbfs_print_entry_info(image, entry, NULL);

		// setup new entry
		entry = cbfs_find_next_entry(image, entry);
		new_size = ntohl(old_entry.len);
		new_size -= (cbfs_get_entry_addr(image, entry) - addr);
		old_entry.len = htonl(new_size);
		memcpy(entry, &old_entry, sizeof(old_entry));
		*CBFS_NAME(entry) = 0;
		cbfs_print_entry_info(image, entry, NULL);

		return 0;

		// assume NULL is 
		// assume we always have good NULL to add.

		// See if we can update current record.
		//if (buffer->size <= len && 
		 //   (location == 0 || current == location))
	}

#if 0
			/* if this is the right size, and if specified, the right location, use it */
			if ((contentsize <= length)
			    && ((location == 0) || (current == location))) {
				if (contentsize < length) {
					dprintf
					    ("this chunk is %x bytes, we need %x. create a new chunk at %x with %x bytes\n",
					     length, contentsize,
					     ALIGN(current + contentsize,
						   align),
					     length - contentsize);
					uint32_t start =
					    ALIGN(current + contentsize, align);
					uint32_t size =
					    current + ntohl(thisfile->offset)
					    + length - start - 16 -
					    sizeof(struct cbfs_file);
					cbfs_create_empty_file(start, size);
				}
				dprintf("copying data\n");
				memcpy(phys_to_virt(current), content,
				       contentsize);
				return 0;
			}
			if (location != 0) {
				/* CBFS has the constraint that the chain always moves up in memory. so once
				   we're past the place we seek, we don't need to look any further */
				if (current > location) {
					fprintf
					    (stderr, "E: The requested space is not available\n");
					return 1;
				}

				/* Is the requested location inside the current chunk? */
				if ((current < location)
				    && ((location + contentsize) <=
					(current + length))) {
					/* Split it up. In the next iteration the code will be at the right place. */
					dprintf("split up. new length: %x\n",
						location - current -
						ntohl(thisfile->offset));
					thisfile->len =
					    htonl(location - current -
						  ntohl(thisfile->offset));
					cbfs_create_empty_file(location,
								   length -
								   (location -
								    current));
				}
			}
		}
		current =
		    ALIGN(current + ntohl(thisfile->len) +
			  ntohl(thisfile->offset), align);
	}
#endif
	ERROR("Could not add [%s, %zd bytes (%zd KB)]; probably too big?\n",
	      buffer->name, buffer->size, buffer->size / 1024);
	return 0;
}

int cbfs_remove_entry(struct cbfs_image *image, const char *name) {
	struct cbfs_file *entry;
	char *entry_name;
	size_t len, name_len;
	entry = cbfs_get_entry(image, name);
	if (!entry) {
		ERROR("CBFS file %s not found.\n", name);
		return -1;
	}
	entry_name = CBFS_NAME(entry);
	name_len = ntohl(entry->offset) - sizeof(*entry);
	DEBUG("cbfs_remove_entry: Removed %s @ 0x%x\n",
	      name, cbfs_get_entry_addr(image, entry));
	// entry->type = htonl(CBFS_COMPONENT_NULL);
	entry->type = htonl(CBFS_COMPONENT_DELETED);
	// adjust name & offset.
	// TODO offset(name) must be aligned at 16.
	memset(entry_name, 0, name_len);
	len = ntohl(entry->len) + name_len;
	entry->offset = htonl(sizeof(*entry) + CBFS_FILENAME_ALIGN);
	// TODO entry->len can expand to next aligned address.
	entry->len = htonl(len - ntohl(entry->offset));

	// TODO(hungte) Merge with previous and next file if possible.
	return 0;
}

struct cbfs_file *cbfs_get_entry(struct cbfs_image *image, const char *name) {
	struct cbfs_file *entry;
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		if (strcmp(CBFS_NAME(entry), name) == 0) {
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
	    get_cbfs_entry_type_name(ntohl(entry->type)),
	    ntohl(entry->len));

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
	printf("Successfully dumped the file to: %s\n", filename);
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

static int cbfs_print_stage_info(struct cbfs_stage *stage) {
	printf("    %s compression, entry: 0x%" PRIx64 ", load: 0x%" PRIx64 ", "
	       "length: %d/%d\n",
	       stage->compression == CBFS_COMPRESS_LZMA ? "LZMA" : "no",
	       stage->entry,
	       stage->load,
	       stage->len,
	       stage->memlen);
	return 0;
}

static int cbfs_print_payload_segment_info(struct cbfs_payload_segment *payload)
{
	switch(payload->type) {
		case PAYLOAD_SEGMENT_CODE:
		case PAYLOAD_SEGMENT_DATA:
			printf("    %s (%s compression, offset: 0x%x, "
			       "load: 0x%" PRIx64 ", length: %d/%d)\n",
			       (payload->type == PAYLOAD_SEGMENT_CODE ?
				"code " : "data"),
			       (payload->compression == CBFS_COMPRESS_LZMA ?
				"LZMA" : "no"),
			       ntohl(payload->offset),
			       htobe64(payload->load_addr),
			       ntohl(payload->len), ntohl(payload->mem_len));
			break;

		case PAYLOAD_SEGMENT_ENTRY:
			printf("    entry (0x%" PRIx64 ")\n",
			       htobe64(payload->load_addr));
			break;

		case PAYLOAD_SEGMENT_BSS:
			printf("    BSS (address 0x%016" PRIx64 ", "
			       "length 0x%x)\n",
			       htobe64(payload->load_addr),
			       ntohl(payload->len));
			break;

		case PAYLOAD_SEGMENT_PARAMS:
			printf("    parameters\n");
			break;

		default:
			printf("    %x (%s compression, offset: 0x%x, "
			       "load: 0x%" PRIx64 ", length: %d/%d\n",
			       payload->type,
			       (payload->compression == CBFS_COMPRESS_LZMA ?
			       "LZMA" : "no"),
			       ntohl(payload->offset),
			       htobe64(payload->load_addr),
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
	int *verbose = (int*)arg;

	printf("%-30s 0x%-8x %-12s %d\n",
	       *name ? name : "(empty)",
	       cbfs_get_entry_addr(image, entry),
	       get_cbfs_entry_type_name(ntohl(entry->type)),
	       ntohl(entry->len));

	if (!(verbose && *verbose))
		return 0;

	/* note the components of the subheader may be in host order ... */
	switch (ntohl(entry->type)) {
		case CBFS_COMPONENT_STAGE:
			cbfs_print_stage_info((struct cbfs_stage*)
					      CBFS_SUBHEADER(entry));
			break;

		case CBFS_COMPONENT_PAYLOAD:
			payload  = (struct cbfs_payload_segment*)
					CBFS_SUBHEADER(entry);
			while (payload) {
				cbfs_print_payload_segment_info(payload);
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
	// TODO check if next entry is valid in cbfs_image?
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
	entry->offset = sizeof(*entry) + strlen(buffer->name) + 1;
	return 0;
}


int cbfs_create_entry(struct cbfs_image *image, struct cbfs_file *entry,
		      size_t len) {
	memset(entry, -1, sizeof(*entry));
	memcpy(entry->magic, CBFS_FILE_MAGIC, sizeof(entry->magic));
	entry->type = htonl(CBFS_COMPONENT_NULL);
	entry->len = htonl(len);
	entry->checksum = 0;  // FIXME Get a checksum algorithm.
	entry->offset = htonl(sizeof(*entry) + CBFS_FILENAME_ALIGN);
	memset(CBFS_NAME(entry), 0, CBFS_FILENAME_ALIGN);
	return 0;
}

int cbfs_create_image(struct cbfs_image *image,
		      uint32_t size,
		      struct buffer *bootblock,
		      uint32_t align,
		      int32_t bootblock_offset,
		      int32_t header_offset,
		      int32_t entries_offset)
{
	struct cbfs_header *header;
	struct cbfs_file *entry;
	uint32_t cbfs_len;
	if (cbfs_image_create(image, size) != 0)
		return -1;

	// Adjust relative offset values (<0) to ROM address (>=0).
	if (entries_offset < 0)
		entries_offset += (int32_t)size;
	if (bootblock_offset < 0)
		bootblock_offset += (int32_t)size;
	if (header_offset < 0)
		header_offset += (int32_t) size;

	if (align == 0)
		align = 64;  // default align size.

	// Prepare bootblock
	if (bootblock_offset + bootblock->size > size) {
		ERROR("Bootblock (0x%x+0x%zx) exceed ROM size (0x%x)\n",
		      bootblock_offset, bootblock->size, size);
		return -1;
	}
	memcpy(image->buffer.data + bootblock_offset, bootblock->data,
	       bootblock->size);

	// Prepare header
	if (header_offset + sizeof(*header) > size) {
		ERROR("Header (0x%x+0x%zx) exceed ROM size (0x%x)\n",
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
	// TODO update x86 signature

	// Prepare entries
	if (align_up(entries_offset, align) != entries_offset) {
		ERROR("Offset (0x%x) must be aligned to 0x%x.\n",
		      entries_offset, align);
		return -1;
	}
	if (entries_offset + sizeof(*entry) > size) {
		ERROR("Offset (0x%x+0x%zx) exceed ROM size(0x%x)\n",
		      entries_offset, sizeof(*entry), size);
		return -1;
	}
	entry = (struct cbfs_file*)(image->buffer.data + entries_offset);
	// TODO Calculate available length by removing bootblock
	cbfs_len = size - entries_offset;
	cbfs_create_entry(image, entry, cbfs_len);
	return 0;
}



#if 0

static struct cbfs_file *merge_adjacent_files(struct cbfs_file *first,
					      struct cbfs_file *second)
{
	uint32_t new_length =
	    ntohl(first->len) + ntohl(second->len) + ntohl(second->offset);
	first->len = htonl(new_length);
	first->checksum = 0; // FIXME?
	return first;
}

/* returns new data block with cbfs_file header, suitable to dump into the ROM. location returns
   the new location that points to the cbfs_file header */
void *create_cbfs_file(const char *filename, void *data, uint32_t * datasize,
		       uint32_t type, uint32_t * location)
{
	uint32_t filename_len = ALIGN(strlen(filename) + 1, 16);
	uint32_t headersize = sizeof(struct cbfs_file) + filename_len;
	if ((location != 0) && (*location != 0)) {
		uint32_t offset = *location % align;
		/* If offset >= (headersize % align), we can stuff the header into the offset.
		   Otherwise the header has to be aligned itself, and put before the offset data */
		if (offset >= (headersize % align)) {
			offset -= (headersize % align);
		} else {
			offset += align - (headersize % align);
		}
		headersize += offset;
		*location -= headersize;
	}
	void *newdata = malloc(*datasize + headersize);
	if (!newdata) {
		fprintf(stderr, "E: Could not get %d bytes for CBFS file.\n", *datasize +
		       headersize);
		exit(1);
	}
	memset(newdata, 0xff, *datasize + headersize);
	struct cbfs_file *nextfile = (struct cbfs_file *)newdata;
	strncpy((char *)(nextfile->magic), "LARCHIVE", 8);
	nextfile->len = htonl(*datasize);
	nextfile->type = htonl(type);
	nextfile->checksum = 0;	// FIXME?
	nextfile->offset = htonl(headersize);
	strcpy(newdata + sizeof(struct cbfs_file), filename);
	memcpy(newdata + headersize, data, *datasize);
	*datasize += headersize;
	return newdata;
}

static int in_segment(int addr, int size, int gran)
{
	return ((addr & ~(gran - 1)) == ((addr + size) & ~(gran - 1)));
}

uint32_t cbfs_find_location(const char *romfile, uint32_t filesize,
			    const char *filename, uint32_t alignment)
{
	void *rom;
	size_t filename_size, headersize, totalsize;
	int ret = 0;
	uint32_t current;

	rom = loadrom(romfile);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			romfile);
		return 0;
	}

	filename_size = strlen(filename);
	headersize = sizeof(struct cbfs_file) + ALIGN(filename_size + 1, 16) +
			sizeof(struct cbfs_stage);
	totalsize = headersize + filesize;

	current = phys_start;
	while (current < phys_end) {
		uint32_t top;
		struct cbfs_file *thisfile;

		if (!cbfs_file_header(current)) {
			current += align;
			continue;
		}

		thisfile = (struct cbfs_file *)phys_to_virt(current);

		top = current + ntohl(thisfile->len) + ntohl(thisfile->offset);

		if (((ntohl(thisfile->type) == 0x0)
		     || (ntohl(thisfile->type) == 0xffffffff))
		    && (ntohl(thisfile->len) + ntohl(thisfile->offset) >=
			totalsize)) {
			if (in_segment
			    (current + headersize, filesize, alignment)) {
				ret = current + headersize;
				break;
			}
			if ((ALIGN(current, alignment) + filesize < top)
			    && (ALIGN(current, alignment) - headersize >
				current)
			    && in_segment(ALIGN(current, alignment), filesize,
					  alignment)) {
				ret = ALIGN(current, alignment);
				break;
			}
			if ((ALIGN(current, alignment) + alignment + filesize <
			     top)
			    && (ALIGN(current, alignment) + alignment -
				headersize > current)
			    && in_segment(ALIGN(current, alignment) + alignment,
					  filesize, alignment)) {
				ret = ALIGN(current, alignment) + alignment;
				break;
			}
		}
		current =
		    ALIGN(current + ntohl(thisfile->len) +
			  ntohl(thisfile->offset), align);
	}

	free(rom);
	return ret;
}
#endif
