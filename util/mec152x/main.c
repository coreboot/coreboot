/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rom.h"

static const char *USAGE_FMT = "Usage: %s <rom-file> <command>\n"
			       "        -h|--help\n"
			       "        -f|--fmap_region_name\n\n"
			       "Command:\n"
			       " GEN_ECFW_PTR - Writes the ECFW PTR\n";

static const char *program_name;

static void print_program_usage(void)
{
	fprintf(stderr, USAGE_FMT, program_name);
	exit(EXIT_FAILURE);
}

#define ERROR(...) do { fprintf(stderr, "ERROR: " __VA_ARGS__); \
			print_program_usage(); } \
			while (0)

static void print_help(void)
{
	printf(USAGE_FMT, program_name);
}

static const struct option longopts[] = {
	{"help", 0, 0, 'h'},
	{"fmap_region_name", 1, 0, 'f'},
	{NULL, 0, 0, 0}
};

int main(int argc, char *argv[])
{
	char *fmap_region_name = NULL;
	const char *rom_file = NULL;
	const char *command = NULL;

	int opt, idx, ret = 0;
	program_name = argv[0];

	if (argc < 3)
		ERROR("Not enough argument given\n\n");

	while (!ret &&
		(opt = getopt_long(argc, argv, "?hf:",
				   longopts, &idx)) != -1) {
		switch (opt) {
		case 'h':
		case '?':
			print_help();
			exit(EXIT_SUCCESS);
			break;
		case 'f':
			fmap_region_name = strdup(optarg);
			break;
		}
	}
	if (optind < argc) {
		rom_file = argv[optind];
		optind++;
	}
	if (optind < argc) {
		command = argv[optind];
		optind++;
	}

	if (optind < argc)
		ERROR("Unknown argument: %s\n\n", argv[optind]);

	if (!rom_file)
		ERROR("No ROM specified\n\n");

	if (!command) {
		ERROR("No command specified\n\n");
	} else if (!strcmp(command, "GEN_ECFW_PTR")) {
		if (!fmap_region_name)
			ERROR("No fmap region specified\n\n");
		if (!rom_set_ec_fw_ptr(rom_file, fmap_region_name))
			ERROR("Failed to write EC FW PTR\n\n");
	} else {
		ERROR("Unknown command specified: '%s'\n\n", command);
	}

	if (fmap_region_name)
		free(fmap_region_name);

	return ret;
}
