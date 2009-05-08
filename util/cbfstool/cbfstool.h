/*
 * cbfstool
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
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

#ifndef _ROMTOOL_H_
#define _ROMTOOL_H_

#include <stdio.h>
#include <arpa/inet.h>
#include "cbfs.h"

/* Definitions */

/* Structures */

struct rom {
	unsigned char *name;
	unsigned char *ptr;

	int fd;
	int size;
	int fssize;

	struct cbfs_header *header;
};

/* Macros */

#define ROM_OFFSET(_r, _c) ((unsigned int) ((unsigned char *) (_c) - (_r)->ptr))

#define ROM_PTR(_r, _o) ((_r)->ptr + (_o))
#define ROM_WRITEL(_r, _o, _v) do { *((unsigned int *) ROM_PTR((_r), (_o))) = (_v); } while(0)
#define ROM_READL(_r, _o) *((unsigned int *) (ROM_PTR((_r), (_o))))

#define ERROR(err, args...) fprintf(stderr, "(cbfstool) E: " err, ##args)
#define WARN(err, args...) fprintf(stderr, "(cbfstool) W: " err, ##args)
#define VERBOSE(str, args...) printf(str, ##args)

#define ALIGN(_v, _a) ( ( (_v) + ( (_a) - 1 ) ) & ~( (_a) - 1 ) )

/* Function prototypes */

/* util.c */
int open_rom(struct rom *rom, const char *filename);
int create_rom(struct rom *rom, const unsigned char *filename, int size,
	       const char *bootblockname, int bootblocksize,
	       int align);
int size_and_open(const char *filename, unsigned int *size);
int copy_from_fd(int fd, void *ptr, int size);
int get_size(const char *size);
int add_bootblock(struct rom *rom, const char *filename);

/* fs.c */

struct cbfs_file *rom_find(struct rom *rom, int offset);
struct cbfs_file *rom_find_first(struct rom *);
struct cbfs_file *rom_find_next(struct rom *, struct cbfs_file *);
int rom_add(struct rom *rom, const char *name, void *, int size, int type);
int rom_set_header(struct rom *rom, struct cbfs_file *c, 
	const char*name, int size, int type);
int rom_extract(struct rom *rom, const char *name, void **buf, int *size);
int rom_remove(struct rom *rom, const char *name);
int rom_used_space(struct rom *rom);
int rom_exists(struct rom *rom);

#endif
