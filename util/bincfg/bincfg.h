/* bincfg - Compiler/Decompiler for data blobs with specs */
/* SPDX-License-Identifier: GPL-3.0-or-later */

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
