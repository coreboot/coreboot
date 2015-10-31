/*
 * cbfstool, CLI utility for creating rmodules
 *
 * Copyright (C) 2014 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "common.h"
#include "rmodule.h"

static const char *optstring  = "i:o:vh?";
static struct option long_options[] = {
	{"inelf",        required_argument, 0, 'i' },
	{"outelf",       required_argument, 0, 'o' },
	{"verbose",      no_argument,       0, 'v' },
	{"help",         no_argument,       0, 'h' },
	{NULL,           0,                 0,  0  }
};

static void usage(char *name)
{
	printf(
		"rmodtool: utility for creating rmodules\n\n"
		"USAGE: %s [-h] [-v] <-i|--inelf name> <-o|--outelf name>\n",
		name
	);
}

int main(int argc, char *argv[])
{
	int c;
	struct buffer elfin;
	struct buffer elfout;
	const char *input_file = NULL;
	const char *output_file = NULL;

	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	while (1) {
		int optindex = 0;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case 'i':
			input_file = optarg;
			break;
		case 'h':
			usage(argv[0]);
			return 1;
		case 'o':
			output_file = optarg;
			break;
		case 'v':
			verbose++;
			break;
		default:
			break;
		}
	}

	if (input_file == NULL || output_file == NULL) {
		usage(argv[0]);
		return 1;
	}

	if (buffer_from_file(&elfin, input_file)) {
		ERROR("Couldn't read in file '%s'.\n", input_file);
		return 1;
	}

	if (rmodule_create(&elfin, &elfout)) {
		ERROR("Unable to create rmodule from '%s'.\n", input_file);
		return 1;
	}

	if (buffer_write_file(&elfout, output_file)) {
		ERROR("Unable to write rmodule elf '%s'.\n", output_file);
		return 1;
	}

	return 0;
}
