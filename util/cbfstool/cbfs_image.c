/*
 * CBFS Image Manipulation
 *
 * Copyright (C) 2013 The Chromium OS Authors. All rights reserved.
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
 * Foundation, Inc.
 */

#include <inttypes.h>
#include <libgen.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "common.h"
#include "cbfs_image.h"

/* Even though the file-adding functions---cbfs_add_entry() and
 * cbfs_add_entry_at()---perform their sizing checks against the beginning of
 * the subsequent section rather than a stable recorded value such as an empty
 * file header's len field, it's possible to prove two interesting properties
 * about their behavior:
 *  - Placing a new file within an empty entry located below an existing file
 *    entry will never leave an aligned flash address containing neither the
 *    beginning of a file header nor part of a file.
 *  - Placing a new file in an empty entry at the very end of the image such
 *    that it fits, but leaves no room for a final header, is guaranteed not to
 *    change the total amount of space for entries, even if that new file is
 *    later removed from the CBFS.
 * These properties are somewhat nonobvious from the implementation, so the
 * reader is encouraged to blame this comment and examine the full proofs
 * in the commit message before making significant changes that would risk
 * removing said guarantees.
 */

/* The file name align is not defined in CBFS spec -- only a preference by
 * (old) cbfstool. */
#define CBFS_FILENAME_ALIGN	(16)

/* To make CBFS more friendly to ROM, fill -1 (0xFF) instead of zero. */
#define CBFS_CONTENT_DEFAULT_VALUE	(-1)

/* Type and format */

struct typedesc_t {
	uint32_t type;
	const char *name;
};

static const struct typedesc_t types_cbfs_entry[] = {
	{CBFS_COMPONENT_STAGE, "stage"},
	{CBFS_COMPONENT_PAYLOAD, "payload"},
	{CBFS_COMPONENT_OPTIONROM, "optionrom"},
	{CBFS_COMPONENT_BOOTSPLASH, "bootsplash"},
	{CBFS_COMPONENT_RAW, "raw"},
	{CBFS_COMPONENT_VSA, "vsa"},
	{CBFS_COMPONENT_MBI, "mbi"},
	{CBFS_COMPONENT_MICROCODE, "microcode"},
	{CBFS_COMPONENT_FSP, "fsp"},
	{CBFS_COMPONENT_MRC, "mrc"},
	{CBFS_COMPONENT_CMOS_DEFAULT, "cmos_default"},
	{CBFS_COMPONENT_CMOS_LAYOUT, "cmos_layout"},
	{CBFS_COMPONENT_SPD, "spd"},
	{CBFS_COMPONENT_MRC_CACHE, "mrc_cache"},
	{CBFS_COMPONENT_DELETED, "deleted"},
	{CBFS_COMPONENT_NULL, "null"},
	{0, NULL},
};

static const struct typedesc_t types_cbfs_compression[] = {
	{CBFS_COMPRESS_NONE, "none"},
	{CBFS_COMPRESS_LZMA, "LZMA"},
	{0, NULL},
};

static const char *lookup_name_by_type(const struct typedesc_t *desc, uint32_t type,
				const char *default_value)
{
	int i;
	for (i = 0; desc[i].name; i++)
		if (desc[i].type == type)
			return desc[i].name;
	return default_value;
}

static const char *get_cbfs_entry_type_name(uint32_t type)
{
	return lookup_name_by_type(types_cbfs_entry, type, "(unknown)");
}

/* CBFS image */

static size_t cbfs_calculate_file_header_size(const char *name)
{
	return (sizeof(struct cbfs_file) +
		align_up(strlen(name) + 1, CBFS_FILENAME_ALIGN));
}

/* Only call on legacy CBFSes possessing a master header. */
static int cbfs_fix_legacy_size(struct cbfs_image *image, char *hdr_loc)
{
	assert(image);
	assert(cbfs_is_legacy_cbfs(image));
	// A bug in old cbfstool may produce extra few bytes (by alignment) and
	// cause cbfstool to overwrite things after free space -- which is
	// usually CBFS header on x86. We need to workaround that.

	struct cbfs_file *entry, *first = NULL, *last = NULL;
	for (first = entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		last = entry;
	}
	if ((char *)first < (char *)hdr_loc &&
	    (char *)entry > (char *)hdr_loc) {
		WARN("CBFS image was created with old cbfstool with size bug. "
		     "Fixing size in last entry...\n");
		last->len = htonl(ntohl(last->len) - image->header.align);
		DEBUG("Last entry has been changed from 0x%x to 0x%x.\n",
		      cbfs_get_entry_addr(image, entry),
		      cbfs_get_entry_addr(image,
					  cbfs_find_next_entry(image, last)));
	}
	return 0;
}

void cbfs_put_header(void *dest, const struct cbfs_header *header)
{
	struct buffer outheader;

	outheader.data = dest;
	outheader.size = 0;

	xdr_be.put32(&outheader, header->magic);
	xdr_be.put32(&outheader, header->version);
	xdr_be.put32(&outheader, header->romsize);
	xdr_be.put32(&outheader, header->bootblocksize);
	xdr_be.put32(&outheader, header->align);
	xdr_be.put32(&outheader, header->offset);
	xdr_be.put32(&outheader, header->architecture);
}

static void cbfs_decode_payload_segment(struct cbfs_payload_segment *output,
					struct cbfs_payload_segment *input)
{
	struct buffer seg = {
		.data = (void *)input,
		.size = sizeof(*input),
	};
	output->type = xdr_be.get32(&seg);
	output->compression = xdr_be.get32(&seg);
	output->offset = xdr_be.get32(&seg);
	output->load_addr = xdr_be.get64(&seg);
	output->len = xdr_be.get32(&seg);
	output->mem_len = xdr_be.get32(&seg);
	assert(seg.size == 0);
}

void cbfs_get_header(struct cbfs_header *header, void *src)
{
	struct buffer outheader;

	outheader.data = src;	/* We're not modifying the data */
	outheader.size = 0;

	header->magic = xdr_be.get32(&outheader);
	header->version = xdr_be.get32(&outheader);
	header->romsize = xdr_be.get32(&outheader);
	header->bootblocksize = xdr_be.get32(&outheader);
	header->align = xdr_be.get32(&outheader);
	header->offset = xdr_be.get32(&outheader);
	header->architecture = xdr_be.get32(&outheader);
}

int cbfs_image_create(struct cbfs_image *image, size_t entries_size)
{
	assert(image);
	assert(image->buffer.data);

	size_t empty_header_len = cbfs_calculate_file_header_size("");
	uint32_t entries_offset = 0;
	uint32_t align = CBFS_ENTRY_ALIGNMENT;
	if (image->has_header) {
		entries_offset = image->header.offset;

		if (entries_offset > image->buffer.size) {
			ERROR("CBFS file entries are located outside CBFS itself\n");
			return -1;
		}

		align = image->header.align;
	}

	// This attribute must be given in order to prove that this module
	// correctly preserves certain CBFS properties. See the block comment
	// near the top of this file (and the associated commit message).
	if (align < empty_header_len) {
		ERROR("CBFS must be aligned to at least %zu bytes\n",
							empty_header_len);
		return -1;
	}

	if (entries_size > image->buffer.size - entries_offset) {
		ERROR("CBFS doesn't have enough space to fit its file entries\n");
		return -1;
	}

	if (empty_header_len > entries_size) {
		ERROR("CBFS is too small to fit any header\n");
		return -1;
	}
	struct cbfs_file *entry_header =
		(struct cbfs_file *)(image->buffer.data + entries_offset);
	// This alignment is necessary in order to prove that this module
	// correctly preserves certain CBFS properties. See the block comment
	// near the top of this file (and the associated commit message).
	entries_size -= entries_size % align;

	size_t capacity = entries_size - empty_header_len;
	LOG("Created CBFS (capacity = %zu bytes)\n", capacity);
	return cbfs_create_empty_entry(entry_header, capacity, "");
}

int cbfs_legacy_image_create(struct cbfs_image *image,
			     uint32_t architecture,
			     uint32_t align,
			     struct buffer *bootblock,
			     uint32_t bootblock_offset,
			     uint32_t header_offset,
			     uint32_t entries_offset)
{
	assert(image);
	assert(image->buffer.data);
	assert(bootblock);

	int32_t *rel_offset;
	uint32_t cbfs_len;
	void *header_loc;
	size_t size = image->buffer.size;

	DEBUG("cbfs_image_create: bootblock=0x%x+0x%zx, "
	      "header=0x%x+0x%zx, entries_offset=0x%x\n",
	      bootblock_offset, bootblock->size, header_offset,
	      sizeof(image->header), entries_offset);

	// Adjust legacy top-aligned address to ROM offset.
	if (IS_TOP_ALIGNED_ADDRESS(entries_offset))
		entries_offset = size + (int32_t)entries_offset;
	if (IS_TOP_ALIGNED_ADDRESS(bootblock_offset))
		bootblock_offset = size + (int32_t)bootblock_offset;
	if (IS_TOP_ALIGNED_ADDRESS(header_offset))
		header_offset = size + (int32_t)header_offset;

	DEBUG("cbfs_create_image: (real offset) bootblock=0x%x, "
	      "header=0x%x, entries_offset=0x%x\n",
	      bootblock_offset, header_offset, entries_offset);

	// Prepare bootblock
	if (bootblock_offset + bootblock->size > size) {
		ERROR("Bootblock (0x%x+0x%zx) exceed ROM size (0x%zx)\n",
		      bootblock_offset, bootblock->size, size);
		return -1;
	}
	if (entries_offset > bootblock_offset &&
	    entries_offset < bootblock->size) {
		ERROR("Bootblock (0x%x+0x%zx) overlap CBFS data (0x%x)\n",
		      bootblock_offset, bootblock->size, entries_offset);
		return -1;
	}
	memcpy(image->buffer.data + bootblock_offset, bootblock->data,
	       bootblock->size);

	// Prepare header
	if (header_offset + sizeof(image->header) > size - sizeof(int32_t)) {
		ERROR("Header (0x%x+0x%zx) exceed ROM size (0x%zx)\n",
		      header_offset, sizeof(image->header), size);
		return -1;
	}
	image->header.magic = CBFS_HEADER_MAGIC;
	image->header.version = CBFS_HEADER_VERSION;
	image->header.romsize = size;
	image->header.bootblocksize = bootblock->size;
	image->header.align = align;
	image->header.offset = entries_offset;
	image->header.architecture = architecture;

	header_loc = (image->buffer.data + header_offset);
	cbfs_put_header(header_loc, &image->header);
	image->has_header = true;

	// The last 4 byte of the image contain the relative offset from the end
	// of the image to the master header as a 32-bit signed integer. x86
	// relies on this also being its (memory-mapped, top-aligned) absolute
	// 32-bit address by virtue of how two's complement numbers work.
	assert(size % sizeof(int32_t) == 0);
	rel_offset = (int32_t *)(image->buffer.data + size - sizeof(int32_t));
	*rel_offset = header_offset - size;

	// Prepare entries
	if (align_up(entries_offset, align) != entries_offset) {
		ERROR("Offset (0x%x) must be aligned to 0x%x.\n",
		      entries_offset, align);
		return -1;
	}
	// To calculate available length, find
	//   e = min(bootblock, header, rel_offset) where e > entries_offset.
	cbfs_len = size - sizeof(int32_t);
	if (bootblock_offset > entries_offset && bootblock_offset < cbfs_len)
		cbfs_len = bootblock_offset;
	if (header_offset > entries_offset && header_offset < cbfs_len)
		cbfs_len = header_offset;

	if (cbfs_image_create(image, cbfs_len - entries_offset))
		return -1;
	return 0;
}

int cbfs_image_from_buffer(struct cbfs_image *out, struct buffer *in,
			   uint32_t offset)
{
	assert(out);
	assert(in);
	assert(in->data);

	buffer_clone(&out->buffer, in);
	out->has_header = false;

	void *header_loc = cbfs_find_header(in->data, in->size, offset);
	if (header_loc) {
		cbfs_get_header(&out->header, header_loc);
		out->has_header = true;
		cbfs_fix_legacy_size(out, header_loc);
	} else if (offset != ~0u) {
		ERROR("The -H switch is only valid on legacy images having CBFS master headers.\n");
		return 1;
	} else if (!cbfs_is_valid_cbfs(out)) {
		ERROR("Selected image region is not a valid CBFS.\n");
		return 1;
	}

	return 0;
}

int cbfs_copy_instance(struct cbfs_image *image, size_t copy_offset,
			size_t copy_size)
{
	assert(image);
	if (!cbfs_is_legacy_cbfs(image))
		return -1;

	struct cbfs_file *src_entry, *dst_entry;
	struct cbfs_header *copy_header;
	size_t align, entry_offset;
	ssize_t last_entry_size;

	size_t cbfs_offset, cbfs_end;
	size_t copy_end = copy_offset + copy_size;

	align = image->header.align;

	cbfs_offset = image->header.offset;
	cbfs_end = image->header.romsize;

	if (copy_end > image->buffer.size) {
		ERROR("Copy offset out of range: [%zx:%zx)\n",
			copy_offset, copy_end);
		return 1;
	}

	/* Range check requested copy region with source cbfs. */
	if ((copy_offset >= cbfs_offset && copy_offset < cbfs_end) ||
	    (copy_end >= cbfs_offset && copy_end <= cbfs_end)) {
		ERROR("New image would overlap old one.\n");
		return 1;
	}

	/* This will work, let's create a copy. */
	copy_header = (struct cbfs_header *)(image->buffer.data + copy_offset);
	cbfs_put_header(copy_header, &image->header);

	copy_header->bootblocksize = 0;
	/* Romsize is a misnomer. It's the absolute limit of cbfs content.*/
	copy_header->romsize = htonl(copy_end);
	entry_offset = align_up(copy_offset + sizeof(*copy_header), align);
	copy_header->offset = htonl(entry_offset);
	dst_entry = (struct cbfs_file *)(image->buffer.data + entry_offset);

	/* Copy non-empty files */
	for (src_entry = cbfs_find_first_entry(image);
	     src_entry && cbfs_is_valid_entry(image, src_entry);
	     src_entry = cbfs_find_next_entry(image, src_entry)) {
		size_t entry_size;

		if ((src_entry->type == htonl(CBFS_COMPONENT_NULL)) ||
		    (src_entry->type == htonl(CBFS_COMPONENT_DELETED)))
			continue;

		entry_size = htonl(src_entry->len) + htonl(src_entry->offset);
		memcpy(dst_entry, src_entry, entry_size);
		dst_entry = (struct cbfs_file *)(
			(uintptr_t)dst_entry + align_up(entry_size, align));

		if ((size_t)((char *)dst_entry - image->buffer.data) >=
								copy_end) {
			ERROR("Ran out of room in copy region.\n");
			return 1;
		}
	}

	/* Last entry size is all the room above it. */
	last_entry_size = copy_end - ((char *)dst_entry - image->buffer.data)
		- cbfs_calculate_file_header_size("");

	if (last_entry_size < 0)
		WARN("No room to create the last entry!\n")
	else
		cbfs_create_empty_entry(dst_entry, last_entry_size, "");

	return 0;
}

int cbfs_image_delete(struct cbfs_image *image)
{
	if (image == NULL)
		return 0;

	buffer_delete(&image->buffer);
	return 0;
}

/* Tries to add an entry with its data (CBFS_SUBHEADER) at given offset. */
static int cbfs_add_entry_at(struct cbfs_image *image,
			     struct cbfs_file *entry,
			     uint32_t size,
			     const char *name,
			     uint32_t type,
			     const void *data,
			     uint32_t content_offset)
{
	struct cbfs_file *next = cbfs_find_next_entry(image, entry);
	uint32_t addr = cbfs_get_entry_addr(image, entry),
		 addr_next = cbfs_get_entry_addr(image, next);
	uint32_t header_size = cbfs_calculate_file_header_size(name),
		 min_entry_size = cbfs_calculate_file_header_size("");
	uint32_t len, target;
	uint32_t align = image->has_header ? image->header.align :
							CBFS_ENTRY_ALIGNMENT;

	target = content_offset - header_size;
	if (target % align)
		target -= target % align;
	if (target < addr) {
		ERROR("No space to hold cbfs_file header.");
		return -1;
	}

	// Process buffer BEFORE content_offset.
	if (target - addr > min_entry_size) {
		DEBUG("|min|...|header|content|... <create new entry>\n");
		len = target - addr - min_entry_size;
		cbfs_create_empty_entry(entry, len, "");
		if (verbose > 1) cbfs_print_entry_info(image, entry, stderr);
		entry = cbfs_find_next_entry(image, entry);
		addr = cbfs_get_entry_addr(image, entry);
	}

	len = size + (content_offset - addr - header_size);
	cbfs_create_empty_entry(entry, len, name);
	if (len != size) {
		DEBUG("|..|header|content|... <use offset to create entry>\n");
		DEBUG("before: offset=0x%x, len=0x%x\n",
		      ntohl(entry->offset), ntohl(entry->len));
		// TODO reset expanded name buffer to 0xFF.
		entry->offset = htonl(ntohl(entry->offset) + (len - size));
		entry->len = htonl(size);
		DEBUG("after: offset=0x%x, len=0x%x\n",
		      ntohl(entry->offset), ntohl(entry->len));
	}

	// Ready to fill data into entry.
	assert(ntohl(entry->len) == size);
	entry->type = htonl(type);
	DEBUG("content_offset: 0x%x, entry location: %x\n",
	      content_offset, (int)((char*)CBFS_SUBHEADER(entry) -
				    image->buffer.data));
	assert((char*)CBFS_SUBHEADER(entry) - image->buffer.data ==
	       (ptrdiff_t)content_offset);
	memcpy(CBFS_SUBHEADER(entry), data, size);
	if (verbose > 1) cbfs_print_entry_info(image, entry, stderr);

	// Process buffer AFTER entry.
	entry = cbfs_find_next_entry(image, entry);
	addr = cbfs_get_entry_addr(image, entry);
	if (addr == addr_next)
		return 0;

	assert(addr < addr_next);
	if (addr_next - addr < min_entry_size) {
		DEBUG("No need for new \"empty\" entry\n");
		/* No need to increase the size of the just
		 * stored file to extend to next file. Alignment
		 * of next file takes care of this.
		 */
		return 0;
	}

	len = addr_next - addr - min_entry_size;
	cbfs_create_empty_entry(entry, len, "");
	if (verbose > 1) cbfs_print_entry_info(image, entry, stderr);
	return 0;
}

int cbfs_add_entry(struct cbfs_image *image, struct buffer *buffer,
		   const char *name, uint32_t type, uint32_t content_offset)
{
	assert(image);
	assert(buffer);
	assert(buffer->data);
	assert(name);
	assert(!IS_TOP_ALIGNED_ADDRESS(content_offset));

	uint32_t entry_type;
	uint32_t addr, addr_next;
	struct cbfs_file *entry, *next;
	uint32_t header_size, need_size, new_size;

	header_size = cbfs_calculate_file_header_size(name);

	need_size = header_size + buffer->size;
	DEBUG("cbfs_add_entry('%s'@0x%x) => need_size = %u+%zu=%u\n",
	      name, content_offset, header_size, buffer->size, need_size);

	// Merge empty entries.
	DEBUG("(trying to merge empty entries...)\n");
	cbfs_walk(image, cbfs_merge_empty_entry, NULL);

	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {

		entry_type = ntohl(entry->type);
		if (entry_type != CBFS_COMPONENT_NULL)
			continue;

		addr = cbfs_get_entry_addr(image, entry);
		next = cbfs_find_next_entry(image, entry);
		addr_next = cbfs_get_entry_addr(image, next);

		DEBUG("cbfs_add_entry: space at 0x%x+0x%x(%d) bytes\n",
		      addr, addr_next - addr, addr_next - addr);

		/* Will the file fit? Don't yet worry if we have space for a new
		 * "empty" entry. We take care of that later.
		 */
		if (addr + need_size > addr_next)
			continue;

		// Can we simply put object here?
		if (!content_offset || content_offset == addr + header_size) {
			DEBUG("Filling new entry data (%zd bytes).\n",
			      buffer->size);
			cbfs_create_empty_entry(entry, buffer->size, name);
			entry->type = htonl(type);
			memcpy(CBFS_SUBHEADER(entry), buffer->data, buffer->size);
			if (verbose)
				cbfs_print_entry_info(image, entry, stderr);

			// setup new entry
			DEBUG("Setting new empty entry.\n");
			entry = cbfs_find_next_entry(image, entry);
			new_size = (cbfs_get_entry_addr(image, next) -
				    cbfs_get_entry_addr(image, entry));

			/* Entry was added and no space for new "empty" entry */
			if (new_size < cbfs_calculate_file_header_size("")) {
				DEBUG("No need for new \"empty\" entry\n");
				/* No need to increase the size of the just
				 * stored file to extend to next file. Alignment
				 * of next file takes care of this.
				 */
				return 0;
			}
			new_size -= cbfs_calculate_file_header_size("");
			DEBUG("new size: %d\n", new_size);
			cbfs_create_empty_entry(entry, new_size, "");
			if (verbose)
				cbfs_print_entry_info(image, entry, stderr);
			return 0;
		}

		// We need to put content here, and the case is really
		// complicated...
		assert(content_offset);
		if (addr_next < content_offset) {
			DEBUG("Not for specified offset yet");
			continue;
		} else if (addr > content_offset) {
			DEBUG("Exceed specified content_offset.");
			break;
		} else if (addr + header_size > content_offset) {
			ERROR("Not enough space for header.\n");
			break;
		} else if (content_offset + buffer->size > addr_next) {
			ERROR("Not enough space for content.\n");
			break;
		}

		// TODO there are more few tricky cases that we may
		// want to fit by altering offset.
		DEBUG("section 0x%x+0x%x for content_offset 0x%x.\n",
		      addr, addr_next - addr, content_offset);

		if (cbfs_add_entry_at(image, entry, buffer->size, name, type,
				      buffer->data, content_offset) == 0) {
			return 0;
		}
		break;
	}

	ERROR("Could not add [%s, %zd bytes (%zd KB)@0x%x]; too big?\n",
	      buffer->name, buffer->size, buffer->size / 1024, content_offset);
	return -1;
}

struct cbfs_file *cbfs_get_entry(struct cbfs_image *image, const char *name)
{
	struct cbfs_file *entry;
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		if (strcasecmp(CBFS_NAME(entry), name) == 0) {
			DEBUG("cbfs_get_entry: found %s\n", name);
			return entry;
		}
	}
	return NULL;
}

int cbfs_export_entry(struct cbfs_image *image, const char *entry_name,
		      const char *filename)
{
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
	buffer.name = strdup("(cbfs_export_entry)");
	if (buffer_write_file(&buffer, filename) != 0) {
		ERROR("Failed to write %s into %s.\n",
		      entry_name, filename);
		free(buffer.name);
		return -1;
	}
	free(buffer.name);
	INFO("Successfully dumped the file to: %s\n", filename);
	return 0;
}

int cbfs_remove_entry(struct cbfs_image *image, const char *name)
{
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

int cbfs_print_header_info(struct cbfs_image *image)
{
	char *name = strdup(image->buffer.name);
	assert(image);
	printf("%s: %zd kB, bootblocksize %d, romsize %d, offset 0x%x\n"
	       "alignment: %d bytes, architecture: %s\n\n",
	       basename(name),
	       image->buffer.size / 1024,
	       image->header.bootblocksize,
	       image->header.romsize,
	       image->header.offset,
	       image->header.align,
	       arch_to_string(image->header.architecture));
	free(name);
	return 0;
}

static int cbfs_print_stage_info(struct cbfs_stage *stage, FILE* fp)
{
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

static int cbfs_print_decoded_payload_segment_info(
		struct cbfs_payload_segment *seg, FILE *fp)
{
	/* The input (seg) must be already decoded by
	 * cbfs_decode_payload_segment.
	 */
	switch (seg->type) {
		case PAYLOAD_SEGMENT_CODE:
		case PAYLOAD_SEGMENT_DATA:
			fprintf(fp, "    %s (%s compression, offset: 0x%x, "
				"load: 0x%" PRIx64 ", length: %d/%d)\n",
				(seg->type == PAYLOAD_SEGMENT_CODE ?
				 "code " : "data"),
				lookup_name_by_type(types_cbfs_compression,
						    seg->compression,
						    "(unknown)"),
				seg->offset, seg->load_addr, seg->len,
				seg->mem_len);
			break;

		case PAYLOAD_SEGMENT_ENTRY:
			fprintf(fp, "    entry (0x%" PRIx64 ")\n",
				seg->load_addr);
			break;

		case PAYLOAD_SEGMENT_BSS:
			fprintf(fp, "    BSS (address 0x%016" PRIx64 ", "
				"length 0x%x)\n",
				seg->load_addr, seg->len);
			break;

		case PAYLOAD_SEGMENT_PARAMS:
			fprintf(fp, "    parameters\n");
			break;

		default:
			fprintf(fp, "   0x%x (%s compression, offset: 0x%x, "
				"load: 0x%" PRIx64 ", length: %d/%d\n",
				seg->type,
				lookup_name_by_type(types_cbfs_compression,
						    seg->compression,
						    "(unknown)"),
				seg->offset, seg->load_addr, seg->len,
				seg->mem_len);
			break;
	}
	return 0;
}

int cbfs_print_entry_info(struct cbfs_image *image, struct cbfs_file *entry,
			  void *arg)
{
	const char *name = CBFS_NAME(entry);
	struct cbfs_payload_segment *payload;
	FILE *fp = (FILE *)arg;

	if (!cbfs_is_valid_entry(image, entry)) {
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
				struct cbfs_payload_segment seg;
				cbfs_decode_payload_segment(&seg, payload);
				cbfs_print_decoded_payload_segment_info(
						&seg, fp);
				if (seg.type == PAYLOAD_SEGMENT_ENTRY)
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

int cbfs_print_directory(struct cbfs_image *image)
{
	if (cbfs_is_legacy_cbfs(image))
		cbfs_print_header_info(image);
	printf("%-30s %-10s %-12s Size\n", "Name", "Offset", "Type");
	cbfs_walk(image, cbfs_print_entry_info, NULL);
	return 0;
}

int cbfs_merge_empty_entry(struct cbfs_image *image, struct cbfs_file *entry,
			   unused void *arg)
{
	struct cbfs_file *next;
	uint32_t type, addr, last_addr;

	type = ntohl(entry->type);
	if (type == CBFS_COMPONENT_DELETED) {
		// Ready to be recycled.
		type = CBFS_COMPONENT_NULL;
		entry->type = htonl(type);
	}
	if (type != CBFS_COMPONENT_NULL)
		return 0;

	next = cbfs_find_next_entry(image, entry);

	while (next && cbfs_is_valid_entry(image, next)) {
		type = ntohl(next->type);
		if (type == CBFS_COMPONENT_DELETED) {
			type = CBFS_COMPONENT_NULL;
			next->type = htonl(type);
		}
		if (type != CBFS_COMPONENT_NULL)
			return 0;

		addr = cbfs_get_entry_addr(image, entry);
		last_addr = cbfs_get_entry_addr(
				image, cbfs_find_next_entry(image, next));

		// Now, we find two deleted/empty entries; try to merge now.
		DEBUG("join_empty_entry: combine 0x%x+0x%x and 0x%x+0x%x.\n",
		      cbfs_get_entry_addr(image, entry), ntohl(entry->len),
		      cbfs_get_entry_addr(image, next), ntohl(next->len));
		cbfs_create_empty_entry(entry,
					(last_addr - addr -
					 cbfs_calculate_file_header_size("")),
					"");
		DEBUG("new empty entry: length=0x%x\n", ntohl(entry->len));
		next = cbfs_find_next_entry(image, entry);
	}
	return 0;
}

int cbfs_walk(struct cbfs_image *image, cbfs_entry_callback callback,
	      void *arg)
{
	int count = 0;
	struct cbfs_file *entry;
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		count ++;
		if (callback(image, entry, arg) != 0)
			break;
	}
	return count;
}

static int cbfs_header_valid(struct cbfs_header *header, size_t size)
{
	if ((ntohl(header->magic) == CBFS_HEADER_MAGIC) &&
	    ((ntohl(header->version) == CBFS_HEADER_VERSION1) ||
	     (ntohl(header->version) == CBFS_HEADER_VERSION2)) &&
	    (ntohl(header->romsize) <= size) &&
	    (ntohl(header->offset) < ntohl(header->romsize)))
		return 1;
	return 0;
}

struct cbfs_header *cbfs_find_header(char *data, size_t size,
				     uint32_t forced_offset)
{
	size_t offset;
	int found = 0;
	int32_t rel_offset;
	struct cbfs_header *header, *result = NULL;

	if (forced_offset < (size - sizeof(struct cbfs_header))) {
		/* Check if the forced header is valid. */
		header = (struct cbfs_header *)(data + forced_offset);
		if (cbfs_header_valid(header, size))
			return header;
		return NULL;
	}

	// Try finding relative offset of master header at end of file first.
	rel_offset = *(int32_t *)(data + size - sizeof(int32_t));
	offset = size + rel_offset;
	DEBUG("relative offset: %#zx(-%#zx), offset: %#zx\n",
	      (size_t)rel_offset, (size_t)-rel_offset, offset);

	if (offset >= size - sizeof(*header) ||
	    !cbfs_header_valid((struct cbfs_header *)(data + offset), size)) {
		// Some use cases append non-CBFS data to the end of the ROM.
		DEBUG("relative offset seems wrong, scanning whole image...\n");
		offset = 0;
	}

	for (; offset + sizeof(*header) < size; offset++) {
		header = (struct cbfs_header *)(data + offset);
		if (!cbfs_header_valid(header, size))
			continue;
		if (!found++)
			result = header;
	}
	if (found > 1)
		// Top-aligned images usually have a working relative offset
		// field, so this is more likely to happen on bottom-aligned
		// ones (where the first header is the "outermost" one)
		WARN("Multiple (%d) CBFS headers found, using the first one.\n",
		       found);
	return result;
}


struct cbfs_file *cbfs_find_first_entry(struct cbfs_image *image)
{
	assert(image);
	return image->has_header ? (struct cbfs_file *)(image->buffer.data +
						   image->header.offset) :
				   (struct cbfs_file *)image->buffer.data;
}

struct cbfs_file *cbfs_find_next_entry(struct cbfs_image *image,
				       struct cbfs_file *entry)
{
	uint32_t addr = cbfs_get_entry_addr(image, entry);
	int align = image->has_header ? image->header.align :
							CBFS_ENTRY_ALIGNMENT;
	assert(entry && cbfs_is_valid_entry(image, entry));
	addr += ntohl(entry->offset) + ntohl(entry->len);
	addr = align_up(addr, align);
	return (struct cbfs_file *)(image->buffer.data + addr);
}

uint32_t cbfs_get_entry_addr(struct cbfs_image *image, struct cbfs_file *entry)
{
	assert(image && image->buffer.data && entry);
	return (int32_t)((char *)entry - image->buffer.data);
}

int cbfs_is_valid_cbfs(struct cbfs_image *image)
{
	return buffer_check_magic(&image->buffer, CBFS_FILE_MAGIC,
						strlen(CBFS_FILE_MAGIC));
}

int cbfs_is_legacy_cbfs(struct cbfs_image *image)
{
	return image->has_header;
}

int cbfs_is_valid_entry(struct cbfs_image *image, struct cbfs_file *entry)
{
	uint32_t offset = cbfs_get_entry_addr(image, entry);

	if (offset >= image->buffer.size)
		return 0;

	struct buffer entry_data;
	buffer_clone(&entry_data, &image->buffer);
	buffer_seek(&entry_data, offset);
	return buffer_check_magic(&entry_data, CBFS_FILE_MAGIC,
						strlen(CBFS_FILE_MAGIC));
}

int cbfs_create_empty_entry(struct cbfs_file *entry,
			    size_t len, const char *name)
{
	memset(entry, CBFS_CONTENT_DEFAULT_VALUE, sizeof(*entry));
	memcpy(entry->magic, CBFS_FILE_MAGIC, sizeof(entry->magic));
	entry->type = htonl(CBFS_COMPONENT_NULL);
	entry->len = htonl(len);
	entry->checksum = 0;  // TODO Build a checksum algorithm.
	entry->offset = htonl(cbfs_calculate_file_header_size(name));
	memset(CBFS_NAME(entry), 0, ntohl(entry->offset) - sizeof(*entry));
	strcpy(CBFS_NAME(entry), name);
	memset(CBFS_SUBHEADER(entry), CBFS_CONTENT_DEFAULT_VALUE, len);
	return 0;
}

/* Finds a place to hold whole data in same memory page. */
static int is_in_same_page(uint32_t start, uint32_t size, uint32_t page)
{
	if (!page)
		return 1;
	return (start / page) == (start + size - 1) / page;
}

/* Tests if data can fit in a range by given offset:
 *  start ->| header_len | offset (+ size) |<- end
 */
static int is_in_range(uint32_t start, uint32_t end, uint32_t header_len,
		       uint32_t offset, uint32_t size)
{
	return (offset >= start + header_len && offset + size <= end);
}

int32_t cbfs_locate_entry(struct cbfs_image *image, const char *name,
			  uint32_t size, uint32_t page_size, uint32_t align)
{
	struct cbfs_file *entry;
	size_t need_len;
	uint32_t addr, addr_next, addr2, addr3, offset, header_len;

	/* Default values: allow fitting anywhere in ROM. */
	if (!page_size)
		page_size = image->has_header ? image->header.romsize :
							image->buffer.size;
	if (!align)
		align = 1;

	if (size > page_size)
		ERROR("Input file size (%d) greater than page size (%d).\n",
		      size, page_size);

	uint32_t image_align = image->has_header ? image->header.align :
							CBFS_ENTRY_ALIGNMENT;
	if (page_size % image_align)
		WARN("%s: Page size (%#x) not aligned with CBFS image (%#x).\n",
		     __func__, page_size, image_align);

	/* TODO Old cbfstool always assume input is a stage file (and adding
	 * sizeof(cbfs_stage) for header. We should fix that by adding "-t"
	 * (type) param in future. For right now, we assume cbfs_stage is the
	 * largest structure and add it into header size. */
	assert(sizeof(struct cbfs_stage) >= sizeof(struct cbfs_payload));
	header_len = (cbfs_calculate_file_header_size(name) +
		      sizeof(struct cbfs_stage));
	need_len = header_len + size;

	// Merge empty entries to build get max available space.
	cbfs_walk(image, cbfs_merge_empty_entry, NULL);

	/* Three cases of content location on memory page:
	 * case 1.
	 *          |  PAGE 1  |   PAGE 2  |
	 *          |     <header><content>| Fit. Return start of content.
	 *
	 * case 2.
	 *          |  PAGE 1  |   PAGE 2  |
	 *          | <header><content>    | Fits when we shift content to align
	 *  shift-> |  <header>|<content>  | at starting of PAGE 2.
	 *
	 * case 3. (large content filling whole page)
	 *  | PAGE 1 |  PAGE 2  | PAGE 3 |
	 *  |  <header>< content >       | Can't fit. If we shift content to
	 *  |trial-> <header>< content > | PAGE 2, header can't fit in free
	 *  |  shift->  <header><content> space, so we must use PAGE 3.
	 *
	 * The returned address can be then used as "base-address" (-b) in add-*
	 * commands (will be re-calculated and positioned by cbfs_add_entry_at).
	 * For stage targets, the address is also used to re-link stage before
	 * being added into CBFS.
	 */
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {

		uint32_t type = ntohl(entry->type);
		if (type != CBFS_COMPONENT_NULL)
			continue;

		addr = cbfs_get_entry_addr(image, entry);
		addr_next = cbfs_get_entry_addr(image, cbfs_find_next_entry(
				image, entry));
		if (addr_next - addr < need_len)
			continue;

		offset = align_up(addr + header_len, align);
		if (is_in_same_page(offset, size, page_size) &&
		    is_in_range(addr, addr_next, header_len, offset, size)) {
			DEBUG("cbfs_locate_entry: FIT (PAGE1).");
			return offset;
		}

		addr2 = align_up(addr, page_size);
		offset = align_up(addr2, align);
		if (is_in_range(addr, addr_next, header_len, offset, size)) {
			DEBUG("cbfs_locate_entry: OVERLAP (PAGE2).");
			return offset;
		}

		/* Assume page_size >= header_len so adding one page will
		 * definitely provide the space for header. */
		assert(page_size >= header_len);
		addr3 = addr2 + page_size;
		offset = align_up(addr3, align);
		if (is_in_range(addr, addr_next, header_len, offset, size)) {
			DEBUG("cbfs_locate_entry: OVERLAP+ (PAGE3).");
			return offset;
		}
	}
	return -1;
}
