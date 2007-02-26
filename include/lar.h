/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH
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
 *
 * This file may be dual licensed with the new BSD license.
 */

#include <stdint.h>

#define MAGIC "LARCHIVE"
#define MAX_PATHLEN 1024

struct lar_header {
	char magic[8];
	u32 len;
	u32 checksum;
	u32 offset;
};

struct mem_file {
	void *start;
	int len;
};

/* prototypes */
int find_file(struct mem_file *archive, char *filename, struct mem_file *result);
int copy_file(struct mem_file *archive, char *filename, void *where);
int run_file(struct mem_file *archive, char *filename, void *where);
