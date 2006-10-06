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

#include "dtc.h"

void fixup_free(struct fixup *f)
{
	free(f->ref);
	free(f);
}

void data_free(struct data d)
{
	struct fixup *f;

	f = d.refs;
	while (f) {
		struct fixup *nf;

		nf = f->next;
		fixup_free(f);
		f = nf;
	}

	assert(!d.val || d.asize);

	if (d.val)
		free(d.val);
}

struct data data_grow_for(struct data d, int xlen)
{
	struct data nd;
	int newsize;

	/* we must start with an allocated datum */
	assert(!d.val || d.asize);

	if (xlen == 0)
		return d;

	newsize = xlen;

	while ((d.len + xlen) > newsize)
		newsize *= 2;

	nd.asize = newsize;
	nd.val = xrealloc(d.val, newsize);
	nd.len = d.len;
	nd.refs = d.refs;

	assert(nd.asize >= (d.len + xlen));

	return nd;
}

struct data data_copy_mem(char *mem, int len)
{
	struct data d;

	d = data_grow_for(empty_data, len);

	d.len = len;
	memcpy(d.val, mem, len);

	return d;
}

static char get_oct_char(char *s, int *i)
{
	char x[4];
	char *endx;
	long val;

	x[3] = '\0';
	x[0] = s[(*i)];
	if (x[0]) {
		x[1] = s[(*i)+1];
		if (x[1])
			x[2] = s[(*i)+2];
	}

	val = strtol(x, &endx, 8);
	if ((endx - x) == 0)
		fprintf(stderr, "Empty \\nnn escape\n");
		
	(*i) += endx - x;
	return val;
}

static char get_hex_char(char *s, int *i)
{
	char x[3];
	char *endx;
	long val;

	x[2] = '\0';
	x[0] = s[(*i)];
	if (x[0])
		x[1] = s[(*i)+1];

	val = strtol(x, &endx, 16);
	if ((endx - x) == 0)
		fprintf(stderr, "Empty \\x escape\n");
		
	(*i) += endx - x;
	return val;
}

struct data data_copy_escape_string(char *s, int len)
{
	int i = 0;
	struct data d;
	char *q;

	d = data_grow_for(empty_data, strlen(s)+1);

	q = d.val;
	while (i < len) {
		char c = s[i++];

		if (c != '\\') {
			q[d.len++] = c;
			continue;
		}

		c = s[i++];
		assert(c);
		switch (c) {
		case 't':
			q[d.len++] = '\t';
			break;
		case 'n':
			q[d.len++] = '\n';
			break;
		case 'r':
			q[d.len++] = '\r';
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			i--; /* need to re-read the first digit as
			      * part of the octal value */
			q[d.len++] = get_oct_char(s, &i);
			break;
		case 'x':
			q[d.len++] = get_hex_char(s, &i);
			break;
		default:
			q[d.len++] = c;
		}
	}

	q[d.len++] = '\0';
	return d;
}

struct data data_copy_file(FILE *f, size_t len)
{
	struct data d;

	d = data_grow_for(empty_data, len);

	d.len = len;
	fread(d.val, len, 1, f);

	return d;
}

struct data data_append_data(struct data d, void *p, int len)
{
	d = data_grow_for(d, len);
	memcpy(d.val + d.len, p, len);
	d.len += len;
	return d;
}

struct data data_append_cell(struct data d, cell_t word)
{
	cell_t beword = cpu_to_be32(word);

	return data_append_data(d, &beword, sizeof(beword));
}

struct data data_append_re(struct data d, struct reserve_entry *re)
{
	struct reserve_entry bere;

	bere.address = cpu_to_be64(re->address);
	bere.size = cpu_to_be64(re->size);

	return data_append_data(d, &bere, sizeof(bere));
}

struct data data_append_addr(struct data d, u64 addr)
{
	u64 beaddr = cpu_to_be64(addr);

	return data_append_data(d, &beaddr, sizeof(beaddr));
}

struct data data_append_byte(struct data d, uint8_t byte)
{
	return data_append_data(d, &byte, 1);
}

struct data data_append_zeroes(struct data d, int len)
{
	d = data_grow_for(d, len);

	memset(d.val + d.len, 0, len);
	d.len += len;
	return d;
}

struct data data_append_align(struct data d, int align)
{
	int newlen = ALIGN(d.len, align);
	return data_append_zeroes(d, newlen - d.len);
}

struct data data_add_fixup(struct data d, char *ref)
{
	struct fixup *f;
	struct data nd;

	f = xmalloc(sizeof(*f));
	f->offset = d.len;
	f->ref = ref;
	f->next = d.refs;

	nd = d;
	nd.refs = f;

	return nd;
}

int data_is_one_string(struct data d)
{
	int i;
	int len = d.len;

	if (len == 0)
		return 0;

	for (i = 0; i < len-1; i++)
		if (d.val[i] == '\0')
			return 0;

	if (d.val[len-1] != '\0')
		return 0;

	return 1;
}
