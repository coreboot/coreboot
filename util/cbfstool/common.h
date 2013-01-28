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
#include "cbfs.h"

/* Message output */
extern int verbose;
#define ERROR(x...) { fprintf(stderr, "E: " x); }
#define WARN(x...) { fprintf(stderr, "W: " x); }
#define LOG(x...) { fprintf(stderr, x); }
#define INFO(x...) { if (verbose > 0) fprintf(stderr, "INFO: " x); }
#define DEBUG(x...) { if (verbose > 1) fprintf(stderr, "DEBUG: " x); }

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* Endianess */
#include <arpa/inet.h>	/* for ntohl, htonl */
#include "swab.h"
extern int is_big_endian(void);
#define ntohll(x)	(is_big_endian() ? (x) : swab64(x))
#define htonll(x)	(is_big_endian() ? (x) : swab64(x))

extern void *offset;
extern uint32_t romsize;
extern int host_bigendian;
extern uint32_t arch;

const char *arch_to_string(uint32_t a);
uint32_t string_to_arch(const char *arch_string);

static inline void *phys_to_virt(uint32_t addr)
{
	return offset + addr;
}

static inline uint32_t virt_to_phys(void *addr)
{
	return (unsigned long)(addr - offset) & 0xffffffff;
}

#define ALIGN(val, by) (((val) + (by)-1)&~((by)-1))

size_t getfilesize(const char *filename);
void *loadfile(const char *filename, uint32_t * romsize_p, void *content,
	       int place);
void *loadrom(const char *filename);
int writerom(const char *filename, void *start, uint32_t size);

int iself(unsigned char *input);

typedef void (*comp_func_ptr) (char *, int, char *, int *);
typedef enum { CBFS_COMPRESS_NONE = 0, CBFS_COMPRESS_LZMA = 1 } comp_algo;

comp_func_ptr compression_function(comp_algo algo);

uint64_t intfiletype(const char *name);

void *create_cbfs_file(const char *filename, void *data, uint32_t * datasize,
		       uint32_t type, uint32_t * location);

int create_cbfs_image(const char *romfile, uint32_t romsize,
		      const char *bootblock, uint32_t align, uint32_t offs);

int add_file_to_cbfs(void *content, uint32_t contentsize, uint32_t location);
int remove_file_from_cbfs(const char *filename);
void print_cbfs_directory(const char *filename);
int extract_file_from_cbfs(const char *filename, const char *payloadname, const char *outpath);
int remove_file_from_cbfs(const char *filename);

uint32_t cbfs_find_stage_location(const char *romfile, uint32_t filesize,
				  const char *filename, uint32_t align);

void print_supported_filetypes(void);

/* cbfs-mkstage.c */
int parse_elf_to_stage(unsigned char *input, unsigned char **output,
		       comp_algo algo, uint32_t * location);
/* cbfs-mkpayload.c */
int parse_elf_to_payload(unsigned char *input, unsigned char **output,
			 comp_algo algo);
int parse_flat_binary_to_payload(unsigned char *input, unsigned char **output,
				 int32_t input_size, uint32_t loadaddress,
				 uint32_t entrypoint, comp_algo algo);
#endif
