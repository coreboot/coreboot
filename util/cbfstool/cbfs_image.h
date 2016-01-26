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
 */

#ifndef __CBFS_IMAGE_H
#define __CBFS_IMAGE_H
#include "common.h"
#include "cbfs.h"

/* CBFS image processing */

struct cbfs_image {
	struct buffer buffer;
	/* An image has a header iff it's a legacy CBFS. */
	bool has_header;
	/* Only meaningful if has_header is selected. */
	struct cbfs_header header;
};

/* Given the string name of a compression algorithm, return the corresponding
 * enum comp_algo if it's supported, or a number < 0 otherwise. */
int cbfs_parse_comp_algo(const char *name);

/* Given the string name of a hash algorithm, return the corresponding
 * id if it's supported, or a number < 0 otherwise. */
int cbfs_parse_hash_algo(const char *name);

/* Given a pointer, serialize the header from host-native byte format
 * to cbfs format, i.e. big-endian. */
void cbfs_put_header(void *dest, const struct cbfs_header *header);
/* Or deserialize into host-native format */
void cbfs_get_header(struct cbfs_header *header, void *src);

/* Populates a CBFS with a single empty entry filling all available space
 * (entries_size bytes). If image's header field is already present, its
 * contents will be used to place an empty entry of the requested length at the
 * appropriate position in the existing buffer; otherwise, if not has_header,
 * the first entries_size bytes of buffer will be filled exclusively with the
 * single empty entry (and no CBFS master header).
 * Returns 0 on success, otherwise nonzero. */
int cbfs_image_create(struct cbfs_image *image, size_t entries_size);

/* Creates an empty CBFS image by given size, and description to its content
 * (bootblock, align, header location, starting offset of CBFS entries).
 * The output image will contain a valid cbfs_header, with one cbfs_file
 * entry with type CBFS_COMPONENT_NULL, with max available size.
 * Only call this if you want a legacy CBFS with a master header.
 * Returns 0 on success, otherwise nonzero. */
int cbfs_legacy_image_create(struct cbfs_image *image,
			      uint32_t arch,
			      uint32_t align,
			      struct buffer *bootblock,
			      uint32_t bootblock_offset,
			      uint32_t header_offset,
			      uint32_t entries_offset);

/* Constructs a cbfs_image from a buffer. The resulting image contains a shallow
 * copy of the buffer; releasing either one is the legal way to clean up after
 * both of them at once. Always produces a cbfs_image, but...
 * Returns 0 if it contains a valid CBFS, non-zero if it's unrecognized data. */
int cbfs_image_from_buffer(struct cbfs_image *out, struct buffer *in,
			   uint32_t offset);

/* Create a duplicate CBFS image. Returns 0 on success, otherwise non-zero.
 * Will not succeed on new-style images without a master header. */
int cbfs_copy_instance(struct cbfs_image *image, struct buffer *dst);

/* Compact a fragmented CBFS image by placing all the non-empty files at the
 * beginning of the image. Returns 0 on success, otherwise non-zero.  */
int cbfs_compact_instance(struct cbfs_image *image);

/* Releases the CBFS image. Returns 0 on success, otherwise non-zero. */
int cbfs_image_delete(struct cbfs_image *image);

/* Returns a pointer to entry by name, or NULL if name is not found. */
struct cbfs_file *cbfs_get_entry(struct cbfs_image *image, const char *name);

/* Exports an entry to external file.
 * Returns 0 on success, otherwise (ex, not found) non-zero. */
int cbfs_export_entry(struct cbfs_image *image, const char *entry_name,
		      const char *filename, uint32_t arch);

/* Adds an entry to CBFS image by given name and type. If content_offset is
 * non-zero, try to align "content" (CBFS_SUBHEADER(p)) at content_offset.
 * Never pass this function a top-aligned address: convert it to an offset.
 * Returns 0 on success, otherwise non-zero. */
int cbfs_add_entry(struct cbfs_image *image, struct buffer *buffer,
		   uint32_t content_offset, struct cbfs_file *header);

/* Removes an entry from CBFS image. Returns 0 on success, otherwise non-zero. */
int cbfs_remove_entry(struct cbfs_image *image, const char *name);

/* Create a new cbfs file header structure to work with.
   Returns newly allocated memory that the caller needs to free after use. */
struct cbfs_file *cbfs_create_file_header(int type, size_t len,
	const char *name);

/* Initializes a new empty (type = NULL) entry with size and name in CBFS image.
 * Returns 0 on success, otherwise (ex, not found) non-zero. */
int cbfs_create_empty_entry(struct cbfs_file *entry, int type,
			    size_t len, const char *name);

/* Finds a location to put given content by specified criteria:
 *  "page_size" limits the content to fit on same memory page, and
 *  "align" specifies starting address alignment.
 * Returns a valid offset, or -1 on failure. */
int32_t cbfs_locate_entry(struct cbfs_image *image, size_t size,
			  size_t page_size, size_t align, size_t metadata_size);

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
struct cbfs_header *cbfs_find_header(char *data, size_t size,
				     uint32_t forced_offset);

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

/* Returns 1 if valid new-format CBFS (without a master header), otherwise 0. */
int cbfs_is_valid_cbfs(struct cbfs_image *image);

/* Returns 1 if valid legacy CBFS (with a master header), otherwise 0. */
int cbfs_is_legacy_cbfs(struct cbfs_image *image);

/* Returns 1 if entry has valid data (by checking magic number), otherwise 0. */
int cbfs_is_valid_entry(struct cbfs_image *image, struct cbfs_file *entry);

/* Print CBFS component information. */
int cbfs_print_directory(struct cbfs_image *image);
int cbfs_print_parseable_directory(struct cbfs_image *image);
int cbfs_print_header_info(struct cbfs_image *image);
int cbfs_print_entry_info(struct cbfs_image *image, struct cbfs_file *entry,
			  void *arg);

/* Merge empty entries starting from given entry.
 * Returns 0 on success, otherwise non-zero. */
int cbfs_merge_empty_entry(struct cbfs_image *image, struct cbfs_file *entry,
			   void *arg);

/* Returns the size of a cbfs file header with no extensions */
size_t cbfs_calculate_file_header_size(const char *name);

/* Given a cbfs_file, return the first file attribute, or NULL. */
struct cbfs_file_attribute *cbfs_file_first_attr(struct cbfs_file *file);

/* Given a cbfs_file and a cbfs_file_attribute, return the attribute that
 * follows it, or NULL. */
struct cbfs_file_attribute *cbfs_file_next_attr(struct cbfs_file *file,
	struct cbfs_file_attribute *attr);

/* Adds to header a new extended attribute tagged 'tag', sized 'size'.
 * Returns pointer to the new attribute, or NULL on error. */
struct cbfs_file_attribute *cbfs_add_file_attr(struct cbfs_file *header,
					       uint32_t tag,
					       uint32_t size);

/* Adds an extended attribute to header, containing a hash of buffer's data of
 * the type specified by hash_type.
 * Returns 0 on success, -1 on error. */
int cbfs_add_file_hash(struct cbfs_file *header, struct buffer *buffer,
	enum vb2_hash_algorithm hash_type);
#endif
