/*
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 * Copyright (C) 2012 Google, Inc.
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
#include <assert.h>

/* Endianess */
#include "swab.h"
#ifndef __APPLE__
#define ntohl(x)	(is_big_endian() ? (x) : swab32(x))
#define htonl(x)	(is_big_endian() ? (x) : swab32(x))
#endif
#define ntohll(x)	(is_big_endian() ? (x) : swab64(x))
#define htonll(x)	(is_big_endian() ? (x) : swab64(x))
extern int is_big_endian(void);

/* Message output */
extern int verbose;
#define ERROR(x...) { fprintf(stderr, "E: " x); }
#define WARN(x...) { fprintf(stderr, "W: " x); }
#define LOG(x...) { fprintf(stderr, x); }
#define INFO(x...) { if (verbose > 0) fprintf(stderr, "INFO: " x); }
#define DEBUG(x...) { if (verbose > 1) fprintf(stderr, "DEBUG: " x); }

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

/* Destroys a memory buffer. */
void buffer_delete(struct buffer *buffer);

extern uint32_t arch;

uint32_t string_to_arch(const char *arch_string);

#define ALIGN(val, by) (((val) + (by)-1)&~((by)-1))

int iself(unsigned char *input);

typedef void (*comp_func_ptr) (char *, int, char *, int *);
typedef enum { CBFS_COMPRESS_NONE = 0, CBFS_COMPRESS_LZMA = 1 } comp_algo;

comp_func_ptr compression_function(comp_algo algo);

uint64_t intfiletype(const char *name);

/* cbfs-mkpayload.c */
int parse_elf_to_payload(const struct buffer *input,
			 struct buffer *output, comp_algo algo);
int parse_fv_to_payload(const struct buffer *input,
			 struct buffer *output, comp_algo algo);
int parse_bzImage_to_payload(const struct buffer *input,
			     struct buffer *output, const char *initrd,
			     char *cmdline, comp_algo algo);
int parse_flat_binary_to_payload(const struct buffer *input,
				 struct buffer *output,
				 uint32_t loadaddress,
				 uint32_t entrypoint,
				 comp_algo algo);
/* cbfs-mkstage.c */
int parse_elf_to_stage(const struct buffer *input, struct buffer *output,
		       comp_algo algo, uint32_t *location);

void print_supported_filetypes(void);

#define ARRAY_SIZE(a) (int)(sizeof(a) / sizeof((a)[0]))
/* lzma/lzma.c */
void do_lzma_compress(char *in, int in_len, char *out, int *out_len);
void do_lzma_uncompress(char *dst, int dst_len, char *src, int src_len);
/* xdr.c */
struct xdr {
	uint16_t (*get16)(struct buffer *input);
	uint32_t (*get32)(struct buffer *input);
	uint64_t (*get64)(struct buffer *input);
	void (*put16)(struct buffer *input, uint16_t val);
	void (*put32)(struct buffer *input, uint32_t val);
	void (*put64)(struct buffer *input, uint64_t val);
};

/* xdr.c */
extern struct xdr xdr_le, xdr_be;

#endif
