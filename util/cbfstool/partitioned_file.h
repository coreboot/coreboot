/*
 * partitioned_file.h, read and write binary file "partitions" described by FMAP
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

#ifndef PARITITONED_FILE_H_
#define PARITITONED_FILE_H_

#include "common.h"
#include "flashmap/fmap.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct partitioned_file partitioned_file_t;

/**
 * Create a new filesystem-backed flat buffer.
 * This backwards-compatibility function creates a new in-memory buffer and
 * backing binary file of the specified size. Although the file won't actually
 * have multiple regions, it'll still be possible to access and manipulate it
 * using this module; this is accomplished by requesting the special region
 * whose name matches SECTION_NAME_PRIMARY_CBFS, which maps to the whole file.
 * Note that the caller will be responsible for calling partitioned_file_close()
 * on the returned object, and that this function will overwrite any existing
 * file with the given name without warning.
 *
 * @param filename   Name of the backing file
 * @param image_size Size of the image
 * @return           Caller-owned partitioned file, or NULL on error
 */
partitioned_file_t *partitioned_file_create_flat(const char *filename,
							size_t image_size);

/**
 * Create a new filesystem-backed partitioned buffer.
 * This creates a new in-memory buffer and backing binary file. Both are
 * segmented into regions according to the provided flashmap's sections, and the
 * flashmap itself is automatically copied into the region named
 * SECTION_NAME_FMAP: a section with this name must already exist in the FMAP.
 * After calling this function, it is safe for the caller to clean up flashmap
 * at any time. The partitioned_file_t returned from this function is separately
 * owned by the caller, and must later be passed to partitioned_file_close().
 * Note that this function will overwrite any existing file with the given name
 * without warning.
 *
 * @param filename Name of the backing file
 * @param flashmap Buffer containing an FMAP file layout
 * @return         Caller-owned partitioned file, or NULL on error
 */
partitioned_file_t *partitioned_file_create(const char *filename,
						struct buffer *flashmap);

/**
 * Read a file back in from the disk.
 * An in-memory buffer is created and populated with the file's
 * contents. If the image contains an FMAP, it will be opened as a
 * full partitioned file; otherwise, it will be opened as a flat file as
 * if it had been created by partitioned_file_create_flat().
 * The partitioned_file_t returned from this function is separately owned by the
 * caller, and must later be passed to partitioned_file_close();
 *
 * @param filename      Name of the file to read in
 * @param write_access  True if the file needs to be modified
 * @return              Caller-owned partitioned file, or NULL on error
 */
partitioned_file_t *partitioned_file_reopen(const char *filename,
					    bool write_access);

/**
 * Write a buffer's contents to its original region within a segmented file.
 * This function should only be called on buffers originally retrieved by a call
 * to partitioned_file_read_region() on the same partitioned file object. The
 * contents of this buffer are copied back to the same region of the buffer and
 * backing file that the region occupied before.
 *
 * @param file   Partitioned file to which to write the data
 * @param buffer Modified buffer obtained from partitioned_file_read_region()
 * @return       Whether the operation was successful
 */
bool partitioned_file_write_region(partitioned_file_t *file,
						const struct buffer *buffer);

/**
 * Obtain one particular region of a segmented file.
 * The result is owned by the partitioned_file_t and shared among every caller
 * of this function. Thus, it is an error to buffer_delete() it; instead, clean
 * up the entire partitioned_file_t once it's no longer needed with a single
 * call to partitioned_file_close().
 * Note that, if the buffer obtained from this function is modified, the changes
 * will be reflected in any buffers handed out---whether earlier or later---for
 * any region inclusive of the altered location(s). However, the backing file
 * will not be updated until someone calls partitioned_file_write_region() on a
 * buffer that includes the alterations.
 *
 * @param dest   Empty destination buffer for the data
 * @param file   Partitioned file from which to read the data
 * @param region Name of the desired FMAP region
 * @return       Whether the copy was performed successfully
 */
bool partitioned_file_read_region(struct buffer *dest,
			const partitioned_file_t *file, const char *region);

/** @param file Partitioned file to flush and cleanup */
void partitioned_file_close(partitioned_file_t *file);

/** @return Whether the file is partitioned (i.e. not flat). */
bool partitioned_file_is_partitioned(const partitioned_file_t *file);

/** @return The image's overall filesize, regardless of whether it's flat. */
size_t partitioned_file_total_size(const partitioned_file_t *file);

/** @return Whether the specified region begins with the magic bytes. */
bool partitioned_file_region_check_magic(const partitioned_file_t *file,
		const char *region, const char *magic, size_t magic_len);

/** @return Whether the specified region exists and contains nested regions. */
bool partitioned_file_region_contains_nested(const partitioned_file_t *file,
							const char *region);

/** @return An immutable reference to the FMAP, or NULL for flat images. */
const struct fmap *partitioned_file_get_fmap(const partitioned_file_t *file);

/** @return Whether to include area in the running count. */
typedef bool (*partitioned_file_fmap_selector_t)
				(const struct fmap_area *area, const void *arg);

/**
 * Count the number of FMAP entries fulfilling a certain criterion.
 * The result is always 0 if run on a flat (non-partitioned) image.
 *
 * @param file     File on whose FMAP entries the operation should be run
 * @param callback Decider answering whether each individual region should count
 * @param arg      Additional information to furnish to the decider on each call
 * @return         The number of FMAP sections with that property
 */
unsigned partitioned_file_fmap_count(const partitioned_file_t *file,
		partitioned_file_fmap_selector_t callback, const void *arg);

/** Selector that counts every single FMAP section. */
extern const partitioned_file_fmap_selector_t partitioned_file_fmap_select_all;

/** Selector that counts FMAP sections that are descendants of fmap_area arg. */
extern const partitioned_file_fmap_selector_t
				partitioned_file_fmap_select_children_of;

/** Selector that counts FMAP sections that contain the fmap_area arg. */
extern const partitioned_file_fmap_selector_t
					partitioned_file_fmap_select_parents_of;

#endif
