#ifndef _DTC_H
#define _DTC_H

/*
 * (C) Copyright David Gibson <dwg@au1.ibm.com>, IBM Corporation.  2005.
 *
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *                                                                       
 *  You should have received a copy of the GNU General Public License    
 *  along with this program; if not, write to the Free Software          
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 *                                                                   USA 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>

/* also covers (part of?) linux's byteswap.h functionality */
#include "endian.h"

#include "flat_dt.h"

static inline void die(char * str, ...)
{
	va_list ap;

	va_start(ap, str);
	fprintf(stderr, "FATAL ERROR: ");
	vfprintf(stderr, str, ap);
	exit(1);
}

static inline void *xmalloc(size_t len)
{
	void *new = malloc(len);

	if (! new)
		die("malloc() failed\n");

	return new;
}

static inline void *xrealloc(void *p, size_t len)
{
	void *new = realloc(p, len);

	if (! new)
		die("realloc() failed (len=%d)\n", len);

	return new;
}

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef u32 cell_t;

#define cpu_to_be16(x)	htons(x)
#define be16_to_cpu(x)	ntohs(x)

#define cpu_to_be32(x)	htonl(x)
#define be32_to_cpu(x)	ntohl(x)

#if __BYTE_ORDER == __BIG_ENDIAN
#define cpu_to_be64(x)	(x)
#define be64_to_cpu(x)	(x)
#else
#define cpu_to_be64(x)	bswap_64(x)
#define be64_to_cpu(x)	bswap_64(x)
#endif

#define streq(a, b)	(strcmp((char *)(a), (char *)(b)) == 0)
#define strneq(a, b, n)	(strncmp((char *)(a), (char *)(b), (n)) == 0)

#define DALIGN(x, a)	(((x) + (a) - 1) & ~((a) - 1))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* Data blobs */
struct fixup {
	int offset;
	char *ref;
	struct fixup *next;
};

struct data {
	int len;
	unsigned char *val;
	int asize;
	struct fixup *refs;
};

#define empty_data \
	((struct data){.len = 0, .val = NULL, .asize = 0, .refs = NULL})

void fixup_free(struct fixup *f);
void data_free(struct data d);

struct data data_grow_for(struct data d, int xlen);

struct data data_copy_mem(char *mem, int len);
struct data data_copy_escape_string(char *s, int len);
struct data data_copy_file(FILE *f, size_t len);

struct data data_append_data(struct data d, void *p, int len);
struct data data_append_cell(struct data d, cell_t word);
struct data data_append_re(struct data d, struct reserve_entry *re);
struct data data_append_addr(struct data d, u64 addr);
struct data data_append_byte(struct data d, uint8_t byte);
struct data data_append_zeroes(struct data d, int len);
struct data data_append_align(struct data d, int align);

struct data data_add_fixup(struct data d, char *ref);

int data_is_one_string(struct data d);

/* DT constraints */

#define MAX_PROPNAME_LEN	63
#define MAX_NODENAME_LEN	63

/* Live trees */
struct property {
	char *name;
	struct data val;

	struct property *next;

	char *label;
};

struct node {
	char *name;
	struct property *proplist;
	struct node *children;
	struct property *config; 

	struct node *parent;
	struct node *next_sibling;
	struct node *next;

	char *fullpath;
	int basenamelen;

	cell_t phandle;
	int addr_cells, size_cells;

	char *label;
};

#define for_each_property(n, p) \
	for ((p) = (n)->proplist; (p); (p) = (p)->next)

#define for_each_child(n, c)	\
	for ((c) = (n)->children; (c); (c) = (c)->next_sibling)

#define for_each_config(n, p)	\
	for ((p) = (n)->config; (p); (p) = (p)->next)

struct property *build_property(char *name, struct data val, char *label);
struct property *chain_property(struct property *first, struct property *list);

struct node *build_node(struct property *config, struct property *proplist, struct node *children);
struct node *name_node(struct node *node, char *name, char *label);
struct node *chain_node(struct node *first, struct node *list);

void add_property(struct node *node, struct property *prop);
void add_child(struct node *parent, struct node *child);

int check_device_tree(struct node *dt, int outversion, int boot_cpuid_phys);

/* Boot info (tree plus memreserve information */

struct reserve_info {
	struct reserve_entry re;

	struct reserve_info *next;

	char *label;
};

struct reserve_info *build_reserve_entry(u64 start, u64 len, char *label);
struct reserve_info *chain_reserve_entry(struct reserve_info *first,
					 struct reserve_info *list);
struct reserve_info *add_reserve_entry(struct reserve_info *list,
				       struct reserve_info *new);


struct boot_info {
	struct reserve_info *reservelist;
	struct node *dt;		/* the device tree */
};

struct boot_info *build_boot_info(struct reserve_info *reservelist,
				  struct node *tree);

/* Flattened trees */

void dt_to_blob(FILE *f, struct boot_info *bi, int version,
		int boot_cpuid_phys);
void dt_to_asm(FILE *f, struct boot_info *bi, int version,
	       int boot_cpuid_phys);
void dt_to_C(FILE *f, struct boot_info *bi, int version, int boot_cpuid_phys);
void dt_to_coreboot(FILE *f, struct boot_info *bi, int version, int boot_cpuid_phys);
void dt_to_corebooth(FILE *f, struct boot_info *bi, int version, int boot_cpuid_phys);

struct boot_info *dt_from_blob(FILE *f);

/* Tree source */

void dt_to_source(FILE *f, struct boot_info *bi);
struct boot_info *dt_from_source(FILE *f);

/* FS trees */

struct boot_info *dt_from_fs(char *dirname);

/* misc */

char *join_path(char *path, char *name);
void fill_fullpaths(struct node *tree, char *prefix);

#endif /* _DTC_H */
