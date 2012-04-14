/*****************************************************************************\
 * common.c
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

#include "common.h"

/* basename of this program, as reported by argv[0] */
const char prog_name[] = "nvramtool";

/* version of this program */
const char prog_version[] = "2.1";

/****************************************************************************
 * get_line_from_file
 *
 * Get a line of input from file 'f'.  Store result in 'line' which is an
 * array of 'line_buf_size' bytes.
 ****************************************************************************/
int get_line_from_file(FILE * f, char line[], int line_buf_size)
{
	if (fgets(line, line_buf_size, f) == NULL)
		return LINE_EOF;

	/* If the file contains a line that is too long, then it's best
	 * to let the user know right away rather than passing back a
	 * truncated result that will lead to problems later on.
	 */
	return (strlen(line) == ((size_t) (line_buf_size - 1))) ?
	    LINE_TOO_LONG : OK;
}

/****************************************************************************
 * out_of_memory
 *
 * We ran out of memory.  Print an error message and die.
 ****************************************************************************/
void out_of_memory(void)
{
	fprintf(stderr, "%s: Out of memory.\n", prog_name);
	exit(1);
}

/****************************************************************************
 * usage
 *
 * Write a usage message to 'outfile'.  If 'outfile' is 'stderr' then exit
 * with a value of 1.  Otherwise exit with a value of 0.
 ****************************************************************************/
void usage(FILE * outfile)
{
	fprintf(outfile,
		"Usage: %s [-y LAYOUT_FILE | -t] PARAMETER ...\n\n"
		"       Read/write coreboot parameters or show info from "
		"coreboot table.\n\n"
		"       -y LAYOUT_FILE: Use CMOS layout specified by "
		"LAYOUT_FILE.\n"
		"       -t:             Use CMOS layout specified by CMOS option "
		"table.\n"
		"       -C CBFS_FILE:   Use CBFS file for layout and CMOS data.\n"
		"       -D CMOS_FILE:   Use CMOS file for CMOS data (overrides CMOS of -C).\n"
		"       [-n] -r NAME:   Show parameter NAME.  If -n is given, "
		"show value only.\n"
		"       -e NAME:        Show all possible values for parameter "
		"NAME.\n"
		"       -a:             Show names and values for all "
		"parameters.\n"
		"       -w NAME=VALUE:  Set parameter NAME to VALUE.\n"
		"       -p INPUT_FILE:  Set parameters according to INPUT_FILE.\n"
		"       -i:             Same as -p but file contents taken from "
		"standard input.\n"
		"       -c [VALUE]:     Show CMOS checksum or set checksum to "
		"VALUE.\n"
		"       -l [ARG]:       Show coreboot table info for ARG, or "
		"all ARG choices.\n"
		"       -L OUTPUT_BIN   Write CMOS layout file in binary format\n"
		"       -H OUTPUT_HDR   Write CMOS layout file in header format\n"
		"       -d:             Show low-level dump of coreboot table.\n"
		"       -Y:             Show CMOS layout info.\n"
		"       -b OUTPUT_FILE: Dump CMOS memory contents to file.\n"
		"       -B INPUT_FILE:  Write file contents to CMOS memory.\n"
		"       -x:             Show hex dump of CMOS memory.\n"
		"       -X DUMPFILE:    Show hex dump of CMOS dumpfile.\n"
		"       -v:             Show version info for this program.\n"
		"       -h:             Show this message.\n", prog_name);
	exit(outfile == stderr);
}
