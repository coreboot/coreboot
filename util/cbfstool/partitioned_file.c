/*
 * partitioned_file.c, read and write binary file "partitions" described by FMAP
 *
 * Copyright (C) 2015 Google, Inc.
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

#include "partitioned_file.h"

#include "cbfs_sections.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct partitioned_file {
	struct fmap *fmap;
	struct buffer buffer;
	FILE *stream;
};

static bool fill_ones_through(struct partitioned_file *file)
{
	assert(file);

	memset(file->buffer.data, 0xff, file->buffer.size);
	return partitioned_file_write_region(file, &file->buffer);
}

static unsigned count_selected_fmap_entries(const struct fmap *fmap,
		partitioned_file_fmap_selector_t callback, const void *arg)
{
	assert(fmap);
	assert(callback);

	unsigned count = 0;
	for (unsigned i = 0; i < fmap->nareas; ++i) {
		if (callback(fmap->areas + i, arg))
			++count;
	}
	return count;
}

static partitioned_file_t *reopen_flat_file(const char *filename,
					    bool write_access)
{
	assert(filename);
	struct partitioned_file *file = calloc(1, sizeof(*file));
	const char *access_mode;

	if (!file) {
		ERROR("Failed to allocate partitioned file structure\n");
		return NULL;
	}

	if (buffer_from_file(&file->buffer, filename)) {
		free(file);
		return NULL;
	}

	access_mode = write_access ?  "rb+" : "rb";
	file->stream = fopen(filename, access_mode);

	if (!file->stream) {
		perror(filename);
		partitioned_file_close(file);
		return NULL;
	}

	return file;
}

partitioned_file_t *partitioned_file_create_flat(const char *filename,
							size_t image_size)
{
	assert(filename);

	struct partitioned_file *file = calloc(1, sizeof(*file));
	if (!file) {
		ERROR("Failed to allocate partitioned file structure\n");
		return NULL;
	}

	file->stream = fopen(filename, "wb");
	if (!file->stream) {
		perror(filename);
		free(file);
		return NULL;
	}

	if (buffer_create(&file->buffer, image_size, filename)) {
		partitioned_file_close(file);
		return NULL;
	}

	if (!fill_ones_through(file)) {
		partitioned_file_close(file);
		return NULL;
	}

	return file;
}

partitioned_file_t *partitioned_file_create(const char *filename,
							struct buffer *flashmap)
{
	assert(filename);
	assert(flashmap);
	assert(flashmap->data);

	if (fmap_find((const uint8_t *)flashmap->data, flashmap->size) != 0) {
		ERROR("Attempted to create a partitioned image out of something that isn't an FMAP\n");
		return NULL;
	}
	struct fmap *bootstrap_fmap = (struct fmap *)flashmap->data;

	const struct fmap_area *fmap_area =
			fmap_find_area(bootstrap_fmap, SECTION_NAME_FMAP);
	if (!fmap_area) {
		ERROR("Provided FMAP missing '%s' region\n", SECTION_NAME_FMAP);
		return NULL;
	}

	if (count_selected_fmap_entries(bootstrap_fmap,
		partitioned_file_fmap_select_children_of, fmap_area)) {
		ERROR("Provided FMAP's '%s' region contains other regions\n",
							SECTION_NAME_FMAP);
		return NULL;
	}

	int fmap_len = fmap_size(bootstrap_fmap);
	if (fmap_len < 0) {
		ERROR("Unable to determine size of provided FMAP\n");
		return NULL;
	}
	assert((size_t)fmap_len <= flashmap->size);
	if ((uint32_t)fmap_len > fmap_area->size) {
		ERROR("Provided FMAP's '%s' region needs to be at least %d bytes\n",
						SECTION_NAME_FMAP, fmap_len);
		return NULL;
	}

	partitioned_file_t *file = partitioned_file_create_flat(filename,
							bootstrap_fmap->size);
	if (!file)
		return NULL;

	struct buffer fmap_region;
	buffer_splice(&fmap_region, &file->buffer, fmap_area->offset, fmap_area->size);
	memcpy(fmap_region.data, bootstrap_fmap, fmap_len);
	if (!partitioned_file_write_region(file, &fmap_region)) {
		partitioned_file_close(file);
		return NULL;
	}
	file->fmap = (struct fmap *)(file->buffer.data + fmap_area->offset);

	return file;
}

partitioned_file_t *partitioned_file_reopen(const char *filename,
					    bool write_access)
{
	assert(filename);

	partitioned_file_t *file = reopen_flat_file(filename, write_access);
	if (!file)
		return NULL;

	long fmap_region_offset = fmap_find((const uint8_t *)file->buffer.data,
							file->buffer.size);
	if (fmap_region_offset < 0) {
		INFO("Opening image as a flat file because it doesn't contain any FMAP\n");
		return file;
	}
	file->fmap = (struct fmap *)(file->buffer.data + fmap_region_offset);

	if (file->fmap->size > file->buffer.size) {
		int fmap_region_size = fmap_size(file->fmap);
		ERROR("FMAP records image size as %u, but file is only %zu bytes%s\n",
					file->fmap->size, file->buffer.size,
						fmap_region_offset == 0 &&
				(signed)file->buffer.size == fmap_region_size ?
				" (is it really an image, or *just* an FMAP?)" :
					" (did something truncate this file?)");
		partitioned_file_close(file);
		return NULL;
	}

	const struct fmap_area *fmap_fmap_entry =
				fmap_find_area(file->fmap, SECTION_NAME_FMAP);
	if ((long)fmap_fmap_entry->offset != fmap_region_offset) {
		ERROR("FMAP's '%s' section doesn't point back to FMAP start (did something corrupt this file?)\n",
							SECTION_NAME_FMAP);
		partitioned_file_close(file);
		return NULL;
	}

	return file;
}

bool partitioned_file_write_region(partitioned_file_t *file,
						const struct buffer *buffer)
{
	assert(file);
	assert(file->stream);
	assert(buffer);
	assert(buffer->data);

	if (buffer->data - buffer->offset != file->buffer.data) {
		ERROR("Attempted to write a partition buffer back to a different file than it came from\n");
		return false;
	}
	if (buffer->offset + buffer->size > file->buffer.size) {
		ERROR("Attempted to write data off the end of image file\n");
		return false;
	}

	if (fseek(file->stream, buffer->offset, SEEK_SET)) {
		ERROR("Failed to seek within image file\n");
		return false;
	}
	if (!fwrite(buffer->data, buffer->size, 1, file->stream)) {
		ERROR("Failed to write to image file\n");
		return false;
	}
	return true;
}

bool partitioned_file_read_region(struct buffer *dest,
			const partitioned_file_t *file, const char *region)
{
	assert(dest);
	assert(file);
	assert(file->buffer.data);
	assert(region);

	if (file->fmap) {
		const struct fmap_area *area = fmap_find_area(file->fmap,
									region);
		if (!area) {
			ERROR("Image is missing '%s' region\n", region);
			return false;
		}
		if (area->offset + area->size > file->buffer.size) {
			ERROR("Region '%s' runs off the end of the image file\n",
									region);
			return false;
		}
		buffer_splice(dest, &file->buffer, area->offset, area->size);
	} else {
		if (strcmp(region, SECTION_NAME_PRIMARY_CBFS) != 0) {
			ERROR("This is a legacy image that contains only a CBFS\n");
			return false;
		}
		buffer_clone(dest, &file->buffer);
	}

	return true;
}

void partitioned_file_close(partitioned_file_t *file)
{
	if (!file)
		return;

	file->fmap = NULL;
	buffer_delete(&file->buffer);
	if (file->stream) {
		fclose(file->stream);
		file->stream = NULL;
	}
	free(file);
}

bool partitioned_file_is_partitioned(const partitioned_file_t *file)
{
	return partitioned_file_get_fmap(file) != NULL;
}

size_t partitioned_file_total_size(const partitioned_file_t *file)
{
	assert(file);

	return file->buffer.size;
}

bool partitioned_file_region_check_magic(const partitioned_file_t *file,
			const char *region, const char *magic, size_t magic_len)
{
	struct buffer area;
	return partitioned_file_read_region(&area, file, region) &&
				buffer_check_magic(&area, magic, magic_len);
}

bool partitioned_file_region_contains_nested(const partitioned_file_t *file,
							const char *region)
{
	assert(file);
	assert(region);

	if (!file->fmap)
		return false;
	const struct fmap_area *area = fmap_find_area(file->fmap, region);
	return area && partitioned_file_fmap_count(file,
			partitioned_file_fmap_select_children_of, area);
}

const struct fmap *partitioned_file_get_fmap(const partitioned_file_t *file)
{
	assert(file);

	return file->fmap;
}

unsigned partitioned_file_fmap_count(const partitioned_file_t *file,
		partitioned_file_fmap_selector_t callback, const void *arg)
{
	assert(file);
	assert(callback);

	if (!file->fmap)
		return 0;
	return count_selected_fmap_entries(file->fmap, callback, arg);
}

static bool select_all(unused const struct fmap_area *area,
							unused const void *arg)
{
	return true;
}
const partitioned_file_fmap_selector_t partitioned_file_fmap_select_all =
								select_all;

static bool select_children_of(const struct fmap_area *child, const void *arg)
{
	assert(child);
	assert(arg);

	const struct fmap_area *parent = (const struct fmap_area *)arg;
	if (child == arg || (child->offset == parent->offset &&
						child->size == parent->size))
		return false;
	return child->offset >= parent->offset &&
		child->offset + child->size <= parent->offset + parent->size;
}
const partitioned_file_fmap_selector_t
		partitioned_file_fmap_select_children_of = select_children_of;

static bool select_parents_of(const struct fmap_area *parent, const void *arg)
{
	return select_children_of((const struct fmap_area *)arg, parent);
}
const partitioned_file_fmap_selector_t partitioned_file_fmap_select_parents_of =
							select_parents_of;
