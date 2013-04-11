/*****************************************************************************\
 * nvramtool.c
 *****************************************************************************
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef __MINGW32__
#include <sys/mman.h>
#endif
#include "common.h"
#include "opts.h"
#include "lbtable.h"
#include "layout.h"
#include "accessors/layout-text.h"
#include "input_file.h"
#include "cmos_ops.h"
#include "cmos_lowlevel.h"
#include "reg_expr.h"
#include "hexdump.h"
#include "cbfs.h"
#ifdef __MINGW32__
#include <windows.h>
#endif

typedef void (*op_fn_t) (void);

static void op_show_version(void);
static void op_show_usage(void);
static void op_lbtable_show_info(void);
static void op_lbtable_dump(void);
static void op_show_param_values(void);
static void op_cmos_show_one_param(void);
static void op_cmos_show_all_params(void);
static void op_cmos_set_one_param(void);
static void op_cmos_set_params_stdin(void);
static void op_cmos_set_params_file(void);
static void op_cmos_checksum(void);
static void op_show_layout(void);
static void op_write_cmos_dump(void);
static void op_read_cmos_dump(void);
static void op_show_cmos_hex_dump(void);
static void op_show_cmos_dumpfile(void);
static void op_write_cmos_layout_bin(void);
static void op_write_cmos_layout_header(void);
static int list_one_param(const char name[], int show_name);
static int list_all_params(void);
static void list_param_enums(const char name[]);
static void set_one_param(const char name[], const char value[]);
static void set_params(FILE * f);
static void parse_assignment(char arg[], const char **name, const char **value);
static int list_cmos_entry(const cmos_entry_t * e, int show_name);
static uint16_t convert_checksum_value(const char value[]);

static const op_fn_t op_fns[] = { op_show_version,
	op_show_usage,
	op_lbtable_show_info,
	op_lbtable_dump,
	op_show_param_values,
	op_cmos_show_one_param,
	op_cmos_show_all_params,
	op_cmos_set_one_param,
	op_cmos_set_params_stdin,
	op_cmos_set_params_file,
	op_cmos_checksum,
	op_show_layout,
	op_write_cmos_dump,
	op_read_cmos_dump,
	op_show_cmos_hex_dump,
	op_show_cmos_dumpfile,
	op_write_cmos_layout_bin,
	op_write_cmos_layout_header
};

static void op_write_cmos_layout_bin(void)
{
	get_layout_from_file();
	write_cmos_output_bin(nvramtool_op.param);
}

static void op_write_cmos_layout_header(void)
{
	get_layout_from_file();
	write_cmos_layout_header(nvramtool_op.param);
}

static const hexdump_format_t cmos_dump_format =
    { 16, 2, "", " | ", " ", " | ", '.' };

/****************************************************************************
 * main
 ****************************************************************************/
int main(int argc, char *argv[])
{
	void *cmos_default = NULL;
	cmos_layout_get_fn_t fn = get_layout_from_cmos_table;

	parse_nvramtool_args(argc, argv);

	/* If we should operate on a CBFS file default to reading the layout
	 * and CMOS contents from it. */
	if (nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CBFS_FILE].found) {
		open_cbfs(nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CBFS_FILE].param);
		if (!nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_FILE].found) {
			cmos_default = cbfs_find_file("cmos.default", CBFS_COMPONENT_CMOS_DEFAULT, NULL);
			if (cmos_default == NULL) {
				fprintf(stderr, "Need a cmos.default in the CBFS image or separate cmos file (-D).\n");
				exit(1);
			}
		}
		fn = get_layout_from_cbfs_file;
	}

	/* If the user wants to use a specific layout file or explicitly use
	 * the coreboot option table allow him to override previous settings. */
	if (nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_LAYOUT_FILE].found) {
		set_layout_filename(nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_LAYOUT_FILE].param);
		fn = get_layout_from_file;
	} else if (nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_OPT_TABLE].found) {
		fn = get_layout_from_cmos_table;
	}

	/* Allow the user to use a file for the CMOS contents, possibly
	 * overriding a previously opened "cmos.default" file from the CBFS. */
	if (nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_FILE].found) {
		struct stat fd_stat;
		int fd;

		if ((fd = open(nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_FILE].param, O_RDWR | O_CREAT, 0666)) < 0) {
			fprintf(stderr, "Couldn't open '%s'\n", nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_FILE].param);
			exit(1);
		}

		if (fstat(fd, &fd_stat) == -1) {
			fprintf(stderr, "Couldn't stat '%s'\n", nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_FILE].param);
			exit(1);
		}

		if (fd_stat.st_size < CMOS_SIZE) {
			lseek(fd, CMOS_SIZE - 1, SEEK_SET);
			if (write(fd, "\0", 1) != 1) {
				fprintf(stderr, "Unable to extended '%s' to its full size.\n",
						nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_FILE].param);
				exit(1);
			}
#ifndef __MINGW32__
			fsync(fd);
#else
			FlushFileBuffers ((HANDLE) _get_osfhandle (fd));
#endif
		}

		cmos_default = mmap(NULL, CMOS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (cmos_default == MAP_FAILED) {
			fprintf(stderr, "Couldn't map '%s'\n", nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_FILE].param);
			exit(1);
		}
	}

	/* Switch to memory based CMOS access. */
	if (cmos_default) {
		select_hal(HAL_MEMORY, cmos_default);
	}

	register_cmos_layout_get_fn(fn);
	op_fns[nvramtool_op.op]();

	return 0;
}

/****************************************************************************
 * op_show_version
 *
 * -v
 *
 * Show version information for this program.
 ****************************************************************************/
static void op_show_version(void)
{
	printf("This is %s version %s.\n", prog_name, prog_version);
}

/****************************************************************************
 * op_show_usage
 *
 * -h
 *
 * Show a usage message for this program.
 ****************************************************************************/
static void op_show_usage(void)
{
	usage(stdout);
}

/****************************************************************************
 * op_lbtable_show_info
 *
 * -l [ARG]
 *
 * If ARG is present, show coreboot table information specified by ARG.
 * Else show all possible values for ARG.
 ****************************************************************************/
static void op_lbtable_show_info(void)
{
	if (nvramtool_op.param == NULL)
		list_lbtable_choices();
	else {
		get_lbtable();
		list_lbtable_item(nvramtool_op.param);
	}
}

/****************************************************************************
 * op_lbtable_dump
 *
 * -d
 *
 * Do low-level dump of coreboot table.
 ****************************************************************************/
static void op_lbtable_dump(void)
{
	get_lbtable();
	dump_lbtable();
}

/****************************************************************************
 * op_show_param_values
 *
 * -e NAME option
 *
 * Show all possible values for parameter NAME.
 ****************************************************************************/
static void op_show_param_values(void)
{
	get_cmos_layout();
	list_param_enums(nvramtool_op.param);
}

/****************************************************************************
 * op_cmos_show_one_param
 *
 * [-n] -r NAME
 *
 * Show parameter NAME.  If -n is specified, show value only.  Else show name
 * and value.
 ****************************************************************************/
static void op_cmos_show_one_param(void)
{
	int result;

	get_cmos_layout();
	result = list_one_param(nvramtool_op.param,
				!nvramtool_op_modifiers
				[NVRAMTOOL_MOD_SHOW_VALUE_ONLY].found);
	cmos_checksum_verify();

	if (result)
		exit(1);
}

/****************************************************************************
 * op_cmos_show_all_params
 *
 * -a
 *
 * Show names and values for all parameters.
 ****************************************************************************/
static void op_cmos_show_all_params(void)
{
	int result;

	get_cmos_layout();
	result = list_all_params();
	cmos_checksum_verify();

	if (result)
		exit(1);
}

/****************************************************************************
 * op_cmos_set_one_param
 *
 * -w NAME=VALUE
 *
 * Set parameter NAME to VALUE.
 ****************************************************************************/
static void op_cmos_set_one_param(void)
{
	const char *name, *value;

	get_cmos_layout();

	/* Separate 'NAME=VALUE' syntax into two strings representing NAME and
	 * VALUE.
	 */
	parse_assignment(nvramtool_op.param, &name, &value);

	set_one_param(name, value);
}

/****************************************************************************
 * op_cmos_set_params_stdin
 *
 * -i
 *
 * Set parameters according to standard input.
 ****************************************************************************/
static void op_cmos_set_params_stdin(void)
{
	get_cmos_layout();
	set_params(stdin);
}

/****************************************************************************
 * op_cmos_set_params_file
 *
 * -p INPUT_FILE
 *
 * Set parameters according to INPUT_FILE.
 ****************************************************************************/
static void op_cmos_set_params_file(void)
{
	FILE *f;

	if ((f = fopen(nvramtool_op.param, "r")) == NULL) {
		fprintf(stderr, "%s: Can not open file %s for reading: %s\n",
			prog_name, nvramtool_op.param, strerror(errno));
		exit(1);
	}

	get_cmos_layout();
	set_params(f);
	fclose(f);
}

/****************************************************************************
 * op_cmos_checksum
 *
 * -c [VALUE]
 *
 * If VALUE is present, set coreboot CMOS checksum to VALUE.  Else show
 * checksum value.
 ****************************************************************************/
static void op_cmos_checksum(void)
{
	uint16_t checksum;

	get_cmos_layout();

	if (nvramtool_op.param == NULL) {
		set_iopl(3);
		checksum = cmos_checksum_read();
		set_iopl(0);
		printf("0x%x\n", checksum);
	} else {
		checksum = convert_checksum_value(nvramtool_op.param);
		set_iopl(3);
		cmos_checksum_write(checksum);
		set_iopl(0);
	}
}

/****************************************************************************
 * op_show_layout
 *
 * -Y
 *
 * Write CMOS layout information to standard output.
 ****************************************************************************/
static void op_show_layout(void)
{
	get_cmos_layout();
	write_cmos_layout(stdout);
}

/****************************************************************************
 * op_write_cmos_dump
 *
 * -b OUTPUT_FILE
 *
 * Write the contents of CMOS memory to a binary file.
 ****************************************************************************/
static void op_write_cmos_dump(void)
{
	unsigned char data[CMOS_SIZE];
	FILE *f;

	if ((f = fopen(nvramtool_op.param, "wb")) == NULL) {
		fprintf(stderr, "%s: Can not open file %s for writing: %s\n",
			prog_name, nvramtool_op.param, strerror(errno));
		exit(1);
	}

	set_iopl(3);
	cmos_read_all(data);
	set_iopl(0);

	if (fwrite(data, 1, CMOS_SIZE, f) != CMOS_SIZE) {
		fprintf(stderr, "%s: Error writing CMOS data to file %s: %s\n",
			prog_name, nvramtool_op.param, strerror(errno));
		exit(1);
	}

	fclose(f);
}

/****************************************************************************
 * op_read_cmos_dump
 *
 * -B INPUT_FILE
 *
 * Read binary data from a file and write the data to CMOS memory.
 ****************************************************************************/
static void op_read_cmos_dump(void)
{
	unsigned char data[CMOS_SIZE];
	size_t nr_bytes;
	FILE *f;

	if ((f = fopen(nvramtool_op.param, "rb")) == NULL) {
		fprintf(stderr, "%s: Can not open file %s for reading: %s\n",
			prog_name, nvramtool_op.param, strerror(errno));
		exit(1);
	}

	if ((nr_bytes = fread(data, 1, CMOS_SIZE, f)) != CMOS_SIZE) {
		fprintf(stderr,
			"%s: Error: Only able to read %d bytes of CMOS data "
			"from file %s.  CMOS data is unchanged.\n", prog_name,
			(int)nr_bytes, nvramtool_op.param);
		exit(1);
	}

	fclose(f);
	set_iopl(3);
	cmos_write_all(data);
	set_iopl(0);
}

/****************************************************************************
 * op_show_cmos_hex_dump
 *
 * -x
 *
 * Write a hex dump of CMOS memory to standard output.
 ****************************************************************************/
static void op_show_cmos_hex_dump(void)
{
	unsigned char data[CMOS_SIZE];

	set_iopl(3);
	cmos_read_all(data);
	set_iopl(0);
	hexdump(data, CMOS_SIZE, 0, stdout, &cmos_dump_format);
}

/****************************************************************************
 * op_show_cmos_dumpfile
 *
 * -X DUMP_FILE
 *
 * Read binary data from a file (presumably a CMOS dump file) and display a
 * hex dump of the CMOS data from the file.
 ****************************************************************************/
static void op_show_cmos_dumpfile(void)
{
	unsigned char data[CMOS_SIZE];
	size_t nr_bytes;
	FILE *f;

	if ((f = fopen(nvramtool_op.param, "r")) == NULL) {
		fprintf(stderr, "%s: Can not open file %s for reading: %s\n",
			prog_name, nvramtool_op.param, strerror(errno));
		exit(1);
	}

	nr_bytes = fread(data, 1, CMOS_SIZE, f);
	fclose(f);
	hexdump(data, nr_bytes, 0, stdout, &cmos_dump_format);
}

/****************************************************************************
 * list_one_param
 *
 * Attempt to list one CMOS parameter given by 'name'.  'show_name' is a
 * boolean value indicating whether the parameter name should be displayed
 * along with its value.  Return 1 if error was encountered.  Else return OK.
 ****************************************************************************/
static int list_one_param(const char name[], int show_name)
{
	const cmos_entry_t *e;

	if (is_checksum_name(name) || ((e = find_cmos_entry(name)) == NULL)) {
		fprintf(stderr, "%s: CMOS parameter %s not found.\n", prog_name,
			name);
		exit(1);
	}

	if (e->config == CMOS_ENTRY_RESERVED) {
		fprintf(stderr, "%s: Parameter %s is reserved.\n", prog_name,
			name);
		exit(1);
	}

	return (list_cmos_entry(e, show_name) != 0);
}

/****************************************************************************
 * list_all_params
 *
 * Attempt to list all CMOS parameters.  Return 1 if error was encountered.
 * Else return OK.
 ****************************************************************************/
static int list_all_params(void)
{
	const cmos_entry_t *e;
	int result;

	result = OK;

	for (e = first_cmos_entry(); e != NULL; e = next_cmos_entry(e)) {
		if ((e->config == CMOS_ENTRY_RESERVED)
		    || is_checksum_name(e->name))
			continue;

		if (list_cmos_entry(e, TRUE))
			result = 1;
	}

	return result;
}

/****************************************************************************
 * list_param_enums
 *
 * List all possible values for CMOS parameter given by 'name'.
 ****************************************************************************/
static void list_param_enums(const char name[])
{
	const cmos_entry_t *e;
	const cmos_enum_t *p;

	if (is_checksum_name(name) || (e = find_cmos_entry(name)) == NULL) {
		fprintf(stderr, "%s: CMOS parameter %s not found.\n", prog_name,
			name);
		exit(1);
	}

	switch (e->config) {
	case CMOS_ENTRY_ENUM:
		for (p = first_cmos_enum_id(e->config_id);
		     p != NULL; p = next_cmos_enum_id(p))
			printf("%s\n", p->text);

		break;

	case CMOS_ENTRY_HEX:
		printf("Parameter %s requires a %u-bit unsigned integer.\n",
		       name, e->length);
		break;

	case CMOS_ENTRY_STRING:
		printf("Parameter %s requires a %u-byte string.\n", name,
		       e->length / 8);
		break;

	case CMOS_ENTRY_RESERVED:
		printf("Parameter %s is reserved.\n", name);
		break;

	default:
		BUG();
	}
}

/****************************************************************************
 * set_one_param
 *
 * Set the CMOS parameter given by 'name' to 'value'.  The 'name' parameter
 * is case-sensitive.  If we are setting an enum parameter, then 'value' is
 * interpreted as a case-sensitive string that must match the option name
 * exactly.  If we are setting a 'hex' parameter, then 'value' is treated as
 * a string representation of an unsigned integer that may be specified in
 * decimal, hex, or octal.
 ****************************************************************************/
static void set_one_param(const char name[], const char value[])
{
	const cmos_entry_t *e;
	unsigned long long n;

	if (is_checksum_name(name) || (e = find_cmos_entry(name)) == NULL) {
		fprintf(stderr, "%s: CMOS parameter %s not found.", prog_name,
			name);
		exit(1);
	}

	switch (prepare_cmos_write(e, value, &n)) {
	case OK:
		break;

	case CMOS_OP_BAD_ENUM_VALUE:
		fprintf(stderr, "%s: Bad value for parameter %s.", prog_name,
			name);
		goto fail;

	case CMOS_OP_NEGATIVE_INT:
		fprintf(stderr,
			"%s: This program does not support assignment of negative "
			"numbers to coreboot parameters.", prog_name);
		goto fail;

	case CMOS_OP_INVALID_INT:
		fprintf(stderr, "%s: %s is not a valid integer.", prog_name,
			value);
		goto fail;

	case CMOS_OP_RESERVED:
		fprintf(stderr,
			"%s: Can not modify reserved coreboot parameter %s.",
			prog_name, name);
		goto fail;

	case CMOS_OP_VALUE_TOO_WIDE:
		fprintf(stderr,
			"%s: Can not write value %s to CMOS parameter %s that is "
			"only %d bits wide.", prog_name, value, name,
			e->length);
		goto fail;

	case CMOS_OP_NO_MATCHING_ENUM:
		fprintf(stderr,
			"%s: coreboot parameter %s has no matching enums.",
			prog_name, name);
		goto fail;

	case CMOS_AREA_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: The CMOS area specified by the layout info for "
			"coreboot parameter %s is out of range.", prog_name,
			name);
		goto fail;

	case CMOS_AREA_OVERLAPS_RTC:
		fprintf(stderr,
			"%s: The CMOS area specified by the layout info for "
			"coreboot parameter %s overlaps the realtime clock area.",
			prog_name, name);
		goto fail;

	case CMOS_AREA_TOO_WIDE:
		fprintf(stderr,
			"%s: The CMOS area specified by the layout info for "
			"coreboot parameter %s is too wide.", prog_name, name);
		goto fail;

	default:
		fprintf(stderr,
			"%s: Unknown error encountered while attempting to modify "
			"coreboot parameter %s.", prog_name, name);
		goto fail;
	}

	/* write the value to nonvolatile RAM */
	set_iopl(3);
	cmos_write(e, n);
	cmos_checksum_write(cmos_checksum_compute());
	set_iopl(0);
	return;

      fail:
	fprintf(stderr, "  CMOS write not performed.\n");
	exit(1);
}

/****************************************************************************
 * set_params
 *
 * Set coreboot parameters according to the contents of file 'f'.
 ****************************************************************************/
static void set_params(FILE * f)
{				/* First process the input file.  Then perform writes only if there were
				 * no problems processing the input.  Either all values will be written
				 * successfully or no values will be written.
				 */
	do_cmos_writes(process_input_file(f));
}

/****************************************************************************
 * parse_assignment
 *
 * Parse the string 'arg' (which supposedly represents an assignment) into a
 * NAME and a VALUE.  If 'arg' does not conform to the proper assignment
 * syntax, exit with a usage message.  Otherwise, on return, 'arg' is broken
 * into substrings representing NAME and VALUE, and *name and *value are set
 * to point to these two substrings.
 ****************************************************************************/
static void parse_assignment(char arg[], const char **name, const char **value)
{
	static const size_t N_MATCHES = 4;
	regmatch_t match[N_MATCHES];
	regex_t assignment;

	compile_reg_expr(REG_EXTENDED | REG_NEWLINE, assignment_regex, &assignment);

	/* Does 'arg' conform to proper assignment syntax?  If not, exit with a
	 * usage message.
	 */
	if (regexec(&assignment, arg, N_MATCHES, match, 0))
		usage(stderr);

	/* Ok, we found a valid assignment.  Break it into two strings
	 * representing NAME and VALUE.
	 */
	arg[match[1].rm_eo] = '\0';
	arg[match[2].rm_eo] = '\0';
	*name = &arg[match[1].rm_so];
	*value = &arg[match[2].rm_so];

	regfree(&assignment);
}

/****************************************************************************
 * list_cmos_entry
 *
 * Attempt to list the CMOS entry represented by 'e'.  'show_name' is a
 * boolean value indicating whether the parameter name should be displayed
 * along with its value.  On success, return OK.  On error, print an error
 * message and return 1.
 ****************************************************************************/
static int list_cmos_entry(const cmos_entry_t * e, int show_name)
{
	const cmos_enum_t *p;
	unsigned long long value;
	char *w;

	/* sanity check CMOS entry */
	switch (prepare_cmos_read(e)) {
	case OK:
		break;

	case CMOS_OP_RESERVED:
		BUG();

	case CMOS_AREA_OUT_OF_RANGE:
		fprintf(stderr,
			"%s: Can not read coreboot parameter %s because "
			"layout info specifies out of range CMOS area.\n",
			prog_name, e->name);
		return 1;

	case CMOS_AREA_OVERLAPS_RTC:
		fprintf(stderr,
			"%s: Can not read coreboot parameter %s because "
			"layout info specifies CMOS area that overlaps realtime "
			"clock area.\n", prog_name, e->name);
		return 1;

	case CMOS_AREA_TOO_WIDE:
		fprintf(stderr,
			"%s: Can not read coreboot parameter %s because "
			"layout info specifies CMOS area that is too wide.\n",
			prog_name, e->name);
		return 1;

	default:
		fprintf(stderr,
			"%s: Unknown error encountered while attempting to "
			"read coreboot parameter %s\n", prog_name, e->name);
		return 1;
	}

	/* read the value from CMOS */
	set_iopl(3);
	value = cmos_read(e);
	set_iopl(0);

	/* display the value */
	switch (e->config) {
	case CMOS_ENTRY_ENUM:
		if ((p = find_cmos_enum(e->config_id, value)) == NULL) {
			if (show_name)
				printf("# Bad value -> %s = 0x%llx\n", e->name,
				       value);
			else
				printf("Bad value -> 0x%llx\n", value);
		} else {
			if (show_name)
				printf("%s = %s\n", e->name, p->text);
			else
				printf("%s\n", p->text);
		}

		break;

	case CMOS_ENTRY_HEX:
		if (show_name)
			printf("%s = 0x%llx\n", e->name, value);
		else
			printf("0x%llx\n", value);

		break;

	case CMOS_ENTRY_STRING:
		w = (char *)(unsigned long)value;
		while (*w) {
			if(!isprint((int)(unsigned char)*w)) {
				if (show_name)
					printf("# Bad value -> %s\n", e->name);
				else
					printf("Bad value\n");
				break;
			}
			w++;
		}

		if (!*w) {

			if (show_name)
				printf("%s = %s\n", e->name,
				       (char *)(unsigned long)value);
			else
				printf("%s\n", (char *)(unsigned long)value);
		}

		free((void *)(unsigned long)value);

		break;

	case CMOS_ENTRY_RESERVED:
	default:
		BUG();
	}

	return OK;
}

/****************************************************************************
 * convert_checksum_value
 *
 * 'value' is the string representation of a checksum value that the user
 * wishes to set using the -c option.  Convert the string to a 16-bit
 * unsigned integer and return the result.  Exit with an error message if
 * 'value' is invalid.
 ****************************************************************************/
static uint16_t convert_checksum_value(const char value[])
{
	unsigned long n;
	const char *p;
	uint16_t result;
	int negative;

	for (p = value; isspace((int)(unsigned char)*p); p++) ;

	negative = (*p == '-');
	n = strtoul(value, (char **)&p, 0);

	if (*p) {
		fprintf(stderr,
			"%s: Checksum value %s is not a valid integer.\n",
			prog_name, value);
		exit(1);
	}

	if (negative) {
		fprintf(stderr,
			"%s: Checksum must be an unsigned integer.\n",
			prog_name);
		exit(1);
	}

	result = (uint16_t) n;

	if (result != n) {
		fprintf(stderr,
			"%s: Checksum value must fit within 16 bits.\n",
			prog_name);
		exit(1);
	}

	return result;
}
