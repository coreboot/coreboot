/* parser generator for flashmap descriptor language */
/* SPDX-License-Identifier: GPL-2.0-only */

%{
#include "fmd_scanner.h"
#include "common.h"

#include <stdlib.h>

struct flashmap_descriptor *res = NULL;
%}

%union {
	unsigned intval;
	char *strval;
	struct unsigned_option maybe_intval;
	struct flashmap_descriptor *region_ptr;
	union flashmap_flags flags;
	struct descriptor_list region_listhdr;
}

%code requires {
#include "fmd.h"
#include "option.h"

#include <stdbool.h>

struct descriptor_node {
	struct flashmap_descriptor *val;
	struct descriptor_node *next;
};

struct descriptor_list {
	size_t len;
	struct descriptor_node *head;
	struct descriptor_node *tail;
};

extern struct flashmap_descriptor *res;

struct flashmap_descriptor *parse_descriptor(
	char *name, union flashmap_flags flags, struct unsigned_option offset,
	struct unsigned_option size, struct descriptor_list children);
void yyerror(const char *s);
}

%token <intval> INTEGER
%token OCTAL
%token <strval> STRING
%token FLAG_CBFS
%token FLAG_PRESERVE

%type <region_ptr> flash_region
%type <strval> region_name
%type <flags> region_flags_opt
%type <flags> region_flags
%type <flags> region_flag
%type <maybe_intval> region_offset_opt
%type <maybe_intval> region_offset
%type <maybe_intval> region_size_opt
%type <maybe_intval> region_size
%type <region_listhdr> region_list_opt
%type <region_listhdr> region_list
%type <region_listhdr> region_list_entries

%%

flash_chip: region_name region_offset_opt region_size region_list
{
	union flashmap_flags flags = { .v=0 };
	if (!(res = parse_descriptor($1, flags, $2, $3, $4)))
		YYABORT;
};
flash_region: region_name region_flags_opt region_offset_opt region_size_opt
	      region_list_opt
{
	struct flashmap_descriptor *node = parse_descriptor($1, $2, $3, $4, $5);
	if (!node)
		YYABORT;

	if (node->flags.f.cbfs && !fmd_process_flag_cbfs(node)) {
		ERROR("Section '%s' cannot have flag 'CBFS''\n", node->name);
		YYABORT;
	}

	$$ = node;
};
region_name: STRING
{
	if (!$1) {
		perror("E: While allocating section name");
		YYABORT;
	}
};
region_flags_opt: { $$ = (union flashmap_flags){ .v=0 }; }
	| '(' region_flags ')' { $$ = $2; };
region_flags: region_flag | region_flag region_flags { $$.v = $1.v | $2.v; };
region_flag: FLAG_CBFS { $$.v = 0; $$.f.cbfs = 1; };
region_flag: FLAG_PRESERVE { $$.v = 0; $$.f.preserve = 1; };
region_offset_opt: { $$ = (struct unsigned_option){false, 0}; }
	| region_offset;
region_offset: '@' INTEGER { $$ = (struct unsigned_option){true, $2}; };
region_size_opt: { $$ = (struct unsigned_option){false, 0}; }
	| region_size;
region_size: INTEGER { $$ = (struct unsigned_option){true, $1}; };
region_list_opt:
{
	$$ = (struct descriptor_list)
					{.len = 0, .head = NULL, .tail = NULL};
}
	| region_list;
region_list: '{' region_list_entries '}' { $$ = $2; };
region_list_entries: flash_region
{
	struct descriptor_node *node = malloc(sizeof(*node));
	if (!node) {
		perror("E: While allocating linked list node");
		YYABORT;
	}
	node->val = $1;
	node->next = NULL;
	$$ = (struct descriptor_list){.len = 1, .head = node, .tail = node};
}
	| region_list_entries flash_region
{
	struct descriptor_node *node = malloc(sizeof(*node));
	if (!node) {
		perror("E: While allocating linked list node");
		YYABORT;
	}
	node->val = $2;
	node->next = NULL;

	$1.tail->next = node;
	$$ = (struct descriptor_list)
			{.len = $1.len + 1, .head = $1.head, .tail = node};
};

%%

struct flashmap_descriptor *parse_descriptor(
	char *name, union flashmap_flags flags, struct unsigned_option offset,
	struct unsigned_option size, struct descriptor_list children)
{
	struct flashmap_descriptor *region = malloc(sizeof(*region));
	if (!region) {
		perror("E: While allocating descriptor section");
		return NULL;
	}
	region->name = name;
	region->flags = flags;
	region->offset_known = offset.val_known;
	region->offset = offset.val;
	region->size_known = size.val_known;
	region->size = size.val;
	region->list_len = children.len;
	if (region->list_len) {
		region->list = malloc(region->list_len * sizeof(*region->list));
		if (!region->list) {
			perror("E: While allocating node children array");
			return NULL;
		}
		struct descriptor_node *cur_node = children.head;
		for (unsigned idx = 0; idx < region->list_len; ++idx) {
			region->list[idx] = cur_node->val;

			struct descriptor_node *next_node = cur_node->next;
			free(cur_node);
			cur_node = next_node;
		}
	} else {
		region->list = NULL;
	}
	return region;
}

void yyerror(const char *s)
{
	fprintf(stderr, "%s\n", s);
}
