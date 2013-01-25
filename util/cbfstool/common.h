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
#include <arpa/inet.h>  /* for ntoh* and swab* */
#include "cbfs.h"

extern int verbose;
#define DEBUG(x...) { if (verbose > 1) fprintf(stderr, "DEBUG: " x); }
#define ERROR(x...) { fprintf(stderr, "ERROR: " x); }
#define WARN(x...) { fprintf(stderr, "WARN: " x); }
#define LOG(x...) { if (verbose > 0) printf(x); }
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

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
int buffer_load_file(struct buffer *buffer, const char *filename);

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

/* Creates an empty CBFS image. Returns 0 on success, otherwise non-zero. */
int create_cbfs_image(struct cbfs_image *image, size_t size);

/* Loads a CBFS image from file. Returns 0 on success, otherwise non-zero. */
int load_cbfs_image(struct cbfs_image *image, const char *filename);

/* Writes a CBFS image into file. Returns 0 on success, otherwise non-zero. */
int write_cbfs_image(struct cbfs_image *image, const char *filename);

/* Releases the CBFS image. Returns 0 on success, otherwise non-zero. */
int delete_cbfs_image(struct cbfs_image *image);

int cbfs_add_entry(struct cbfs_image *image, const char *name,
		   uint32_t type, struct buffer *buffer);

/* Removes an entry from CBFS image. Returns 0 on success, otherwise non-zero. */
int cbfs_remove_entry(struct cbfs_image *image, const char *name);

/* Returns a pointer to entry by name, or NULL if name is not found. */
struct cbfs_file *cbfs_get_entry(struct cbfs_image *image, const char *name);

/* Exports an entry to external file.
 * Returns 0 on success, otherwise (ex, not found) non-zero. */
int cbfs_export_entry(struct cbfs_image *image, const char *entry_name,
		      const char *filename);

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
struct cbfs_header *cbfs_find_header(char *data, size_t size);
struct cbfs_file *cbfs_find_first_entry(struct cbfs_image *image);
struct cbfs_file *cbfs_find_next_entry(struct cbfs_image *image,
				       struct cbfs_file *entry);
/* Returns ROM address (offset) of entry.
 * This is different from entry->offset (pointer to content). */
uint32_t cbfs_get_entry_addr(struct cbfs_image *image, struct cbfs_file *entry);

int cbfs_print_header_info(struct cbfs_image *image);
int cbfs_print_entry_info(struct cbfs_image *image, struct cbfs_file *entry,
			  void *arg);
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
		       uint32_t *location);
#endif
