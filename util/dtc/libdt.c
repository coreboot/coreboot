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

#include <stdint.h>
#include <string.h>

#include "flat_dt.h"

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))
#define PALIGN(p, a)	((void *)(ALIGN((unsigned long)(p), (a))))
#define GET_CELL(p)	(p += 4, *((uint32_t *)(p-4)))

static char *skip_name(char *p)
{
	while (*p != '\0')
		p++;

	return PALIGN(p, sizeof(uint32_t));
}

static char *skip_prop(void *blob, char *p)
{
	struct boot_param_header *bph = blob;
	uint32_t len, nameoff;

	len = GET_CELL(p);
	nameoff = GET_CELL(p);
	if ((bph->version < 0x10) && (len >= sizeof(uint64_t)))
		p = PALIGN(p, sizeof(uint64_t));
	return PALIGN(p + len, sizeof(uint32_t));
}

static char *get_unit(char *dtpath)
{
	char *p;

	if (dtpath[0] != '/')
		return dtpath;

	p = dtpath + strlen(dtpath);
	while (*p != '/')
		p--;

	return p+1;
}

static int first_seg_len(char *dtpath)
{
	int len = 0;

	while ((dtpath[len] != '/') && (dtpath[len] != '\0'))
		len++;

	return len;
}

char *flat_dt_get_string(void *blob, uint32_t offset)
{
	struct boot_param_header *bph = blob;

	return (char *)blob + bph->off_dt_strings + offset;
}

void *flat_dt_get_subnode(void *blob, void *node, char *uname, int unamelen)
{
	struct boot_param_header *bph = blob;
	char *p = node;
	uint32_t tag;
	int depth = 0;
	char *nuname;

	if (! unamelen)
		unamelen = strlen(uname);

	do {
		tag = GET_CELL(p);

		switch (tag) {
		case OF_DT_PROP:
			p = skip_prop(blob, p);
			break;

		case OF_DT_BEGIN_NODE:
			if (depth == 0) {
				nuname = p;

				if (bph->version < 0x10)
					nuname = get_unit(nuname);

				p = skip_name(p);

				if (strncmp(nuname, uname, unamelen) == 0)
					return p;
			}
			depth++;
			break;

		case OF_DT_END_NODE:
			depth--;
			break;

		case OF_DT_END:
			/* looks like a malformed tree */
			return NULL;
			break;

		default:
			/* FIXME: throw some sort of error */
			return NULL;
		}
	} while (depth >= 0);

	return NULL;
}

void *flat_dt_get_node(void *blob, char *path)
{
	struct boot_param_header *bph = blob;
	char *node;
	int seglen;

	node = blob + bph->off_dt_struct;
	node += sizeof(uint32_t); /* skip initial OF_DT_BEGIN_NODE */
	node = skip_name(node);	/* skip root node name */

	while (node && (*path)) {
		if (path[0] == '/')
			path++;

		seglen = first_seg_len(path);

		node = flat_dt_get_subnode(blob, node, path, seglen);

		path += seglen;
	}

	return node;
}

void flat_dt_traverse(void *blob,
		      int (*fn)(void *blob, void *node, void *priv),
		      void *private)
{
	struct boot_param_header *bph = blob;
	char *p;
	uint32_t tag;
	int depth = 0;
	char *uname;

	p = (char *)blob + bph->off_dt_struct;

	tag = GET_CELL(p);
	while (tag != OF_DT_END) {
		switch (tag) {
		case OF_DT_BEGIN_NODE:
			uname = p;

			if (bph->version < 0x10)
				uname = get_unit(uname);

			p = skip_name(p);

			(*fn)(blob, p, private);
			depth++;
			break;

		case OF_DT_END_NODE:
			depth--;
			break;

		case OF_DT_PROP:
			p = skip_prop(blob, p);
			break;

		default:
			/* FIXME: badly formed tree */
			return;
		}
	}
}

void *flat_dt_get_prop(void *blob, void *node, char *name, uint32_t *len)
{
	struct boot_param_header *bph = blob;
	char *p = node;

	do {
		uint32_t tag = GET_CELL(p);
		uint32_t sz, noff;
		const char *nstr;

		if (tag != OF_DT_PROP)
			return NULL;

		sz = GET_CELL(p);
		noff = GET_CELL(p);

		/* Old versions have variable alignment of the
		 * property value */
		if ((bph->version < 0x10) && (sz >= 8))
		    p = PALIGN(p, 8);

		nstr = flat_dt_get_string(blob, noff);

		if (strcmp(name, nstr) == 0) {
			if (len)
				*len = sz;
			return (void *)p;
		}

		p = PALIGN(p + sz, sizeof(uint32_t));
	} while(1);
}
