/*
 * cbfstool, CLI utility for CBFS file manipulation
 *
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 * Copyright (C) 2012 Google, Inc.
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
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
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include "common.h"

struct command {
	const char *name;
	const char *optstring;
	int (*function) (void);
};

int verbose = 0;

struct param {
	char *cbfs_name;
	char *rom_name;
	char *rom_filename;
	char *rom_bootblock;
	uint32_t rom_type;
	uint32_t rom_baseaddress;
	uint32_t rom_loadaddress;
	uint32_t rom_bootaddress;
	uint32_t rom_headeraddress;
	uint32_t rom_entrypoint;
	uint32_t rom_size;
	uint32_t rom_alignment;
	uint32_t rom_offset;
	comp_algo rom_algo;
} param;

typedef int (*convert_buffer_t)(struct buffer *buffer);

static int cbfs_add_file(const char *rom_filename,
			 const char *rom_name,
			 uint32_t rom_type,
			 convert_buffer_t convert,
			 const char *cbfs_name) {
	struct cbfs_image image;
	struct buffer buffer;
	int result = 1;

	if (!rom_filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (rom_type == 0) {
		fprintf(stderr, "E: You need to specify a valid -t/--type.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, cbfs_name) != 0) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	if (buffer_from_file(&buffer, rom_filename) != 0) {
		fprintf(stderr, "E: Could not load file '%s'.\n", rom_filename);
		cbfs_image_delete(&image);
		return 1;
	}


	do {
		if (convert && convert(&buffer) != 0) {
			fprintf(stderr, "E: Failed to parse file '%s'.\n",
				rom_filename);
			break;
		}
		if (cbfs_add_entry(&image, rom_name, rom_type,
				   &buffer) != 0)
			break;
		if (cbfs_image_write_file(&image, cbfs_name) != 0)
			break;

		result = 0;
	} while (0);

	buffer_delete(&buffer);
	cbfs_image_delete(&image);
	return result;
}

static int cbfs_convert_mkstage(struct buffer *buffer) {
	char *name;
	char *output;
	int result = parse_elf_to_stage(buffer->data, &output, param.rom_algo,
					&param.rom_baseaddress);
	if (result < 0)
		return -1;
	name = strdup(buffer->name);
	buffer_delete(buffer);
	buffer->name = name;
	buffer->size = (size_t)result;
	buffer->data = (char*)output;
	return 0;
}

static int cbfs_convert_elf_mkpayload(struct buffer *buffer) {
	char *name;
	char *output;
	int result = parse_elf_to_payload(buffer->data, &output,
					  param.rom_algo);
	if (result < 0)
		return -1;
	name = strdup(buffer->name);
	buffer_delete(buffer);
	buffer->name = name;
	buffer->size = (size_t)result;
	buffer->data = (char*)output;
	return 0;
}

static int cbfs_convert_flat_mkpayload(struct buffer *buffer) {
	char *name;
	char *output;
	int result;
	result = parse_flat_binary_to_payload(buffer->data, &output,
					      buffer->size,
					      param.rom_loadaddress,
					      param.rom_entrypoint,
					      param.rom_algo);
	if (result < 0)
		return -1;
	name = strdup(buffer->name);
	buffer_delete(buffer);
	buffer->name = name;
	buffer->size = (size_t)result;
	buffer->data = (char*)output;
	return 0;
}

static int cbfs_add(void)
{
	// TODO where is rom_baseaddress?
	return cbfs_add_file(param.rom_filename,
			     param.rom_name,
			     param.rom_type,
			     NULL,
			     param.cbfs_name);
}


static int cbfs_add_payload(void)
{
	return cbfs_add_file(param.rom_filename,
			     param.rom_name,
			     CBFS_COMPONENT_PAYLOAD,
			     cbfs_convert_elf_mkpayload,
			     param.cbfs_name);
}

static int cbfs_add_stage(void)
{
	return cbfs_add_file(param.rom_filename,
			     param.rom_name,
			     CBFS_COMPONENT_STAGE,
			     cbfs_convert_mkstage,
			     param.cbfs_name);
}

static int cbfs_add_flat_binary(void)
{
	if (param.rom_loadaddress == 0) {
		fprintf(stderr, "E: You need to specify a valid "
			"-l/--load-address.\n");
		return 1;
	}
	if (param.rom_entrypoint == 0) {
		fprintf(stderr, "E: You need to specify a valid "
			"-e/--entry-point.\n");
		return 1;
	}
	return cbfs_add_file(param.rom_filename,
			     param.rom_name,
			     CBFS_COMPONENT_PAYLOAD,
			     cbfs_convert_flat_mkpayload,
			     param.cbfs_name);
}

static int cbfs_remove(void)
{
	int result = 1;
	struct cbfs_image image;

	if (!param.rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	do {
		if (cbfs_remove_entry(&image, param.rom_name) != 0) {
			fprintf(stderr, "E: Removing file '%s' failed.\n",
				param.rom_name);
			break;
		}
		if (cbfs_image_write_file(&image, param.cbfs_name) != 0)
			break;
		result = 0;
	} while (0);

	cbfs_image_delete(&image);
	return result;
}

static int cbfs_create(void)
{
	struct cbfs_image image;
	struct buffer bootblock;

	if (param.rom_size == 0) {
		fprintf(stderr, "E: You need to specify a valid -s/--size.\n");
		return 1;
	}

	if (!param.rom_bootblock) {
		fprintf(stderr, "E: You need to specify -b/--bootblock.\n");
		return 1;
	}

	if (buffer_from_file(&bootblock, param.rom_bootblock) != 0) {
		return 1;
	}

	if (cbfs_create_image(&image,
			      param.rom_size,
			      &bootblock,
			      param.rom_alignment,
			      param.rom_bootaddress,
			      param.rom_headeraddress,
			      param.rom_offset) != 0) {
		fprintf(stderr, "E: Failed to create %s.\n", param.cbfs_name);
		return 1;
	}
	if (cbfs_image_write_file(&image, param.cbfs_name) != 0) {
		fprintf(stderr, "E: Failed to write %s.\n", param.cbfs_name);
		return 1;
	}
	cbfs_image_delete(&image);
	return 0;

}

static int cbfs_locate(void)
{
#if 0
	struct cbfs_image image;
	uint32_t filesize, location;

	if (!param.rom_filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	filesize = getfilesize(param.rom_filename);

	location = cbfs_find_location(param.cbfs_name, filesize,
					param.rom_name, param.rom_alignment);

	printf("%x\n", location);
	return location == 0 ? 1 : 0;
#endif
	return 0;
}

static int cbfs_print(void)
{
	struct cbfs_image image;
	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}
	cbfs_print_header_info(&image);
	printf("%-30s %-10s %-12s Size\n", "Name", "Offset", "Type");
	cbfs_walk(&image, cbfs_print_entry_info, &verbose);

	cbfs_image_delete(&image);
	return 0;
}

static int cbfs_extract(void)
{
	int result = 0;
	struct cbfs_image image;

	if (!param.rom_filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		result = 1;
	} else if (cbfs_export_entry(&image, param.rom_name,
				     param.rom_filename) != 0) {
		result = 1;
	}

	cbfs_image_delete(&image);
	return result;
}

static const struct command commands[] = {
	{"add", "vf:n:t:b:h?", cbfs_add},
	{"add-payload", "vf:n:t:c:b:h?", cbfs_add_payload},
	{"add-stage", "vf:n:t:c:b:h?", cbfs_add_stage},
	{"add-flat-binary", "vf:n:l:e:c:b:h?", cbfs_add_flat_binary},
	{"remove", "vn:h?", cbfs_remove},
	{"create", "vs:B:A:H:a:o:m:h?", cbfs_create},
	{"locate", "vf:n:a:h?", cbfs_locate},
	{"print", "vh?", cbfs_print},
	{"extract", "vn:f:h?", cbfs_extract},
};

static struct option long_options[] = {
	{"name",         required_argument, 0, 'n' },
	{"type",         required_argument, 0, 't' },
	{"compression",  required_argument, 0, 'c' },
	{"base-address", required_argument, 0, 'b' },
	{"load-address", required_argument, 0, 'l' },
	{"entry-point",  required_argument, 0, 'e' },
	{"size",         required_argument, 0, 's' },
	{"bootblock",    required_argument, 0, 'B' },
	{"boot-address", required_argument, 0, 'A' },
	{"header-address",required_argument,0, 'H' },
	{"alignment",    required_argument, 0, 'a' },
	{"offset",       required_argument, 0, 'o' },
	{"file",         required_argument, 0, 'f' },
	{"arch",         required_argument, 0, 'm' },
	{"verbose",      no_argument,       0, 'v' },
	{"help",         no_argument,       0, 'h' },
	{NULL,           0,                 0,  0  }
};

static void usage(char *name)
{
	printf
	    ("cbfstool: Management utility for CBFS formatted ROM images\n\n"
	     "USAGE:\n" " %s [-h]\n"
	     " %s FILE COMMAND [-v] [PARAMETERS]...\n\n" "OPTIONs:\n"
	     "  -h             Display this help message\n\n"
	     "  -v             Verbose output\n"
	     "COMMANDs:\n"
	     " add -f FILE -n NAME -t TYPE [-b base-address]               "
			"Add a component\n"
	     " add-payload -f FILE -n NAME [-c compression] [-b base]      "
			"Add a payload to the ROM\n"
	     " add-stage -f FILE -n NAME [-c compression] [-b base]        "
			"Add a stage to the ROM\n"
	     " add-flat-binary -f FILE -n NAME -l load-address \\\n"
	     "        -e entry-point [-c compression] [-b base]            "
			"Add a 32bit flat mode binary\n"
	     " remove -n NAME                                              "
			"Remove a component\n"
	     " create -s size -B bootblock [-A boot-address] \\\n"
	     "        [-H header-address] -m ARCH [-a align] [-o offset]   "
			"Create a ROM file\n"
	     " locate -f FILE -n NAME -a align                             "
			"Find a place for a file of that size\n"
	     " print	                                                   "
			"Show the contents of the ROM\n"
	     " extract -n NAME -f FILE                                     "
			"Extracts a raw payload from ROM\n"
	     "\n"
	     "ARCHes:\n"
	     "  armv7, x86\n"
	     "TYPEs:\n", name, name
	    );
	print_all_cbfs_entry_types();
}

int main(int argc, char **argv)
{
	size_t i;
	int c;

	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	param.cbfs_name = argv[1];
	char *cmd = argv[2];
	optind += 2;

	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(cmd, commands[i].name) != 0)
			continue;

		while (1) {
			char *suffix = NULL;
			int option_index = 0;

			c = getopt_long(argc, argv, commands[i].optstring,
						long_options, &option_index);
			if (c == -1)
				break;

			/* filter out illegal long options */
			if (strchr(commands[i].optstring, c) == NULL) {
				/* TODO maybe print actual long option instead */
				printf("%s: invalid option -- '%c'\n",
					argv[0], c);
				c = '?';
			}

			switch(c) {
			case 'n':
				param.rom_name = optarg;
				break;
			case 't':
				param.rom_type = get_cbfs_entry_type(optarg);
				if (param.rom_type == (uint32_t)-1)
					param.rom_type = strtoul(optarg, NULL,
								 0);
				if (param.rom_type == 0)
					printf("W: Unknown type '%s' ignored\n",
					       optarg);
				break;
			case 'c':
				if (!strncasecmp(optarg, "lzma", 5))
					param.rom_algo = CBFS_COMPRESS_LZMA;
				else if (!strncasecmp(optarg, "none", 5))
					param.rom_algo = CBFS_COMPRESS_NONE;
				else
					printf("W: Unknown compression '%s'"
					       " ignored.\n", optarg);
				break;
			case 'b':
				param.rom_baseaddress = strtoul(optarg, NULL, 0);
				break;
			case 'l':
				param.rom_loadaddress = strtoul(optarg, NULL, 0);

				break;
			case 'e':
				param.rom_entrypoint = strtoul(optarg, NULL, 0);
				break;
			case 's':
				param.rom_size = strtoul(optarg, &suffix, 0);
				if (tolower(suffix[0])=='k') {
					param.rom_size *= 1024;
				}
				if (tolower(suffix[0])=='m') {
					param.rom_size *= 1024 * 1024;
				}
			case 'B':
				param.rom_bootblock = optarg;
				break;
			case 'A':
				param.rom_bootaddress = strtoul(optarg, NULL, 0);
				break;
			case 'H':
				param.rom_headeraddress = strtoul(
						optarg, NULL, 0);
				break;
			case 'a':
				param.rom_alignment = strtoul(optarg, NULL, 0);
				break;
			case 'o':
				param.rom_offset = strtoul(optarg, NULL, 0);
				break;
			case 'f':
				param.rom_filename = optarg;
				break;
			case 'v':
				verbose++;
				break;
			case 'm':
				// arch = string_to_arch(optarg);
				break;
			case 'h':
			case '?':
				usage(argv[0]);
				return 1;
			default:
				break;
			}
		}

		return commands[i].function();
	}

	printf("Unknown command '%s'.\n", cmd);
	usage(argv[0]);
	return 1;
}
