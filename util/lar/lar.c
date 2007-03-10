/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006-2007 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "lib.h"
#include "lar.h"

#define VERSION "v0.9"
#define COPYRIGHT "Copyright (C) 2006-2007 coresystems GmbH"

static int isverbose = 0;
static long larsize = 0;
static char *bootblock = NULL;

static void usage(char *name)
{
	printf("\nLAR - the LinuxBIOS Archiver " VERSION "\n" COPYRIGHT "\n\n"
	       "Usage: %s [-cxl] archive.lar [[[file1] file2] ...]\n\n", name);
}

int verbose(void)
{
	return isverbose;
}

long get_larsize(void)
{
	return larsize;
}

char *get_bootblock(void)
{
	return bootblock;
}

int main(int argc, char *argv[])
{
	int opt;
	int option_index = 0;

	int larmode = NONE;

	char *archivename = NULL;

	static struct option long_options[] = {
		{"create", 0, 0, 'c'},
		{"extract", 0, 0, 'x'},
		{"list", 0, 0, 'l'},
		{"size", 1, 0, 's'},
		{"bootblock", 1, 0, 'b'},
		{"verbose", 0, 0, 'v'},
		{"version", 0, 0, 'V'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};

	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}

	while ((opt = getopt_long(argc, argv, "cxls:b:vVh?",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'c':
			larmode = CREATE;
			break;
		case 'l':
			larmode = LIST;
			break;
		case 'x':
			larmode = EXTRACT;
			break;
		case 's':
			larsize = strtol(optarg, (char **)NULL, 10);
			break;
		case 'b':
			printf("Bootblock handling not yet supported. Ignoring.\n");
			bootblock = strdup(optarg);
			if (!bootblock) {
				fprintf(stderr, "Out of memory.\n");
				exit(1);
			}
			break;
		case 'v':
			isverbose = 1;
			break;
		case 'V':
			printf("LAR - the LinuxBIOS Archiver " VERSION "\n");
			break;
		default:
			usage(argv[0]);
			exit(1);
		}
	}

	// tar compatibility: Allow lar x as alternative to
	// lar -x.  This should be dropped or done correctly.
	// Right now, you'd have to write lar x -v instead of
	// lar xv... but the author of this software was too
	// lazy to handle all option parameter twice.
	if (larmode == NONE) {
		if (strncmp(argv[optind], "x", 2) == 0)
			larmode = EXTRACT;
		else if (strncmp(argv[optind], "c", 2) == 0)
			larmode = CREATE;
		else if (strncmp(argv[optind], "l", 2) == 0)
			larmode = LIST;

		/* If larmode changed in this if branch,
		 * eat a parameter
		 */
		if (larmode != NONE)
			optind++;
	}

	if (larmode == NONE) {
		usage(argv[0]);
		printf("Error: No mode specified.\n\n");
		exit(1);
	}

	/* size only makes sense when creating a lar */
	if (larmode != CREATE && larsize) {
		printf("Warning: size parameter ignored since "
		       "not creating an archive.\n");
	}

	if (optind < argc) {
		archivename = argv[optind++];
	} else {

		usage(argv[0]);
		printf("Error: No archive name.\n\n");
		exit(1);
	}

	/* when a new archive is created, recurse over
	 * physical files when a directory is found.
	 * Otherwise just add the directory to the match list
	 */

	while (optind < argc) {
		if (larmode == CREATE) {
			add_files(argv[optind++]);
		} else
			add_file_or_directory(argv[optind++]);
	}

	switch (larmode) {
	case EXTRACT:
		extract_lar(archivename, get_files());
		break;
	case CREATE:
		create_lar(archivename, get_files());
		break;
	case LIST:
		list_lar(archivename, get_files());
		break;
	}

	free_files();
	return 0;
}
