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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include "common.h"

int verbose = 0;

struct command {
	const char *name;
	const char *optstring;
	int (*function) (void);
};

struct param {
	char *cbfs_name;
	char *name;
	char *filename;
	char *bootblock_file;
	uint32_t type;
	uint32_t base_address;
	uint32_t load_address;
	uint32_t bootblock_offset;
	uint32_t bootblock_offset_assigned;
	uint32_t header_offset;
	uint32_t header_offset_assigned;
	uint32_t entrypoint;
	uint32_t size;
	uint32_t alignment;
	uint32_t offset;
	uint32_t top_aligned;
	comp_algo algo;
} param;

typedef int (*convert_buffer_t)(struct buffer *buffer);

static int cbfs_add_file(const char *filename,
			 const char *name,
			 uint32_t type,
			 convert_buffer_t convert,
			 const char *cbfs_name) {
	struct cbfs_image image;
	struct buffer buffer;
	int result = 1;

	if (!filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (type == 0) {
		fprintf(stderr, "E: You need to specify a valid -t/--type.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, cbfs_name) != 0) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	if (cbfs_get_entry(&image, name)) {
		fprintf(stderr, "E: '%s' already in ROM image.\n", name);
		return 1;
	}

	if (buffer_from_file(&buffer, filename) != 0) {
		fprintf(stderr, "E: Could not load file '%s'.\n", filename);
		cbfs_image_delete(&image);
		return 1;
	}

	do {
		if (convert && convert(&buffer) != 0) {
			fprintf(stderr, "E: Failed to parse file '%s'.\n",
				filename);
			break;
		}
		if (cbfs_add_entry(&image, name, type, &buffer) != 0)
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
	int result = parse_elf_to_stage(buffer->data, &output, param.algo,
					param.base_address);
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
					  param.algo);
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
					      param.load_address,
					      param.entrypoint,
					      param.algo);
	if (result < 0)
		return -1;
	name = strdup(buffer->name);
	buffer_delete(buffer);
	buffer->name = name;
	buffer->size = (size_t)result;
	buffer->data = (char*)output;
	return 0;
}

static int cbfstool_add(void)
{
	// TODO where is rom_base_address?
	return cbfs_add_file(param.filename,
			     param.name,
			     param.type,
			     NULL,
			     param.cbfs_name);
}


static int cbfstool_add_payload(void)
{
	return cbfs_add_file(param.filename,
			     param.name,
			     CBFS_COMPONENT_PAYLOAD,
			     cbfs_convert_elf_mkpayload,
			     param.cbfs_name);
}

static int cbfstool_add_stage(void)
{
	return cbfs_add_file(param.filename,
			     param.name,
			     CBFS_COMPONENT_STAGE,
			     cbfs_convert_mkstage,
			     param.cbfs_name);
}

static int cbfstool_add_flat_binary(void)
{
	if (param.load_address == 0) {
		fprintf(stderr, "E: You need to specify a valid "
			"-l/--load-address.\n");
		return 1;
	}
	if (param.entrypoint == 0) {
		fprintf(stderr, "E: You need to specify a valid "
			"-e/--entry-point.\n");
		return 1;
	}
	return cbfs_add_file(param.filename,
			     param.name,
			     CBFS_COMPONENT_PAYLOAD,
			     cbfs_convert_flat_mkpayload,
			     param.cbfs_name);
}

static int cbfstool_remove(void)
{
	int result = 1;
	struct cbfs_image image;

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	do {
		if (cbfs_remove_entry(&image, param.name) != 0) {
			fprintf(stderr, "E: Removing file '%s' failed.\n",
				param.name);
			break;
		}
		if (cbfs_image_write_file(&image, param.cbfs_name) != 0)
			break;
		result = 0;
	} while (0);

	cbfs_image_delete(&image);
	return result;
}

static int cbfstool_create(void)
{
	struct cbfs_image image;
	struct buffer bootblock;

	if (param.size == 0) {
		fprintf(stderr, "E: You need to specify a valid -s/--size.\n");
		return 1;
	}

	if (!param.bootblock_file) {
		fprintf(stderr, "E: You need to specify -b/--bootblock.\n");
		return 1;
	}

	if (buffer_from_file(&bootblock, param.bootblock_file) != 0) {
		return 1;
	}

	// Setup default boot offset and header offset.
	if (!param.bootblock_offset_assigned) {
		// put boot block before end of ROM.
		param.bootblock_offset = param.size - bootblock.size;
		DEBUG("bootblock in end of ROM.\n");
	}
	if (!param.header_offset_assigned) {
		// Put header before bootblock, and make a reference in end of
		// bootblock.
		param.header_offset = (
				param.bootblock_offset -
				sizeof(struct cbfs_header));
		if (bootblock.size >= sizeof(uint32_t)) {
			// TODO this only works for 32b top-aligned system now...
			uint32_t ptr = param.header_offset - param.size;
			uint32_t *sig = (uint32_t*)(bootblock.data +
						    bootblock.size -
						    sizeof(ptr));
			*sig = ptr;
			DEBUG("CBFS header reference in end of bootblock.\n");
		}
	}

	if (cbfs_image_create(&image,
			      param.size,
			      param.alignment,
			      &bootblock,
			      param.bootblock_offset,
			      param.header_offset,
			      param.offset) != 0) {
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

static int cbfstool_locate_stage(void)
{
	struct cbfs_image image;
	struct cbfs_file *entry;
	struct buffer buffer;
	size_t stage_size;
	uint32_t address;

	if (!param.filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		fprintf(stderr, "E: Failed to load %s.\n", param.cbfs_name);
		return 1;
	}

	if (buffer_from_file(&buffer, param.filename) != 0) {
		fprintf(stderr, "E: Cannot load %s.\n", param.filename);
		cbfs_image_delete(&image);
		return 1;
	}

	// Increase buffer for cbfs_stage header.
	stage_size = buffer.size + sizeof(struct cbfs_stage);
	buffer_delete(&buffer);
	buffer_create(&buffer, stage_size, param.name);

	if (cbfs_get_entry(&image, param.name)) {
		fprintf(stderr, "E: '%s' already in CBFS.\n", param.name);
		buffer_delete(&buffer);
		cbfs_image_delete(&image);
		return 1;
	}

	if (cbfs_add_entry(&image, param.name, CBFS_COMPONENT_STAGE,
			   &buffer) != 0) {
		buffer_delete(&buffer);
		cbfs_image_delete(&image);
		return 1;
	}
	entry = cbfs_get_entry(&image, param.name);
	assert(entry);
	assert(cbfs_is_valid_entry(entry));
	address = (cbfs_get_entry_addr(&image, entry) + ntohl(entry->offset) +
		   sizeof(struct cbfs_stage));

	if (param.top_aligned) {
		address = address - ntohl(image.header->romsize);
	}
	buffer_delete(&buffer);
	cbfs_image_delete(&image);
	printf("0x%x\n", address);
	return 0;
}

static int cbfstool_print(void)
{
	struct cbfs_image image;
	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}
	cbfs_print_directory(&image);
	cbfs_image_delete(&image);
	return 0;
}

static int cbfstool_extract(void)
{
	int result = 0;
	struct cbfs_image image;

	if (!param.filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		result = 1;
	} else if (cbfs_export_entry(&image, param.name,
				     param.filename) != 0) {
		result = 1;
	}

	cbfs_image_delete(&image);
	return result;
}

static const struct command commands[] = {
	{"add", "vf:n:t:b:h?", cbfstool_add},
	{"add-payload", "vf:n:t:c:b:h?", cbfstool_add_payload},
	{"add-stage", "vf:n:t:c:b:h?", cbfstool_add_stage},
	{"add-flat-binary", "vf:n:l:e:c:b:h?", cbfstool_add_flat_binary},
	{"remove", "vn:h?", cbfstool_remove},
	{"create", "vs:B:A:H:a:o:m:h?", cbfstool_create},
	{"locate-stage", "vTf:n:h?", cbfstool_locate_stage},
	{"print", "vh?", cbfstool_print},
	{"extract", "vn:f:h?", cbfstool_extract},
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
	{"bootblock-offset", required_argument, 0, 'A' },
	{"header-offset",required_argument,0, 'H' },
	{"alignment",    required_argument, 0, 'a' },
	{"offset",       required_argument, 0, 'o' },
	{"file",         required_argument, 0, 'f' },
	{"arch",         required_argument, 0, 'm' },
	{"top-aligned",  no_argument,       0, 'T' },
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
	     "  -T             Display top-aligned address instead of offset\n"
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
	     " create -s size -B bootblock [-A bootblock-offset] \\\n"
	     "        [-H header-offset] [-a align] [-o offset]            "
			"Create a ROM file\n"
	     " locate-stage -f FILE -n NAME [-T]                           "
			"Find the entry if the stage component is added\n"
	     " print                                                       "
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
				param.name = optarg;
				break;
			case 't':
				param.type = get_cbfs_entry_type(optarg);
				if (param.type == (uint32_t)-1)
					param.type = strtoul(optarg, NULL,
								 0);
				if (param.type == 0)
					printf("W: Unknown type '%s' ignored\n",
					       optarg);
				break;
			case 'c':
				if (!strncasecmp(optarg, "lzma", 5))
					param.algo = CBFS_COMPRESS_LZMA;
				else if (!strncasecmp(optarg, "none", 5))
					param.algo = CBFS_COMPRESS_NONE;
				else
					printf("W: Unknown compression '%s'"
					       " ignored.\n", optarg);
				break;
			case 'b':
				param.base_address = strtoul(optarg, NULL, 0);
				break;
			case 'l':
				param.load_address = strtoul(optarg, NULL, 0);

				break;
			case 'e':
				param.entrypoint = strtoul(optarg, NULL, 0);
				break;
			case 's':
				param.size = strtoul(optarg, &suffix, 0);
				if (tolower(suffix[0])=='k') {
					param.size *= 1024;
				}
				if (tolower(suffix[0])=='m') {
					param.size *= 1024 * 1024;
				}
			case 'B':
				param.bootblock_file = optarg;
				break;
			case 'A':
				param.bootblock_offset = strtoul(
						optarg, NULL, 0);
				param.bootblock_offset_assigned = 1;
				break;
			case 'H':
				param.header_offset = strtoul(
						optarg, NULL, 0);
				param.header_offset_assigned = 1;
				break;
			case 'a':
				param.alignment = strtoul(optarg, NULL, 0);
				break;
			case 'o':
				param.offset = strtoul(optarg, NULL, 0);
				break;
			case 'f':
				param.filename = optarg;
				break;
			case 'T':
				param.top_aligned = 1;
				break;
			case 'v':
				verbose++;
				break;
			case 'm':
				INFO("Architecture (-m) is deprecated.\n");
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
