/*
 * This file is part of the bayou project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "pbuilder.h"

static void usage(void)
{
	printf("./pbuilder [-c config] [create|show] [LAR]\n");
}

int main(int argc, char **argv)
{
	char *config = NULL;

	while (1) {
		signed ch = getopt(argc, argv, "c:");
		if (ch == -1)
			break;

		switch (ch) {
		case 'c':
			config = optarg;
			break;
		default:
			usage();
			return -1;
		}
	}

	if (optind >= argc) {
		usage();
		return 0;
	}

	if (!strcmp(argv[optind], "create")) {
		if (config == NULL) {
			warn("E: No config was provided\n");
			usage();
			return -1;
		}

		if (optind + 1 >= argc) {
			warn("E: No LAR name was given\n");
			usage();
			return -1;
		}

		create_lar_from_config((const char *)config,
				       (const char *)argv[optind + 1]);
	} else if (!strcmp(argv[optind], "show")) {
		if (optind + 1 >= argc) {
			warn("E: No LAR name was given\n");
			usage();
			return -1;
		}
		pbuilder_show_lar((const char *)argv[optind + 1]);
	} else {
		usage();
		return -1;
	}

	return 0;
}
