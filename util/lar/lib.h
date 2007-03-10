/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006-2007 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
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

#ifndef __LAR_LIB_H
#define __LAR_LIB_H

/* data types */

struct file {
	char *name;
	struct file *next;
};

enum {
	NONE,
	CREATE,
	LIST,
	EXTRACT
} larmodes;

/* prototypes for lar.c functions */
int verbose(void);
long get_larsize(void);
char *get_bootblock(void);

/* prototypes for lib.c functions */
int mkdirp(const char *dirpath);

int add_files(const char *name);
int add_file_or_directory(const char *name);

struct file *get_files(void);
void free_files(void);

/* prototypes for extract.c functions */
int extract_lar(const char *archivename, struct file *files);

/* prototypes for list.c functions */
int list_lar(const char *archivename, struct file *files);

/* prototypes for create.c functions */
int create_lar(const char *archivename, struct file *files);

#endif
