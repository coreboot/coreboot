/*
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

#ifndef __CBFSTOOL_COMMON_H
#define __CBFSTOOL_COMMON_H

#include <stdint.h>
#include "cbfs.h"

/* Message output */
extern int verbose;
#define DEBUG(x...) { if (verbose > 1) fprintf(stderr, "DEBUG: " x); }
#define ERROR(x...) { fprintf(stderr, "ERROR: " x); }
#define WARN(x...) { fprintf(stderr, "WARN: " x); }
#define LOG(x...) { if (verbose > 0) printf(x); }
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* Endianess */
#include <arpa/inet.h>	// for ntohl, htonl
#include "swab.h"
int is_big_endian(void);
#define ntohll(x)	(is_big_endian() ? (x) : swab64(x))
#define htonll(x)	(is_big_endian() ? (x) : swab64(x))

/* Buffer and file I/O */
struct buffer {
	char *name;
	char *data;
	size_t size;
};

/* Creates an empty memory buffer with given size.
 * Returns 0 on success, otherwise non-zero. */
int buffer_create(struct buffer *buffer, size_t size, const char *name);

/* Loads a file into memory buffer. Returns 0 on success, otherwise non-zero. */
int buffer_from_file(struct buffer *buffer, const char *filename);

/* Writes memory buffer content into file.
 * Returns 0 on success, otherwise non-zero. */
int buffer_write_file(struct buffer *buffer, const char *filename);

/* Destroys a memory buffer. Returns 0 on success, otherwise non-zero. */
int buffer_delete(struct buffer *buffer);


/* Type and format */

/* Returns 1 if input points to an ELF object, otherwise 0. */
int is_elf_object(const void *input);

/* Returns CBFS entry type value for given name, or (uint32_t)-1 as unknown. */
uint32_t get_cbfs_entry_type(const char *name);

/* Gets the name of given CBFS entry type value, or "(unknown)". */
const char *get_cbfs_entry_type_name(uint32_t type);

/* Prints all known CBFS entry types. */
void print_all_cbfs_entry_types();


/* CBFS image processing */

struct cbfs_image {
	struct buffer buffer;
	struct cbfs_header *header;
};

/* Creates an empty CBFS image by given size, and description to its content
 * (bootblock, align, header location, starting offset of CBFS entries.
 * The output image will contain a valid cbfs_header, with one cbfs_file
 * entry with type CBFS_COMPONENT_NULL, with max available size.
 * Returns 0 on success, otherwise none-zero. */
int cbfs_image_create(struct cbfs_image *image,
		      size_t size,
		      uint32_t align,
		      struct buffer *bootblock,
		      int32_t bootblock_offset,
		      int32_t header_offset,
		      int32_t entries_offset);


/* Loads a CBFS image from file. Returns 0 on success, otherwise non-zero. */
int cbfs_image_from_file(struct cbfs_image *image, const char *filename);

/* Writes a CBFS image into file. Returns 0 on success, otherwise non-zero. */
int cbfs_image_write_file(struct cbfs_image *image, const char *filename);

/* Releases the CBFS image. Returns 0 on success, otherwise non-zero. */
int cbfs_image_delete(struct cbfs_image *image);

/* Adds an entry to CBFS image by given name and type.
 * If data_align is specified, entry data (CBFS_SUBHEADER(entry)) must be
 * aligned (usually for eXecute-In-Place).
 * Returns 0 on success, otherwise non-zero. */
int cbfs_add_entry(struct cbfs_image *image, const char *name,
		   uint32_t type, size_t data_align, struct buffer *buffer);

/* Removes an entry from CBFS image. Returns 0 on success, otherwise non-zero. */
int cbfs_remove_entry(struct cbfs_image *image, const char *name);

/* Returns a pointer to entry by name, or NULL if name is not found. */
struct cbfs_file *cbfs_get_entry(struct cbfs_image *image, const char *name);

/* Exports an entry to external file.
 * Returns 0 on success, otherwise (ex, not found) non-zero. */
int cbfs_export_entry(struct cbfs_image *image, const char *entry_name,
		      const char *filename);

/* Initializes a new entry with size and name in CBFS image.
 * Returns 0 on success, otherwise (ex, not found) non-zero. */
int cbfs_create_entry(struct cbfs_image *image, struct cbfs_file *entry,
		      size_t len, const char *name);

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

int cbfs_print_header_info(struct cbfs_image *image);
int cbfs_print_entry_info(struct cbfs_image *image, struct cbfs_file *entry,
			  void *arg);
int cbfs_merge_empty_entry(struct cbfs_image *image, struct cbfs_file *entry,
			   void *arg);

/* Returns 1 if entry has valid data (by checking magic number), otherwise 0. */
int cbfs_is_valid_entry(struct cbfs_file *entry);

/* Utilities */

/* compress.c */
typedef void (*comp_func_ptr) (char *, int, char *, int *);
typedef enum {
	CBFS_COMPRESS_NONE = 0,
	CBFS_COMPRESS_LZMA
} comp_algo;
comp_func_ptr compression_function(comp_algo algo);

/* cbfs-mkpayload.c */
int parse_elf_to_payload(char *input, char **output, comp_algo algo);
int parse_flat_binary_to_payload(char *input, char **output,
				 uint32_t input_size, uint32_t loadaddress,
				 uint32_t entrypoint, comp_algo algo);
/* cbfs-mkstage.c */
int parse_elf_to_stage(char *input, char **output, comp_algo algo,
		       uint32_t location);
#endif
