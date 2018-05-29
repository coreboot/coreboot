/*
 * bincfg - Compiler/Decompiler for data blobs with specs
 * Copyright (C) 2017 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2018 Denis 'GNUtoo' Carikli <GNUtoo@no-log.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __BINCFG_H
#define __BINCFG_H

#define VALID_BIT 0x80
#define MAX_WIDTH 32
#define CHECKSUM_SIZE 16

struct field {
	char *name;
	unsigned int width;
	unsigned int value;
	struct field *next;
};

/* Bit array intermediary representation */
struct blob {
	unsigned int bloblen;
	unsigned char *blb;
	unsigned short checksum;
	unsigned char *actualblob;
	unsigned int lenactualblob;
};

static struct field *putsym (char const *, unsigned int);
static struct field *getsym (char const *);
static void yyerror (FILE* fp, char const *);
int yylex (void);

static struct blob *binary;
static struct field *sym_table;
static struct field *sym_table_tail;

#endif /* __BINCFG_H */
