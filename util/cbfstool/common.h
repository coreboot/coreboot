/*
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
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

#include <stdint.h>
#ifndef WIN32
#include <arpa/inet.h>
#else
#define ntohl(x) (((x)>>24) | ((x)<<24) | (((x)>>8)&0xff00) | (((x)<<8)&0xff0000))
#define htonl ntohl
#endif

extern void *offset;
extern struct cbfs_header *master_header;
extern uint32_t phys_start, phys_end, align, romsize;

static void *phys_to_virt(uint32_t addr)
{
	return offset + addr;
}

static uint32_t virt_to_phys(void *addr)
{
	return (unsigned long)(addr - offset) & 0xffffffff;
}

#define ALIGN(val, by) (((val) + (by)-1)&~((by)-1))

uint32_t getfilesize(const char *filename);
void *loadfile(const char *filename, uint32_t * romsize_p, void *content,
	       int place);
void *loadrom(const char *filename);
int writerom(const char *filename, void *start, uint32_t size);

int iself(unsigned char *input);

typedef void (*comp_func_ptr) (char *, int, char *, int *);
typedef enum { CBFS_COMPRESS_NONE = 0, CBFS_COMPRESS_LZMA = 1 } comp_algo;

comp_func_ptr compression_function(comp_algo algo);

uint64_t intfiletype(const char *name);

int parse_elf_to_payload(unsigned char *input, unsigned char **output,
			 comp_algo algo);
int parse_elf_to_stage(unsigned char *input, unsigned char **output,
		       comp_algo algo, uint32_t * location);

void *create_cbfs_file(const char *filename, void *data, uint32_t * datasize,
		       uint32_t type, uint32_t * location);

int create_cbfs_image(const char *romfile, uint32_t romsize,
		      const char *bootblock, uint32_t align);

int add_file_to_cbfs(void *content, uint32_t contentsize, uint32_t location);
void print_cbfs_directory(const char *filename);
int extract_file_from_cbfs(const char *filename, const char *payloadname, const char *outpath);

uint32_t cbfs_find_location(const char *romfile, uint32_t filesize,
			    const char *filename, uint32_t align);

void print_supported_filetypes(void);

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
