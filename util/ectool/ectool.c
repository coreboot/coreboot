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
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#ifndef __NetBSD__
#include <sys/io.h>
#endif
#include <ec.h>
#include <stdlib.h>

#ifdef __NetBSD__
#include <machine/sysarch.h>
#endif


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
	"GNU General Public License for more details.\n\n");
}

void print_usage(const char *name)
{
	printf("usage: %s [-vh?Vidq] [-w 0x<addr> -z 0x<data>]\n", name);
	printf("\n"
	       "   -v | --version:                   print the version\n"
	       "   -h | --help:                      print this help\n\n"
	       "   -V | --verbose:                   print debug information\n"
	       "   -d | --dump:                      print RAM\n"
	       "   -i | --idx:                       print IDX RAM & RAM\n"
	       "   -q | --query:                     print query byte\n"
	       "   -w <addr in hex>                  write to addr\n"
	       "   -z <data in hex>                  write to data\n"
	       "\n");
	exit(1);
}

int verbose = 0, dump_idx = 0, dump_ram = 0, dump_query = 0;

int main(int argc, char *argv[])
{
	int i, opt, option_index = 0;
	long write_data = -1;
	long write_addr = -1;

	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"verbose", 0, 0, 'V'},
		{"idx", 0, 0, 'i'},
		{"query", 0, 0, 'q'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?Vidqw:z:",
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
			dump_ram = 1;
			break;
		case 'w':
			write_addr = strtol(optarg , NULL, 16);
			break;
		case 'z':
			write_data = strtol(optarg , NULL, 16);
			break;
		case 'd':
			dump_ram = 1;
			break;
		case 'q':
			dump_query = 1;
			break;
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			exit(0);
			break;
		}
	}

#ifdef __NetBSD__
# ifdef __i386__
	if (i386_iopl(3)) {
# else
	if (x86_64_iopl(3)) {
# endif
#else
	if (iopl(3)) {
#endif
		printf("You need to be root.\n");
		exit(1);
	}
	if (write_addr >= 0 && write_data >= 0) {
		write_addr &= 0xff;
		write_data &= 0xff;
		printf("\nWriting ec %02lx = %02lx\n", write_addr & 0xff, write_data & 0xff);
		ec_write(write_addr & 0xff, write_data & 0xff);
	}

	/* preserve default - dump_ram if nothing selected */
	if (!dump_ram && !dump_idx && !dump_query && !write_addr) {
		dump_ram = 1;
	}

	if (dump_ram) {
		printf("EC RAM:\n");
		for (i = 0; i < 0x100; i++) {
			if ((i % 0x10) == 0)
				printf("\n%02x: ", i);
			printf("%02x ", ec_read(i));
		}
		printf("\n\n");
	}

	if (dump_query) {
		printf("EC QUERY %02x\n", ec_query());
	}

	if (dump_idx) {
		printf("EC IDX RAM:\n");
		for (i = 0; i < 0x10000; i++) {
			if ((i % 0x10) == 0)
				printf("\n%04x: ", i);
			printf("%02x ", ec_idx_read(i));
		}
		printf("\n\n");
	}

	return 0;
}
