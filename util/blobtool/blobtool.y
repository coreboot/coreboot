/*
 * blobtool - Compiler/Decompiler for data blobs with specs
 * Copyright (C) 2017 Damien Zammit <damien@zamaudio.com>
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

%{
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
//#define YYDEBUG 1
int yylex (void);
void yyerror (char const *);

struct field {
	char *name;
	unsigned int width;
	unsigned int value;
	struct field *next;
};

extern struct field *sym_table;
struct field *putsym (char const *, unsigned int);
struct field *getsym (char const *);

struct field *sym_table;
struct field *sym_table_tail;

FILE* fp;

/* Bit array intermediary representation */
struct blob {
	unsigned int bloblen;
	unsigned char *blb;
	unsigned short checksum;
	unsigned char *actualblob;
	unsigned int lenactualblob;
};

#define VALID_BIT 0x80
#define MAX_WIDTH 32
#define CHECKSUM_SIZE 16

struct blob *binary;

unsigned char* value_to_bits (unsigned int v, unsigned int w)
{
	unsigned int i;
	unsigned char* bitarr;

	if (w > MAX_WIDTH) w = MAX_WIDTH;
	bitarr = (unsigned char *) malloc (w * sizeof (unsigned char));
	memset (bitarr, 0, w);

	for (i = 0; i < w; i++) {
		bitarr[i] = VALID_BIT | ((v & (1 << i)) >> i);
	}
	return bitarr;
}

/* Store each bit of a bitfield in a new byte sequentially 0x80 or 0x81 */
void append_field_to_blob (unsigned char b[], unsigned int w)
{
	unsigned int i, j;
	binary->blb = (unsigned char *) realloc (binary->blb, binary->bloblen + w);
	for (j = 0, i = binary->bloblen; i < binary->bloblen + w; i++, j++) {
		binary->blb[i] = VALID_BIT | (b[j] & 1);
		//fprintf (stderr, "blob[%d] = %d\n", i, binary->blb[i] & 1);
	}
	binary->bloblen += w;
}

void set_bitfield(char *name, unsigned int value)
{
	unsigned long long i;
	struct field *bf = getsym (name);
	if (bf) {
		bf->value = value & 0xffffffff;
		i = (1 << bf->width) - 1;
		if (bf->width > 8 * sizeof (unsigned int)) {
			fprintf(stderr, "Overflow in bitfield, truncating bits to fit\n");
			bf->value = value & i;
		}
		//fprintf(stderr, "Setting `%s` = %d\n", bf->name, bf->value);
	} else {
		fprintf(stderr, "Can't find bitfield `%s` in spec\n", name);
	}
}

void set_bitfield_array(char *name, unsigned int n, unsigned int value)
{
	unsigned int i;
	unsigned int len = strlen (name);
	char *namen = (char *) malloc ((len + 9) * sizeof (char));
	for (i = 0; i < n; i++) {
		snprintf (namen, len + 8, "%s%x", name, i);
		set_bitfield (namen, value);
	}
	free(namen);
}

void create_new_bitfield(char *name, unsigned int width)
{
	struct field *bf;

	if (!(bf = putsym (name, width))) return;
	//fprintf(stderr, "Added bitfield `%s` : %d\n", bf->name, width);
}

void create_new_bitfields(char *name, unsigned int n, unsigned int width)
{
	unsigned int i;
	unsigned int len = strlen (name);
	char *namen = (char *) malloc ((len + 9) * sizeof (char));
	for (i = 0; i < n; i++) {
		snprintf (namen, len + 8, "%s%x", name, i);
		create_new_bitfield (namen, width);
	}
	free(namen);
}

struct field *putsym (char const *sym_name, unsigned int w)
{
	if (getsym(sym_name)) {
		fprintf(stderr, "Cannot add duplicate named bitfield `%s`\n", sym_name);
		return 0;
	}
	struct field *ptr = (struct field *) malloc (sizeof (struct field));
	ptr->name = (char *) malloc (strlen (sym_name) + 1);
	strcpy (ptr->name, sym_name);
	ptr->width = w;
	ptr->value = 0;
	ptr->next = (struct field *)0;
	if (sym_table_tail) {
		sym_table_tail->next = ptr;
	} else {
		sym_table = ptr;
	}
	sym_table_tail = ptr;
	return ptr;
}

struct field *getsym (char const *sym_name)
{
	struct field *ptr;
	for (ptr = sym_table; ptr != (struct field *) 0;
			ptr = (struct field *)ptr->next) {
		if (strcmp (ptr->name, sym_name) == 0)
			return ptr;
	}
	return 0;
}

void dump_all_values (void)
{
	struct field *ptr;
	for (ptr = sym_table; ptr != (struct field *) 0;
			ptr = (struct field *)ptr->next) {
		fprintf(stderr, "%s = %d (%d bits)\n",
				ptr->name,
				ptr->value,
				ptr->width);
	}
}

void empty_field_table(void)
{
	struct field *ptr;
	struct field *ptrnext;

	for (ptr = sym_table; ptr != (struct field *) 0; ptr = ptrnext) {
		if (ptr) {
			ptrnext = ptr->next;
			free(ptr);
		} else {
			ptrnext = (struct field *) 0;
		}
	}
	sym_table = 0;
	sym_table_tail = 0;
}

void create_binary_blob (void)
{
	if (binary && binary->blb) {
		free(binary->blb);
		free(binary);
	}
	binary = (struct blob *) malloc (sizeof (struct blob));
	binary->blb = (unsigned char *) malloc (sizeof (unsigned char));
	binary->bloblen = 0;
	binary->blb[0] = VALID_BIT;
}

void interpret_next_blob_value (struct field *f)
{
	int i;
	unsigned int v = 0;

	if (binary->bloblen >= binary->lenactualblob * 8) {
		f->value = 0;
		return;
	}

	for (i = 0; i < f->width; i++) {
		v |= (binary->blb[binary->bloblen++] & 1) << i;
	}

	f->value = v;
}

/* {}%BIN -> {} */
void generate_setter_bitfields(unsigned char *bin)
{
	unsigned int i;
	struct field *ptr;

	/* Convert bytes to bit array */
	for (i = 0; i < binary->lenactualblob; i++) {
		append_field_to_blob (value_to_bits(bin[i], 8), 8);
	}

	/* Reset blob position to zero */
	binary->bloblen = 0;

	fprintf (fp, "# AUTOGENERATED SETTER BY BLOBTOOL\n{\n");

	/* Traverse spec and output bitfield setters based on blob values */
	for (ptr = sym_table; ptr != (struct field *) 0; ptr = ptr->next) {

		interpret_next_blob_value(ptr);
		fprintf (fp, "\t\"%s\" = 0x%x,\n", ptr->name, ptr->value);
	}
	fseek(fp, -2, SEEK_CUR);
	fprintf (fp, "\n}\n");
}

void generate_binary_with_gbe_checksum(void)
{
	int i;
	unsigned short checksum;

	/* traverse spec, push to blob and add up for checksum */
	struct field *ptr;
	unsigned int uptochksum = 0;
	for (ptr = sym_table; ptr != (struct field *) 0; ptr = ptr->next) {
		if (strcmp (ptr->name, "checksum_gbe") == 0) {
			/* Stop traversing because we hit checksum */
			ptr = ptr->next;
			break;
		}
		append_field_to_blob (
			value_to_bits(ptr->value, ptr->width),
			ptr->width);
		uptochksum += ptr->width;
	}

	/* deserialize bits of blob up to checksum */
	for (i = 0; i < uptochksum; i += 8) {
		unsigned char byte = (((binary->blb[i+0] & 1) << 0)
					| ((binary->blb[i+1] & 1) << 1)
					| ((binary->blb[i+2] & 1) << 2)
					| ((binary->blb[i+3] & 1) << 3)
					| ((binary->blb[i+4] & 1) << 4)
					| ((binary->blb[i+5] & 1) << 5)
					| ((binary->blb[i+6] & 1) << 6)
					| ((binary->blb[i+7] & 1) << 7)
		);
		fprintf(fp, "%c", byte);

		/* incremental 16 bit checksum */
		if ((i % 16) < 8) {
			binary->checksum += byte;
		} else {
			binary->checksum += byte << 8;
		}
	}

	checksum = (0xbaba - binary->checksum) & 0xffff;

	/* Now write checksum */
	set_bitfield ("checksum_gbe", checksum);

	fprintf(fp, "%c", checksum & 0xff);
	fprintf(fp, "%c", (checksum & 0xff00) >> 8);

	append_field_to_blob (value_to_bits(checksum, 16), 16);

	for (; ptr != (struct field *) 0; ptr = ptr->next) {
		append_field_to_blob (
			value_to_bits(ptr->value, ptr->width), ptr->width);
	}

	/* deserialize rest of blob past checksum */
	for (i = uptochksum + CHECKSUM_SIZE; i < binary->bloblen; i += 8) {
		unsigned char byte = (((binary->blb[i+0] & 1) << 0)
					| ((binary->blb[i+1] & 1) << 1)
					| ((binary->blb[i+2] & 1) << 2)
					| ((binary->blb[i+3] & 1) << 3)
					| ((binary->blb[i+4] & 1) << 4)
					| ((binary->blb[i+5] & 1) << 5)
					| ((binary->blb[i+6] & 1) << 6)
					| ((binary->blb[i+7] & 1) << 7)
		);
		fprintf(fp, "%c", byte);
	}
}

/* {}{} -> BIN */
void generate_binary(void)
{
	unsigned int i;
	struct field *ptr;

	if (binary->bloblen % 8) {
		fprintf (stderr, "ERROR: Spec must be multiple of 8 bits wide\n");
		exit (1);
	}

	if (getsym ("checksum_gbe")) {
		generate_binary_with_gbe_checksum();
		return;
	}

	/* traverse spec, push to blob */
	for (ptr = sym_table; ptr != (struct field *) 0; ptr = ptr->next) {
		append_field_to_blob (
			value_to_bits(ptr->value, ptr->width),
			ptr->width);
	}

	/* deserialize bits of blob */
	for (i = 0; i < binary->bloblen; i += 8) {
		unsigned char byte = (((binary->blb[i+0] & 1) << 0)
				| ((binary->blb[i+1] & 1) << 1)
				| ((binary->blb[i+2] & 1) << 2)
				| ((binary->blb[i+3] & 1) << 3)
				| ((binary->blb[i+4] & 1) << 4)
				| ((binary->blb[i+5] & 1) << 5)
				| ((binary->blb[i+6] & 1) << 6)
				| ((binary->blb[i+7] & 1) << 7)
		);
		fprintf(fp, "%c", byte);
	}
}

%}

%union
{
	char *str;
	unsigned int u32;
	unsigned int *u32array;
	unsigned char u8;
	unsigned char *u8array;
}

%token <str> name
%token <u32> val
%token <u32array> vals
%token <u8> hexbyte
%token <u8array> binblob
%token <u8> eof

%left '%'
%left '{' '}'
%left ','
%left ':'
%left '='

%%

input:
  /* empty */
| input spec setter eof		{ empty_field_table(); YYACCEPT;}
| input spec blob		{ fprintf (stderr, "Parsed all bytes\n");
				  empty_field_table(); YYACCEPT;}
;

blob:
  '%' eof			{ generate_setter_bitfields(binary->actualblob); }
;

spec:
  '{' '}'		{	fprintf (stderr, "No spec\n"); }
| '{' specmembers '}'	{	fprintf (stderr, "Parsed all spec\n");
				create_binary_blob(); }
;

specmembers:
  specpair
| specpair ',' specmembers
;

specpair:
  name ':' val		{	create_new_bitfield($1, $3); }
| name '[' val ']' ':' val	{ create_new_bitfields($1, $3, $6); }
;

setter:
  '{' '}'		{	fprintf (stderr, "No values\n"); }
| '{' valuemembers '}'	{	fprintf (stderr, "Parsed all values\n");
				generate_binary(); }
;

valuemembers:
  setpair
| setpair ',' valuemembers
;

setpair:
  name '=' val		{	set_bitfield($1, $3); }
| name '[' val ']' '=' val	{ set_bitfield_array($1, $3, $6); }
;

%%

/* Called by yyparse on error.  */
void yyerror (char const *s)
{
	fprintf (stderr, "yyerror: %s\n", s);
}

/* Declarations */
void set_input_string(char* in);

/* This function parses a string */
int parse_string(unsigned char* in) {
	set_input_string ((char *)in);
	return yyparse ();
}

int main (int argc, char *argv[])
{
	unsigned int lenspec, lensetter;
	unsigned char *parsestring;
	unsigned char c;
	unsigned int pos = 0;
	int ret = 0;

#if YYDEBUG == 1
	yydebug = 1;
#endif
	create_binary_blob();
	binary->lenactualblob = 0;

	if (argc == 4 && strcmp(argv[1], "-d") != 0) {
		/* Compile mode */

		/* Load Spec */
		fp = fopen(argv[1], "r");
		fseek(fp, 0, SEEK_END);
		lenspec = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		parsestring = (unsigned char *) malloc (lenspec);
		if (!parsestring) {
			printf("Out of memory\n");
			exit(1);
		}
		fread(parsestring, 1, lenspec, fp);
		fclose(fp);

		/* Load Setter */
		fp = fopen(argv[2], "r");
		fseek(fp, 0, SEEK_END);
		lensetter = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		parsestring = (unsigned char *) realloc (parsestring,
							lenspec + lensetter);
		if (!parsestring) {
			printf("Out of memory\n");
			exit(1);
		}
		fread(parsestring + lenspec, 1, lensetter, fp);
		fclose(fp);

		/* Open output and parse string - output to fp */
		fp = fopen(argv[3], "wb");
		ret = parse_string(parsestring);
		free(parsestring);
	} else if (argc == 5 && strcmp (argv[1], "-d") == 0) {
		/* Decompile mode */

		/* Load Spec */
		fp = fopen(argv[2], "r");
		fseek(fp, 0, SEEK_END);
		lenspec = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		parsestring = (unsigned char *) malloc (lenspec + 1);
		fread(parsestring, 1, lenspec, fp);
		if (!parsestring) {
			printf("Out of memory\n");
			exit(1);
		}
		fclose(fp);

		/* Add binary read trigger token */
		parsestring[lenspec] = '%';

		/* Load Actual Binary */
		fp = fopen(argv[3], "rb");
		fseek(fp, 0, SEEK_END);
		binary->lenactualblob = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		binary->actualblob = (unsigned char *) malloc (binary->lenactualblob);
		if (!binary->actualblob) {
			printf("Out of memory\n");
			exit(1);
		}
		fread(binary->actualblob, 1, binary->lenactualblob, fp);
		fclose(fp);

		/* Open output and parse - output to fp */
		fp = fopen(argv[4], "w");
		ret = parse_string(parsestring);
		free(parsestring);
		free(binary->actualblob);
		fclose(fp);
	} else {
		printf("Usage: Compile mode\n\n");
		printf("       blobtool    spec  setter  binaryoutput\n");
		printf("                  (file) (file)     (file)\n");
		printf(" OR  : Decompile mode\n\n");
		printf("       blobtool -d spec  binary  setteroutput\n");
	}
	return ret;
}
