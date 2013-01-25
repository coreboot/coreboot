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

#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	uint32_t base_address_assigned;
	uint32_t load_address;
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

static int cbfstool_add_file(const char *cbfs_name,
			     const char *filename,
			     const char *name,
			     uint32_t type,
			     uint32_t offset,
			     convert_buffer_t convert) {
	struct cbfs_image image;
	struct buffer buffer;
	int result = 1;

	if (!filename) {
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (type == 0) {
		ERROR("You need to specify a valid -t/--type.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, cbfs_name) != 0) {
		ERROR("Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	if (cbfs_get_entry(&image, name)) {
		ERROR("'%s' already in ROM image.\n", name);
		return 1;
	}

	if (buffer_from_file(&buffer, filename) != 0) {
		ERROR("Could not load file '%s'.\n", filename);
		cbfs_image_delete(&image);
		return 1;
	}

	do {
		if (convert && convert(&buffer) != 0) {
			ERROR("Failed to parse file '%s'.\n",
				filename);
			break;
		}
		if (cbfs_add_entry(&image, &buffer, name, type, offset) != 0)
			break;
		if (cbfs_image_write_file(&image, cbfs_name) != 0)
			break;

		result = 0;
	} while (0);

	buffer_delete(&buffer);
	cbfs_image_delete(&image);
	return result;
}

static int cbfstool_convert_mkstage(struct buffer *buffer) {
	struct buffer output;
	if (parse_elf_to_stage(buffer, &output, param.algo,
			       param.base_address) != 0) 
		return -1;
	if (param.base_address)
		param.base_address -= sizeof(struct cbfs_stage);
	buffer_delete(buffer);
	// direct assign, no dupe.
	memcpy(buffer, &output, sizeof(*buffer));
	return 0;
}

static int cbfstool_convert_elf_mkpayload(struct buffer *buffer) {
	struct buffer output;
	if (parse_elf_to_payload(buffer, &output, param.algo) != 0)
		return -1;
	buffer_delete(buffer);
	// direct assign, no dupe.
	memcpy(buffer, &output, sizeof(*buffer));
	return 0;
}

static int cbfstool_convert_flat_mkpayload(struct buffer *buffer) {
	struct buffer output;
	if (parse_flat_binary_to_payload(buffer, &output,
					 param.load_address,
					 param.entrypoint,
					 param.algo) != 0) {
		return -1;
	}
	buffer_delete(buffer);
	// direct assign, no dupe.
	memcpy(buffer, &output, sizeof(*buffer));
	return 0;
}

static int cbfstool_add(void)
{
	return cbfstool_add_file(param.cbfs_name,
				 param.filename,
				 param.name,
				 param.type,
				 param.base_address,
				 NULL);
}

static int cbfstool_add_payload(void)
{
	return cbfstool_add_file(param.cbfs_name,
				 param.filename,
				 param.name,
				 CBFS_COMPONENT_PAYLOAD,
				 param.base_address,
				 cbfstool_convert_elf_mkpayload);
}

static int cbfstool_add_stage(void)
{
	return cbfstool_add_file(param.cbfs_name,
				 param.filename,
				 param.name,
				 CBFS_COMPONENT_STAGE,
				 param.base_address,
				 cbfstool_convert_mkstage);
}

static int cbfstool_add_flat_binary(void)
{
	if (param.load_address == 0) {
		ERROR("You need to specify a valid "
			"-l/--load-address.\n");
		return 1;
	}
	if (param.entrypoint == 0) {
		ERROR("You need to specify a valid "
			"-e/--entry-point.\n");
		return 1;
	}
	return cbfstool_add_file(param.cbfs_name,
				 param.filename,
				 param.name,
				 CBFS_COMPONENT_PAYLOAD,
				 param.base_address,
				 cbfstool_convert_flat_mkpayload);
}

static int cbfstool_remove(void)
{
	int result = 1;
	struct cbfs_image image;

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		ERROR("Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	do {
		if (cbfs_remove_entry(&image, param.name) != 0) {
			ERROR("Removing file '%s' failed.\n",
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
		ERROR("You need to specify a valid -s/--size.\n");
		return 1;
	}

	if (!param.bootblock_file) {
		ERROR("You need to specify -B/--bootblock.\n");
		return 1;
	}

	if (buffer_from_file(&bootblock, param.bootblock_file) != 0) {
		return 1;
	}

	// Setup default boot offset and header offset.
	if (!param.base_address_assigned) {
		// put boot block before end of ROM.
		param.base_address = param.size - bootblock.size;
		DEBUG("bootblock in end of ROM.\n");
	}
	if (!param.header_offset_assigned) {
		// Put header before bootblock, and make a reference in end of
		// bootblock.
		param.header_offset = (
				param.base_address -
				sizeof(struct cbfs_header));
		if (bootblock.size >= sizeof(uint32_t)) {
			// TODO this only works for 32b top-aligned system now...
			uint32_t ptr = param.header_offset - param.size;
			uint32_t *sig = (uint32_t *)(bootblock.data +
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
			      param.base_address,
			      param.header_offset,
			      param.offset) != 0) {
		ERROR("Failed to create %s.\n", param.cbfs_name);
		return 1;
	}
	if (cbfs_image_write_file(&image, param.cbfs_name) != 0) {
		ERROR("Failed to write %s.\n", param.cbfs_name);
		return 1;
	}
	cbfs_image_delete(&image);
	return 0;

}

static int cbfstool_locate_stage(void)
{
	struct cbfs_image image;
	struct buffer buffer;
	int32_t address;

	if (!param.filename) {
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		ERROR("Failed to load %s.\n", param.cbfs_name);
		return 1;
	}

	if (buffer_from_file(&buffer, param.filename) != 0) {
		ERROR("Cannot load %s.\n", param.filename);
		cbfs_image_delete(&image);
		return 1;
	}

	if (cbfs_get_entry(&image, param.name)) {
		ERROR("'%s' already in CBFS.\n", param.name);
		buffer_delete(&buffer);
		cbfs_image_delete(&image);
		return 1;
	}

	address = cbfs_locate_stage(&image, param.name, buffer.size,
				    param.alignment);
	if (address == -1) {
		ERROR("'%s' can't fit in CBFS for align 0x%x.\n",
			param.name, param.alignment);
		buffer_delete(&buffer);
		cbfs_image_delete(&image);
		return 1;
	}

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
		ERROR("Could not load ROM image '%s'.\n",
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
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name) != 0) {
		ERROR("Could not load ROM image '%s'.\n",
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
	{"add", "f:n:t:b:vh?", cbfstool_add},
	{"add-payload", "f:n:t:c:b:vh?", cbfstool_add_payload},
	{"add-stage", "f:n:t:c:b:vh?", cbfstool_add_stage},
	{"add-flat-binary", "f:n:l:e:c:b:vh?", cbfstool_add_flat_binary},
	{"remove", "n:vh?", cbfstool_remove},
	{"create", "s:B:b:H:a:o:m:vh?", cbfstool_create},
	{"locate-stage", "Tf:n:a:vh?", cbfstool_locate_stage},
	{"print", "vh?", cbfstool_print},
	{"extract", "n:f:vh?", cbfstool_extract},
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
	{"header-offset",required_argument, 0, 'H' },
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
	LOG("cbfstool: Management utility for CBFS formatted ROM images\n\n"
	     "USAGE:\n" " %s [-h]\n"
	     " %s FILE COMMAND [-v] [PARAMETERS]...\n\n" "OPTIONs:\n"
	     "  -h             Display this help message\n\n"
	     "  -T             Display top-aligned address instead of offset\n"
	     "  -v             Verbose output\n"
	     "COMMANDs:\n"
	     " add -f FILE -n NAME -t TYPE [-b base]                       "
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
	     " create -s size -B bootblock [-b base] [-a align] \\\n"
	     "         [-o offset] [-H header-offset]                      "
			"Create a ROM file\n"
	     " locate-stage -f FILE -n NAME [-a align] [-T]                "
			"Find a space for stage to fit in one aligned page\n"
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

	/* Help tracing in debug builds. */
	if (verbose > 1) {
		LOG("[CBFS] ");
		for (i = 0; i < argc; i++)
			LOG("'%s' ", argv[i]);
		LOG("\n");
	}

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
				ERROR("%s: invalid option -- '%c'\n", argv[0],
				      c);
				c = '?';
			}

			switch(c) {
			case 'n':
				param.name = optarg;
				break;
			case 't':
				// zero should be reserved for types.
				param.type = get_cbfs_entry_type(
						optarg,
						strtoul(optarg, NULL, 0));
				if (param.type == 0)
					WARN("Unknown type '%s' ignored\n",
					     optarg);
				break;
			case 'c':
				param.algo = get_cbfs_compression(
						optarg, -1);
				if (param.algo == -1) {
					WARN("Unknown compression '%s'"
					     " ignored.\n", optarg);
					param.algo = 0;
				}
				break;
			case 'b':
				// base_address may be zero on non-x86, so we
				// need an explicit "base_address_assigned".
				param.base_address = strtoul(optarg, NULL, 0);
				param.base_address_assigned = 1;
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

	ERROR("Unknown command '%s'.\n", cmd);
	usage(argv[0]);
	return 1;
}
