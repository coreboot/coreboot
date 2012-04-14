/*****************************************************************************\
 * layout-text.c
 *****************************************************************************
 *  Copyright (C) 2012, Vikram Narayanan
 *  	Unified build_opt_tbl and nvramtool
 *  	build_opt_tbl.c
 *  	Copyright (C) 2003 Eric Biederman (ebiederm@xmission.com)
 *  	Copyright (C) 2007-2010 coresystems GmbH
 *
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by Dave Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
 *  UCRL-CODE-2003-012
 *  All rights reserved.
 *
 *  This file is part of nvramtool, a utility for reading/writing coreboot
 *  parameters and displaying information from the coreboot table.
 *  For details, see http://coreboot.org/nvramtool.
 *
 *  Please also read the file DISCLAIMER which is included in this software
 *  distribution.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License (as published by the
 *  Free Software Foundation) version 2, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the terms and
 *  conditions of the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
\*****************************************************************************/

#include "common.h"
#include "layout-text.h"
#include "layout.h"
#include "cmos_lowlevel.h"
#include "reg_expr.h"

static void process_layout_file(FILE * f);
static void skip_past_start(FILE * f);
static int process_entry(FILE * f, int skip_add);
static int process_enum(FILE * f, int skip_add);
static void process_checksum_info(FILE * f);
static void skip_remaining_lines(FILE * f);
static void create_entry(cmos_entry_t * cmos_entry,
			 const char start_bit_str[], const char length_str[],
			 const char config_str[], const char config_id_str[],
			 const char name_str[]);
static void try_add_layout_file_entry(const cmos_entry_t * cmos_entry);
static void create_enum(cmos_enum_t * cmos_enum, const char id_str[],
			const char value_str[], const char text_str[]);
static void try_add_cmos_enum(const cmos_enum_t * cmos_enum);
static void set_checksum_info(const char start_str[], const char end_str[],
			      const char index_str[]);
static char cmos_entry_char_value(cmos_entry_config_t config);
static int get_layout_file_line(FILE * f, char line[], int line_buf_size);
static unsigned string_to_unsigned(const char str[], const char str_name[]);
static unsigned long string_to_unsigned_long(const char str[],
					     const char str_name[]);
static unsigned long do_string_to_unsigned_long(const char str[],
						const char str_name[],
						const char blurb[]);

/* matches either a blank line or a comment line */
static const char blank_or_comment_regex[] =
    /* a blank line */
    "(^[[:space:]]+$)" "|"	/* or ... */
    /* a line consisting of: optional whitespace followed by */
    "(^[[:space:]]*"
    /* a '#' character and optionally, additional characters */
    "#.*$)";

static regex_t blank_or_comment_expr;

/* matches the line in a CMOS layout file indicating the start of the
 * "entries" section.
 */
static const char start_entries_regex[] =
    /* optional whitespace */
    "^[[:space:]]*"
    /* followed by "entries" */
    "entries"
    /* followed by optional whitespace */
    "[[:space:]]*$";

static regex_t start_entries_expr;

/* matches the line in a CMOS layout file indicating the start of the
 * "enumerations" section
 */
static const char start_enums_regex[] =
    /* optional whitespace */
    "^[[:space:]]*"
    /* followed by "enumerations" */
    "enumerations"
    /* followed by optional whitespace */
    "[[:space:]]*$";

static regex_t start_enums_expr;

/* matches the line in a CMOS layout file indicating the start of the
 * "checksums" section
 */
static const char start_checksums_regex[] =
    /* optional whitespace */
    "^[[:space:]]*"
    /* followed by "checksums" */
    "checksums"
    /* followed by optional whitespace */
    "[[:space:]]*$";

static regex_t start_checksums_expr;

/* matches a line in a CMOS layout file specifying a CMOS entry */
static const char entries_line_regex[] =
    /* optional whitespace */
    "^[[:space:]]*"
    /* followed by a chunk of nonwhitespace for start-bit field */
    "([^[:space:]]+)"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for length field */
    "([^[:space:]]+)"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for config field */
    "([^[:space:]]+)"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for config-ID field */
    "([^[:space:]]+)"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for name field */
    "([^[:space:]]+)"
    /* followed by optional whitespace */
    "[[:space:]]*$";

static regex_t entries_line_expr;

/* matches a line in a CMOS layout file specifying a CMOS enumeration */
static const char enums_line_regex[] =
    /* optional whitespace */
    "^[[:space:]]*"
    /* followed by a chunk of nonwhitespace for ID field */
    "([^[:space:]]+)"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for value field */
    "([^[:space:]]+)"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for text field */
    "([[:print:]]*[^[:space:]])"
    /* followed by optional whitespace */
    "[[:space:]]*$";

static regex_t enums_line_expr;

/* matches the line in a CMOS layout file specifying CMOS checksum
 * information
 */
static const char checksum_line_regex[] =
    /* optional whitespace */
    "^[[:space:]]*"
    /* followed by "checksum" */
    "checksum"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for first bit of summed area */
    "([^[:space:]]+)"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for last bit of summed area */
    "([^[:space:]]+)"
    /* followed by one or more whitespace characters */
    "[[:space:]]+"
    /* followed by a chunk of nonwhitespace for checksum location bit */
    "([^[:space:]]+)"
    /* followed by optional whitespace */
    "[[:space:]]*$";

static regex_t checksum_line_expr;

static const int LINE_BUF_SIZE = 256;

static int line_num;

static const char *layout_filename = NULL;

/****************************************************************************
 * set_layout_filename
 *
 * Set the name of the file we will obtain CMOS layout information from.
 ****************************************************************************/
void set_layout_filename(const char filename[])
{
	layout_filename = filename;
}

/****************************************************************************
 * get_layout_from_file
 *
 * Read CMOS layout information from the user-specified CMOS layout file.
 ****************************************************************************/
void get_layout_from_file(void)
{
	FILE *f;

	assert(layout_filename != NULL);

	if ((f = fopen(layout_filename, "r")) == NULL) {
		fprintf(stderr,
			"%s: Can not open CMOS layout file %s for reading: "
			"%s\n", prog_name, layout_filename, strerror(errno));
		exit(1);
	}

	process_layout_file(f);
	fclose(f);
}

void write_cmos_layout_header(const char *header_filename)
{
	FILE *fp;
	const cmos_entry_t *cmos_entry;
	cmos_checksum_layout_t layout;

	if ((fp = fopen(header_filename, "w+")) == NULL) {
		fprintf(stderr,
				"%s: Can't open file %s for writing: %s\n",
				prog_name, header_filename, strerror(errno));
			exit(1);
	}

	fprintf(fp, "/**\n * This is an autogenerated file. Do not EDIT.\n"
			" * All changes made to this file will be lost.\n"
			" * See mainboard's cmos.layout file.\n */\n"
			"\n#ifndef __OPTION_TABLE_H\n"
			"#define __OPTION_TABLE_H\n\n");

	for (cmos_entry = first_cmos_entry(); cmos_entry != NULL;
			cmos_entry = next_cmos_entry(cmos_entry)) {

		if (!is_ident((char *)cmos_entry->name)) {
			fprintf(stderr,
				"Error - Name %s is an invalid identifier\n",
				cmos_entry->name);
			fclose(fp);
			exit(1);
		}

		fprintf(fp, "#define CMOS_VSTART_%s\t%d\n",
				cmos_entry->name, cmos_entry->bit);
		fprintf(fp, "#define CMOS_VLEN_%s\t%d\n",
				cmos_entry->name, cmos_entry->length);
	}

	layout.summed_area_start = cmos_checksum_start;
	layout.summed_area_end = cmos_checksum_end;
	layout.checksum_at = cmos_checksum_index;
	checksum_layout_to_bits(&layout);

	fprintf(fp, "\n#define LB_CKS_RANGE_START %d\n",
			layout.summed_area_start / 8);
	fprintf(fp, "#define LB_CKS_RANGE_END %d\n",
			layout.summed_area_end / 8);
	fprintf(fp, "#define LB_CKS_LOC %d\n",
			layout.checksum_at / 8);
	fprintf(fp, "\n#endif /* __OPTION_TABLE_H */\n");

	fclose(fp);
}
/****************************************************************************
 * write_cmos_layout
 *
 * Write CMOS layout information to file 'f'.  The output is written in the
 * format that CMOS layout files adhere to.
 ****************************************************************************/
void write_cmos_layout(FILE * f)
{
	const cmos_entry_t *cmos_entry;
	const cmos_enum_t *cmos_enum;
	cmos_checksum_layout_t layout;

	fprintf(f, "entries\n");

	for (cmos_entry = first_cmos_entry();
	     cmos_entry != NULL; cmos_entry = next_cmos_entry(cmos_entry))
		fprintf(f, "%u %u %c %u %s\n", cmos_entry->bit,
			cmos_entry->length,
			cmos_entry_char_value(cmos_entry->config),
			cmos_entry->config_id, cmos_entry->name);

	fprintf(f, "\nenumerations\n");

	for (cmos_enum = first_cmos_enum();
	     cmos_enum != NULL; cmos_enum = next_cmos_enum(cmos_enum))
		fprintf(f, "%u %llu %s\n", cmos_enum->config_id,
			cmos_enum->value, cmos_enum->text);

	layout.summed_area_start = cmos_checksum_start;
	layout.summed_area_end = cmos_checksum_end;
	layout.checksum_at = cmos_checksum_index;
	checksum_layout_to_bits(&layout);
	fprintf(f, "\nchecksums\nchecksum %u %u %u\n", layout.summed_area_start,
		layout.summed_area_end, layout.checksum_at);
}

/****************************************************************************
 * process_layout_file
 *
 * Read CMOS layout information from file 'f' and add it to our internal
 * repository.
 ****************************************************************************/
static void process_layout_file(FILE * f)
{
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, blank_or_comment_regex, &blank_or_comment_expr);
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, start_entries_regex, &start_entries_expr);
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, entries_line_regex, &entries_line_expr);
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, start_enums_regex, &start_enums_expr);
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, enums_line_regex, &enums_line_expr);
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, start_checksums_regex, &start_checksums_expr);
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, checksum_line_regex, &checksum_line_expr);
	line_num = 1;
	skip_past_start(f);

	/* Skip past all entries.  We will process these later when we
	 * make a second pass through the file.
	 */
	while (!process_entry(f, 1)) ;

	/* Process all enums, adding them to our internal repository as
	 * we go. */

	if (process_enum(f, 0)) {
		fprintf(stderr, "%s: Error: CMOS layout file contains no "
			"enumerations.\n", prog_name);
		exit(1);
	}

	while (!process_enum(f, 0)) ;

	/* Go back to start of file. */
	line_num = 1;
	fseek(f, 0, SEEK_SET);

	skip_past_start(f);

	/* Process all entries, adding them to the repository as we go.
	 * We must add the entries after the enums, even though they
	 * appear in the layout file before the enums.  This is because
	 * the entries are sanity checked against the enums as they are
	 * added.
	 */

	if (process_entry(f, 0)) {
		fprintf(stderr,
			"%s: Error: CMOS layout file contains no entries.\n",
			prog_name);
		exit(1);
	}

	while (!process_entry(f, 0)) ;

	/* Skip past all enums.  They have already been processed. */
	while (!process_enum(f, 1)) ;

	/* Process CMOS checksum info. */
	process_checksum_info(f);

	/* See if there are any lines left to process.  If so, verify
	 * that they are all either blank lines or comments.
	 */
	skip_remaining_lines(f);

	regfree(&blank_or_comment_expr);
	regfree(&start_entries_expr);
	regfree(&entries_line_expr);
	regfree(&start_enums_expr);
	regfree(&enums_line_expr);
	regfree(&start_checksums_expr);
	regfree(&checksum_line_expr);
}

/****************************************************************************
 * skip_past_start
 *
 * Skip past the line that marks the start of the "entries" section.
 ****************************************************************************/
static void skip_past_start(FILE * f)
{
	char line[LINE_BUF_SIZE];

	for (;; line_num++) {
		if (get_layout_file_line(f, line, LINE_BUF_SIZE)) {
			fprintf(stderr,
				"%s: \"entries\" line not found in CMOS layout file.\n",
				prog_name);
			exit(1);
		}

		if (!regexec(&blank_or_comment_expr, line, 0, NULL, 0))
			continue;

		if (!regexec(&start_entries_expr, line, 0, NULL, 0))
			break;

		fprintf(stderr,
			"%s: Syntax error on line %d of CMOS layout file.  "
			"\"entries\" line expected.\n", prog_name, line_num);
		exit(1);
	}

	line_num++;
}

/****************************************************************************
 * process_entry
 *
 * Get an entry from "entries" section of file and add it to our repository
 * of layout information.  Return 0 if an entry was found and processed.
 * Return 1 if there are no more entries.
 ****************************************************************************/
static int process_entry(FILE * f, int skip_add)
{
	static const size_t N_MATCHES = 6;
	char line[LINE_BUF_SIZE];
	regmatch_t match[N_MATCHES];
	cmos_entry_t cmos_entry;
	int result;

	result = 1;

	for (;; line_num++) {
		if (get_layout_file_line(f, line, LINE_BUF_SIZE)) {
			fprintf(stderr,
				"%s: Unexpected end of CMOS layout file reached while "
				"reading \"entries\" section.\n", prog_name);
			exit(1);
		}

		if (!regexec(&blank_or_comment_expr, line, 0, NULL, 0))
			continue;

		if (regexec(&entries_line_expr, line, N_MATCHES, match, 0)) {
			if (regexec(&start_enums_expr, line, 0, NULL, 0)) {
				fprintf(stderr,
					"%s: Syntax error on line %d of CMOS layout "
					"file.\n", prog_name, line_num);
				exit(1);
			}

			break;	/* start of enumerations reached: no more entries */
		}

		result = 0;	/* next layout entry found */

		if (skip_add)
			break;

		line[match[1].rm_eo] = '\0';
		line[match[2].rm_eo] = '\0';
		line[match[3].rm_eo] = '\0';
		line[match[4].rm_eo] = '\0';
		line[match[5].rm_eo] = '\0';
		create_entry(&cmos_entry, &line[match[1].rm_so],
			     &line[match[2].rm_so], &line[match[3].rm_so],
			     &line[match[4].rm_so], &line[match[5].rm_so]);
		try_add_layout_file_entry(&cmos_entry);
		break;
	}

	line_num++;
	return result;
}

/****************************************************************************
 * process_enum
 *
 * Get an enuneration from "enumerations" section of file and add it to our
 * repository of layout information.  Return 0 if an enumeration was found
 * and processed.  Return 1 if there are no more enumerations.
 ****************************************************************************/
static int process_enum(FILE * f, int skip_add)
{
	static const size_t N_MATCHES = 4;
	char line[LINE_BUF_SIZE];
	regmatch_t match[N_MATCHES];
	cmos_enum_t cmos_enum;
	int result;

	result = 1;

	for (;; line_num++) {
		if (get_layout_file_line(f, line, LINE_BUF_SIZE)) {
			fprintf(stderr,
				"%s: Unexpected end of CMOS layout file reached while "
				"reading \"enumerations\" section.\n",
				prog_name);
			exit(1);
		}

		if (!regexec(&blank_or_comment_expr, line, 0, NULL, 0))
			continue;

		if (regexec(&enums_line_expr, line, N_MATCHES, match, 0)) {
			if (regexec(&start_checksums_expr, line, 0, NULL, 0)) {
				fprintf(stderr,
					"%s: Syntax error on line %d of CMOS layout "
					"file.\n", prog_name, line_num);
				exit(1);
			}

			break;	/* start of checksums reached: no more enumerations */
		}

		result = 0;	/* next layout enumeration found */

		if (skip_add)
			break;

		line[match[1].rm_eo] = '\0';
		line[match[2].rm_eo] = '\0';
		line[match[3].rm_eo] = '\0';
		create_enum(&cmos_enum, &line[match[1].rm_so],
			    &line[match[2].rm_so], &line[match[3].rm_so]);
		try_add_cmos_enum(&cmos_enum);
		break;
	}

	line_num++;
	return result;
}

/****************************************************************************
 * process_checksum_info
 *
 * Get line conatining CMOS checksum information.
 ****************************************************************************/
static void process_checksum_info(FILE * f)
{
	static const size_t N_MATCHES = 4;
	char line[LINE_BUF_SIZE];
	regmatch_t match[N_MATCHES];

	for (;; line_num++) {
		if (get_layout_file_line(f, line, LINE_BUF_SIZE)) {
			fprintf(stderr,
				"%s: Unexpected end of CMOS layout file reached while "
				"reading \"checksums\" section.\n", prog_name);
			exit(1);
		}

		if (!regexec(&blank_or_comment_expr, line, 0, NULL, 0))
			continue;

		if (regexec(&checksum_line_expr, line, N_MATCHES, match, 0)) {
			fprintf(stderr,
				"%s: Syntax error on line %d of CMOS layout "
				"file.  \"checksum\" line expected.\n",
				prog_name, line_num);
			exit(1);
		}

		/* checksum line found */
		line[match[1].rm_eo] = '\0';
		line[match[2].rm_eo] = '\0';
		line[match[3].rm_eo] = '\0';
		set_checksum_info(&line[match[1].rm_so], &line[match[2].rm_so],
				  &line[match[3].rm_so]);
		break;
	}
}

/****************************************************************************
 * skip_remaining_lines
 *
 * Get any remaining lines of unprocessed input.  Complain if we find a line
 * that contains anything other than comments and whitespace.
 ****************************************************************************/
static void skip_remaining_lines(FILE * f)
{
	char line[LINE_BUF_SIZE];

	for (line_num++;
	     get_layout_file_line(f, line, LINE_BUF_SIZE) == OK; line_num++) {
		if (regexec(&blank_or_comment_expr, line, 0, NULL, 0)) {
			fprintf(stderr,
				"%s: Syntax error on line %d of CMOS layout file: "
				"Only comments and/or whitespace allowed after "
				"\"checksum\" line.\n", prog_name, line_num);
			exit(1);
		}
	}
}

/****************************************************************************
 * create_entry
 *
 * Create a CMOS entry structure representing the given information.  Perform
 * sanity checking on input parameters.
 ****************************************************************************/
static void create_entry(cmos_entry_t * cmos_entry,
			 const char start_bit_str[], const char length_str[],
			 const char config_str[], const char config_id_str[],
			 const char name_str[])
{
	cmos_entry->bit = string_to_unsigned(start_bit_str, "start-bit");
	cmos_entry->length = string_to_unsigned(length_str, "length");

	if (config_str[1] != '\0')
		goto bad_config_str;

	switch (config_str[0]) {
	case 'e':
		cmos_entry->config = CMOS_ENTRY_ENUM;
		break;

	case 'h':
		cmos_entry->config = CMOS_ENTRY_HEX;
		break;

	case 's':
		cmos_entry->config = CMOS_ENTRY_STRING;
		break;

	case 'r':
		cmos_entry->config = CMOS_ENTRY_RESERVED;
		break;

	default:
		goto bad_config_str;
	}

	cmos_entry->config_id = string_to_unsigned(config_id_str, "config-ID");

	if (strlen(name_str) >= CMOS_MAX_NAME_LENGTH) {
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file: name too "
			"long (max length is %d).\n", prog_name, line_num,
			CMOS_MAX_NAME_LENGTH - 1);
		exit(1);
	}

	strcpy(cmos_entry->name, name_str);
	return;

      bad_config_str:
	fprintf(stderr,
		"%s: Error on line %d of CMOS layout file: 'e', 'h', or "
		"'r' expected for config value.\n", prog_name, line_num);
	exit(1);
}

/****************************************************************************
 * try_add_layout_file_entry
 *
 * Attempt to add the given CMOS entry to our internal repository.  Exit with
 * an error message on failure.
 ****************************************************************************/
static void try_add_layout_file_entry(const cmos_entry_t * cmos_entry)
{
	const cmos_entry_t *conflict;

	switch (add_cmos_entry(cmos_entry, &conflict)) {
	case OK:
		return;

	case CMOS_AREA_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  Area "
			"covered by entry %s is out of range.\n", prog_name,
			line_num, cmos_entry->name);
		break;

	case CMOS_AREA_TOO_WIDE:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  Area "
			"covered by entry %s is too wide.\n", prog_name,
			line_num, cmos_entry->name);
		break;

	case LAYOUT_ENTRY_OVERLAP:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  Layouts "
			"overlap for entries %s and %s.\n", prog_name, line_num,
			cmos_entry->name, conflict->name);
		break;

	case LAYOUT_ENTRY_BAD_LENGTH:
		/* Silently ignore entries with zero length.  Although this should
		 * never happen in practice, we should handle the case in a
		 * reasonable manner just to be safe.
		 */
		return;

	default:
		BUG();
	}

	exit(1);
}

/****************************************************************************
 * create_enum
 *
 * Create a CMOS enumeration structure representing the given information.
 * Perform sanity checking on input parameters.
 ****************************************************************************/
static void create_enum(cmos_enum_t * cmos_enum, const char id_str[],
			const char value_str[], const char text_str[])
{
	cmos_enum->config_id = string_to_unsigned(id_str, "ID");
	cmos_enum->value = string_to_unsigned_long(value_str, "value");

	if (strlen(text_str) >= CMOS_MAX_TEXT_LENGTH) {
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file: text too "
			"long (max length is %d).\n", prog_name, line_num,
			CMOS_MAX_TEXT_LENGTH - 1);
		exit(1);
	}

	strcpy(cmos_enum->text, text_str);
}

/****************************************************************************
 * try_add_cmos_enum
 *
 * Attempt to add the given CMOS enum to our internal repository.  Exit with
 * an error message on failure.
 ****************************************************************************/
static void try_add_cmos_enum(const cmos_enum_t * cmos_enum)
{
	switch (add_cmos_enum(cmos_enum)) {
	case OK:
		return;

	case LAYOUT_DUPLICATE_ENUM:
		fprintf(stderr, "%s: Error on line %d of CMOS layout file: "
			"Enumeration found with duplicate ID/value combination.\n",
			prog_name, line_num);
		break;

	default:
		BUG();
	}

	exit(1);
}

/****************************************************************************
 * set_checksum_info
 *
 * Set CMOS checksum information according to input parameters and perform
 * sanity checking on input parameters.
 ****************************************************************************/
static void set_checksum_info(const char start_str[], const char end_str[],
			      const char index_str[])
{
	cmos_checksum_layout_t layout;

	/* These are bit positions that we want to convert to byte positions. */
	layout.summed_area_start =
	    string_to_unsigned(start_str, "CMOS checksummed area start");
	layout.summed_area_end =
	    string_to_unsigned(end_str, "CMOS checksummed area end");
	layout.checksum_at =
	    string_to_unsigned(index_str, "CMOS checksum location");

	switch (checksum_layout_to_bytes(&layout)) {
	case OK:
		break;

	case LAYOUT_SUMMED_AREA_START_NOT_ALIGNED:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  CMOS "
			"checksummed area start is not byte-aligned.\n",
			prog_name, line_num);
		goto fail;

	case LAYOUT_SUMMED_AREA_END_NOT_ALIGNED:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  CMOS "
			"checksummed area end is not byte-aligned.\n",
			prog_name, line_num);
		goto fail;

	case LAYOUT_CHECKSUM_LOCATION_NOT_ALIGNED:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  CMOS "
			"checksum location is not byte-aligned.\n", prog_name,
			line_num);
		goto fail;

	case LAYOUT_INVALID_SUMMED_AREA:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  CMOS "
			"checksummed area end must be greater than CMOS checksummed "
			"area start.\n", prog_name, line_num);
		goto fail;

	case LAYOUT_CHECKSUM_OVERLAPS_SUMMED_AREA:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  CMOS "
			"checksum overlaps checksummed area.\n", prog_name,
			line_num);
		goto fail;

	case LAYOUT_SUMMED_AREA_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  CMOS "
			"checksummed area out of range.\n", prog_name,
			line_num);
		goto fail;

	case LAYOUT_CHECKSUM_LOCATION_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file.  CMOS "
			"checksum location out of range.\n", prog_name,
			line_num);
		goto fail;

	default:
		BUG();
	}

	cmos_checksum_start = layout.summed_area_start;
	cmos_checksum_end = layout.summed_area_end;
	cmos_checksum_index = layout.checksum_at;
	return;

      fail:
	exit(1);
}

/****************************************************************************
 * cmos_entry_char_value
 *
 * Return the character representation of 'config'.
 ****************************************************************************/
static char cmos_entry_char_value(cmos_entry_config_t config)
{
	switch (config) {
	case CMOS_ENTRY_ENUM:
		return 'e';

	case CMOS_ENTRY_HEX:
		return 'h';

	case CMOS_ENTRY_RESERVED:
		return 'r';

	case CMOS_ENTRY_STRING:
		return 's';

	default:
		BUG();
	}

	return 0;		/* not reached */
}

/****************************************************************************
 * get_layout_file_line
 *
 * Get a line of input from file 'f'.  Store result in 'line' which is an
 * array of 'line_buf_size' bytes.  Return OK on success or an error code on
 * failure.
 ****************************************************************************/
static int get_layout_file_line(FILE * f, char line[], int line_buf_size)
{
	switch (get_line_from_file(f, line, line_buf_size)) {
	case OK:
		return OK;

	case LINE_EOF:
		return LINE_EOF;

	case LINE_TOO_LONG:
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file: Maximum "
			"line length exceeded.  Max is %d characters.\n",
			prog_name, line_num, line_buf_size - 2);
		break;
	}

	exit(1);
	return 1;		/* keep compiler happy */
}

/****************************************************************************
 * string_to_unsigned
 *
 * Convert the string 'str' to an unsigned and return the result.
 ****************************************************************************/
static unsigned string_to_unsigned(const char str[], const char str_name[])
{
	unsigned long n;
	unsigned z;

	n = do_string_to_unsigned_long(str, str_name, "");

	if ((z = (unsigned)n) != n) {
		/* This could happen on an architecture in which
		 * sizeof(unsigned) < sizeof(unsigned long).
		 */
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file: %s value is "
			"out of range.\n", prog_name, line_num, str_name);
		exit(1);
	}

	return z;
}

/****************************************************************************
 * string_to_unsigned_long
 *
 * Convert the string 'str' to an unsigned long and return the result.
 ****************************************************************************/
static unsigned long string_to_unsigned_long(const char str[],
					     const char str_name[])
{
	return do_string_to_unsigned_long(str, str_name, " long");
}

/****************************************************************************
 * do_string_to_unsigned_long
 *
 * Convert the string 'str' to an unsigned long and return the result.  Exit
 * with an appropriate error message on failure.
 ****************************************************************************/
static unsigned long do_string_to_unsigned_long(const char str[],
						const char str_name[],
						const char blurb[])
{
	unsigned long n;
	char *p;

	n = strtoul(str, &p, 0);

	if (*p != '\0') {
		fprintf(stderr,
			"%s: Error on line %d of CMOS layout file: %s is not a "
			"valid unsigned%s integer.\n", prog_name, line_num,
			str_name, blurb);
		exit(1);
	}

	return n;
}
