/*
 * This file is part of the ectool project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/io.h>
#include <ec.h>

#define ECTOOL_VERSION "0.1"

void print_version(void)
{
	printf("ectool v%s -- ", ECTOOL_VERSION);
	printf("Copyright (C) 2008-2009 coresystems GmbH\n\n");
	printf(
	"This program is free software: you can redistribute it and/or modify\n"
	"it under the terms of the GNU General Public License as published by\n"
	"the Free Software Foundation, version 2 of the License.\n\n"
	"This program is distributed in the hope that it will be useful,\n"
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	"GNU General Public License for more details.\n\n"
	"You should have received a copy of the GNU General Public License\n"
	"along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
}

void print_usage(const char *name)
{
 	printf("usage: %s [-vh?Vi]\n", name);
	printf("\n"
	       "   -v | --version:                   print the version\n"
	       "   -h | --help:                      print this help\n\n"
	       "   -V | --verbose:                   print debug information\n"
	       "   -i | --idx:                       print IDX RAM\n"
	       "\n");
	exit(1);
}

int verbose = 0, dump_idx = 0;

int main(int argc, char *argv[])
{
	int i, opt, option_index = 0;

	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"verbose", 0, 0, 'V'},
		{"idx", 0, 0, 'i'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?Vi",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 'V':
			verbose = 1;
			break;
		case 'i':
			dump_idx = 1;
			break;
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			exit(0);
			break;
		}
	}

	if (iopl(3)) {
		printf("You need to be root.\n");
		exit(1);
	}

	printf("EC RAM:\n");
	for (i = 0; i < 0x100; i++) {
		if ((i % 0x10) == 0)
			printf("\n%02x: ", i);
		printf("%02x ", ec_read(i));
	}
	printf("\n\n");

	if (dump_idx) {
		printf("EC IDX RAM:\n");
		for (i = 0; i < 0x10000; i++) {
			if ((i % 0x10) == 0)
				printf("\n%04x: ", i);
			printf("%02x ", ec_idx_read(i));
		}
		printf("\n\n");
	} else {
		printf("Not dumping EC IDX RAM.\n");
	}

	return 0;
}
