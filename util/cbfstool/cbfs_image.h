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

#ifndef __CBFS_IMAGE_H
#define __CBFS_IMAGE_H
#include "common.h"
#include "cbfs.h"

/* CBFS image processing */

struct cbfs_image {
	struct buffer buffer;
	struct cbfs_header *header;
};

/* Loads a CBFS image from file. Returns 0 on success, otherwise non-zero. */
int cbfs_image_from_file(struct cbfs_image *image, const char *filename);

/* Writes a CBFS image into file. Returns 0 on success, otherwise non-zero. */
int cbfs_image_write_file(struct cbfs_image *image, const char *filename);

/* Releases the CBFS image. Returns 0 on success, otherwise non-zero. */
int cbfs_image_delete(struct cbfs_image *image);

/* Returns a pointer to entry by name, or NULL if name is not found. */
struct cbfs_file *cbfs_get_entry(struct cbfs_image *image, const char *name);

/* Exports an entry to external file.
 * Returns 0 on success, otherwise (ex, not found) non-zero. */
int cbfs_export_entry(struct cbfs_image *image, const char *entry_name,
		      const char *filename);

/* Removes an entry from CBFS image. Returns 0 on success, otherwise non-zero. */
int cbfs_remove_entry(struct cbfs_image *image, const char *name);

/* Callback function used by cbfs_walk.
 * Returns 0 on success, or non-zero to stop further iteration. */
typedef int (*cbfs_entry_callback)(struct cbfs_image *image,
				   struct cbfs_file *file,
				   void *arg);

/* Iterates through all entries in CBFS image, and invoke with callback.
 * Stops if callback returns non-zero values.
 * Returns number of entries invoked. */
int cbfs_walk(struct cbfs_image *image, cbfs_entry_callback callback, void *arg);

/* Primitive CBFS utilities */

/* Returns a pointer to the only valid CBFS header in give buffer, otherwise
 * NULL (including when multiple headers were found). If there is a X86 ROM
 * style signature (pointer at 0xfffffffc) found in ROM, it will be selected as
 * the only header.*/
struct cbfs_header *cbfs_find_header(char *data, size_t size);

/* Returns the first cbfs_file entry in CBFS image by CBFS header (no matter if
 * the entry has valid content or not), otherwise NULL. */
struct cbfs_file *cbfs_find_first_entry(struct cbfs_image *image);

/* Returns next cbfs_file entry (no matter if its content is valid or not), or
 * NULL on failure. */
struct cbfs_file *cbfs_find_next_entry(struct cbfs_image *image,
				       struct cbfs_file *entry);

/* Returns ROM address (offset) of entry.
 * This is different from entry->offset (pointer to content). */
uint32_t cbfs_get_entry_addr(struct cbfs_image *image, struct cbfs_file *entry);

/* Returns 1 if entry has valid data (by checking magic number), otherwise 0. */
int cbfs_is_valid_entry(struct cbfs_file *entry);

/* Print CBFS component information. */
int cbfs_print_directory(struct cbfs_image *image);
int cbfs_print_header_info(struct cbfs_image *image);
int cbfs_print_entry_info(struct cbfs_image *image, struct cbfs_file *entry,
			  void *arg);

#endif
