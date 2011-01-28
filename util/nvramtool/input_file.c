/*****************************************************************************\
 * input_file.c
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by David S. Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
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
#include "input_file.h"
#include "layout.h"
#include "cmos_ops.h"
#include "cmos_lowlevel.h"
#include "reg_expr.h"

static int get_input_file_line(FILE * f, char line[], int line_buf_size);
static unsigned long long try_prepare_cmos_write(const cmos_entry_t * e,
						 const char value_str[]);

/* matches either a blank line or a comment line */
static const char blank_or_comment_regex[] =
    /* a blank line */
    "(^[[:space:]]+$)" "|"	/* or ... */
    /* a line consisting of: optional whitespace followed by */
    "(^[[:space:]]*"
    /* a '#' character and optionally, additional characters */
    "#.*$)";

/* matches an assignment line */
const char assignment_regex[] =
    /* optional whitespace */
    "^[[:space:]]*"
    /* followed by a coreboot parameter name */
    "([^[:space:]]+)"
    /* followed by optional whitespace */
    "[[:space:]]*"
    /* followed by an '=' character */
    "="
    /* followed by optional whitespace */
    "[[:space:]]*"
    /* followed by a value that may contain embedded whitespace */
    "([^[:space:]]+([[:space:]]+[^[:space:]]+)*)+"
    /* followed by optional whitespace */
    "[[:space:]]*$";

static int line_num;

/****************************************************************************
 * process_input_file
 *
 * Read the contents of file 'f' and return a pointer to a list of pending
 * write operations.  Perform sanity checking on all write operations and
 * exit with an error message if there is a problem.
 ****************************************************************************/
cmos_write_t *process_input_file(FILE * f)
{
	static const int LINE_BUF_SIZE = 256;
	static const size_t N_MATCHES = 4;
	char line[LINE_BUF_SIZE];
	const char *name, *value;
	cmos_write_t *list, *item, **p;
	regex_t blank_or_comment, assignment;
	regmatch_t match[N_MATCHES];
	const cmos_entry_t *e;

	list = NULL;
	p = &list;

	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, blank_or_comment_regex, &blank_or_comment);
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, assignment_regex, &assignment);

	/* each iteration processes one line from input file */
	for (line_num = 1; get_input_file_line(f, line, LINE_BUF_SIZE) == OK; line_num++) {	/* skip comments and blank lines */
		if (!regexec(&blank_or_comment, line, 0, NULL, 0))
			continue;

		/* Is this a valid assignment line?  If not, then it's a syntax
		 * error.
		 */
		if (regexec(&assignment, line, N_MATCHES, match, 0)) {
			fprintf(stderr,
				"%s: Syntax error on line %d of input file.\n",
				prog_name, line_num);
			exit(1);
		}

		/* OK, we found an assignment.  Break the line into substrings
		 * representing the lefthand and righthand sides of the assignment.
		 */
		line[match[1].rm_eo] = '\0';
		line[match[2].rm_eo] = '\0';
		name = &line[match[1].rm_so];
		value = &line[match[2].rm_so];

		/* now look up the coreboot parameter name */
		if (is_checksum_name(name)
		    || (e = find_cmos_entry(name)) == NULL) {
			fprintf(stderr,
				"%s: Error on line %d of input file: CMOS parameter "
				"%s not found.\n", prog_name, line_num, name);
			exit(1);
		}

		/* At this point, we figure out what numeric value needs to be written
		 * to which location.  At the same time, we perform sanity checking on
		 * the write operation.
		 */

		if ((item = (cmos_write_t *) malloc(sizeof(*item))) == NULL)
			out_of_memory();

		item->bit = e->bit;
		item->length = e->length;
		item->config = e->config;
		item->value = try_prepare_cmos_write(e, value);

		/* Append write operation to pending write list. */
		item->next = NULL;
		*p = item;
		p = &item->next;
	}

	regfree(&blank_or_comment);
	regfree(&assignment);
	return list;
}

/****************************************************************************
 * do_cmos_writes
 *
 * 'list' is a linked list of pending CMOS write operations that have passed
 * all sanity checks.  Perform all write operations, destroying the list as
 * we go.
 ****************************************************************************/
void do_cmos_writes(cmos_write_t * list)
{
	cmos_write_t *item;

	set_iopl(3);

	while (list != NULL) {
		cmos_entry_t e;
		item = list;
		e.bit = item->bit;
		e.length = item->length;
		e.config = item->config;
		list = item->next;
		cmos_write(&e, item->value);
		free(item);
	}

	cmos_checksum_write(cmos_checksum_compute());
	set_iopl(0);
}

/****************************************************************************
 * get_input_file_line
 *
 * Get a line of input from file 'f'.  Store result in 'line' which is an
 * array of 'line_buf_size' bytes.  Return OK on success or an error code on
 * error.
 ****************************************************************************/
static int get_input_file_line(FILE * f, char line[], int line_buf_size)
{
	switch (get_line_from_file(f, line, line_buf_size)) {
	case OK:
		return OK;

	case LINE_EOF:
		return LINE_EOF;

	case LINE_TOO_LONG:
		fprintf(stderr,
			"%s: Error on line %d of input file: Maximum line "
			"length exceeded.  Max is %d characters.\n", prog_name,
			line_num, line_buf_size - 2);
		break;

	default:
		BUG();
	}

	exit(1);
	return 1;		/* keep compiler happy */
}

/****************************************************************************
 * try_prepare_cmos_write
 *
 * Attempt to convert 'value_str' to an integer representation for storage in
 * CMOS memory.  On success, return the converted value.  On error, exit with
 * an error message.
 ****************************************************************************/
static unsigned long long try_prepare_cmos_write(const cmos_entry_t * e,
						 const char value_str[])
{
	unsigned long long value;

	switch (prepare_cmos_write(e, value_str, &value)) {
	case OK:
		return value;

	case CMOS_OP_BAD_ENUM_VALUE:
		fprintf(stderr,
			"%s: Error on line %d of input file: Bad value for "
			"parameter %s.", prog_name, line_num, e->name);
		break;

	case CMOS_OP_NEGATIVE_INT:
		fprintf(stderr,
			"%s: Error on line %d of input file: This program "
			"does not support assignment of negative numbers to "
			"coreboot parameters.", prog_name, line_num);
		break;

	case CMOS_OP_INVALID_INT:
		fprintf(stderr,
			"%s: Error on line %d of input file: %s is not a "
			"valid integer.", prog_name, line_num, value_str);
		break;

	case CMOS_OP_RESERVED:
		fprintf(stderr,
			"%s: Error on line %d of input file: Can not modify "
			"reserved coreboot parameter %s.", prog_name, line_num,
			e->name);
		break;

	case CMOS_OP_VALUE_TOO_WIDE:
		fprintf(stderr,
			"%s: Error on line %d of input file: Can not write "
			"value %s to CMOS parameter %s that is only %d bits wide.",
			prog_name, line_num, value_str, e->name, e->length);
		break;

	case CMOS_OP_NO_MATCHING_ENUM:
		fprintf(stderr,
			"%s: coreboot parameter %s has no matching enums.",
			prog_name, e->name);
		break;

	case CMOS_AREA_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: The CMOS area specified by the layout info for "
			"coreboot parameter %s is out of range.", prog_name,
			e->name);
		break;

	case CMOS_AREA_OVERLAPS_RTC:
		fprintf(stderr,
			"%s: The CMOS area specified by the layout info for "
			"coreboot parameter %s overlaps the realtime clock area.",
			prog_name, e->name);
		break;

	case CMOS_AREA_TOO_WIDE:
		fprintf(stderr,
			"%s: The CMOS area specified by the layout info for "
			"coreboot parameter %s is too wide.", prog_name,
			e->name);
		break;

	default:
		fprintf(stderr,
			"%s: Unknown error encountered while attempting to modify "
			"coreboot parameter %s.", prog_name, e->name);
		break;
	}

	fprintf(stderr, "  No CMOS writes performed.\n");
	exit(1);
	return 0;		/* keep compiler happy */
}
