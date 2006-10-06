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

extern FILE *yyin;
extern int yyparse(void);
extern void yyerror(char const *);

struct boot_info *the_boot_info;

struct boot_info *dt_from_source(FILE *f)
{
	the_boot_info = NULL;

	yyin = f;
	if (yyparse() != 0)
		return NULL;

	fill_fullpaths(the_boot_info->dt, "");

	return the_boot_info;
}

static void write_prefix(FILE *f, int level)
{
	int i;

	for (i = 0; i < level; i++)
		fputc('\t', f);
}

enum proptype {
	PROP_EMPTY,
	PROP_STRING,
	PROP_CELLS,
	PROP_BYTES,
};

static enum proptype guess_type(struct property *prop)
{
	int len = prop->val.len;
	char *p = prop->val.val;
	int nnoprint = 0;
	int i;

	if (len == 0)
		return PROP_EMPTY;

	for (i = 0; i < len; i++) {
		if (! isprint(p[i]))
			nnoprint++;
	}

	if ((nnoprint == 1) && (p[len-1] == '\0'))
		return PROP_STRING;
	else if ((len % sizeof(cell_t)) == 0)
		return PROP_CELLS;
	else
		return PROP_BYTES;
		
}


static void write_tree_source_node(FILE *f, struct node *tree, int level)
{
	struct property *prop;
	struct node *child;

	write_prefix(f, level);
	if (tree->name && (*tree->name))
		fprintf(f, "%s {\n", tree->name);
	else
		fprintf(f, "/ {\n");

	for_each_property(tree, prop) {
		cell_t *cp;
		char *bp;
		void *propend;
		enum proptype type;

		write_prefix(f, level);
		fprintf(f, "\t%s", prop->name);
		type = guess_type(prop);
		propend = prop->val.val + prop->val.len;

		switch (type) {
		case PROP_EMPTY:
			fprintf(f, ";\n");
			break;

		case PROP_STRING:
			fprintf(f, " = \"%s\";\n", (char *)prop->val.val);
			break;

		case PROP_CELLS:
			fprintf(f, " = <");
			cp = (cell_t *)prop->val.val;
			for (;;) {
				fprintf(f, "%x", be32_to_cpu(*cp++));
				if ((void *)cp >= propend)
					break;
				fprintf(f, " ");
			}
			fprintf(f, ">;\n");
			break;

		case PROP_BYTES:
			fprintf(f, " = [");
			bp = prop->val.val;
			for (;;) {
				fprintf(f, "%02hhx", *bp++);
				if ((void *)bp >= propend)
					break;
				fprintf(f, " ");
			}
			fprintf(f, "];\n");
			break;
		}
	}
	for_each_child(tree, child) {
		fprintf(f, "\n");
		write_tree_source_node(f, child, level+1);
	}
	write_prefix(f, level);
	fprintf(f, "};\n");
}


void dt_to_source(FILE *f, struct boot_info *bi)
{
	struct reserve_info *re;

	for (re = bi->reservelist; re; re = re->next) {
		fprintf(f, "/memreserve/\t%016llx-%016llx;\n",
			(unsigned long long)re->re.address,
			(unsigned long long)(re->re.address + re->re.size - 1));
	}

	write_tree_source_node(f, bi->dt, 0);
}

