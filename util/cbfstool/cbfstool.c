/*
 * cbfstool, CLI utility for CBFS file manipulation
 *
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 * Copyright (C) 2012 Google, Inc.
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
#include "cbfs.h"
#include "cbfs_image.h"

struct command {
	const char *name;
	const char *optstring;
	int (*function) (void);
};

int verbose = 0;
static struct param {
	char *cbfs_name;
	char *name;
	char *filename;
	char *bootblock;
	uint32_t type;
	uint32_t baseaddress;
	uint32_t baseaddress_assigned;
	uint32_t loadaddress;
	uint32_t headeroffset;
	uint32_t headeroffset_assigned;
	uint32_t entrypoint;
	uint32_t size;
	uint32_t alignment;
	uint32_t offset;
	uint32_t top_aligned;
	comp_algo algo;
} param = {
	/* All variables not listed are initialized as zero. */
	.algo = CBFS_COMPRESS_NONE,
};

static int cbfs_add(void)
{
	uint32_t filesize = 0;
	void *rom, *filedata, *cbfsfile;

	if (!param.filename) {
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (param.type == 0) {
		ERROR("You need to specify a valid -t/--type.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		ERROR("Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	filedata = loadfile(param.filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		ERROR("Could not load file '%s'.\n",
			param.filename);
		free(rom);
		return 1;
	}

	cbfsfile = create_cbfs_file(param.name, filedata, &filesize,
					param.type, &param.baseaddress);
	free(filedata);

	if (add_file_to_cbfs(cbfsfile, filesize, param.baseaddress)) {
		ERROR("Adding file '%s' failed.\n", param.filename);
		free(cbfsfile);
		free(rom);
		return 1;
	}
	if (writerom(param.cbfs_name, rom, romsize)) {
		free(cbfsfile);
		free(rom);
		return 1;
	}

	free(cbfsfile);
	free(rom);
	return 0;
}

static int cbfs_add_payload(void)
{
	uint32_t filesize = 0;
	void *rom, *filedata, *cbfsfile;
	unsigned char *payload;

	if (!param.filename) {
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		ERROR("Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	filedata = loadfile(param.filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		ERROR("Could not load file '%s'.\n",
			param.filename);
		free(rom);
		return 1;
	}

	filesize = parse_elf_to_payload(filedata, &payload, param.algo);
	if (filesize <= 0) {
		ERROR("Adding payload '%s' failed.\n",
			param.filename);
		free(rom);
		return 1;
	}

	cbfsfile = create_cbfs_file(param.name, payload, &filesize,
				CBFS_COMPONENT_PAYLOAD, &param.baseaddress);

	free(filedata);
	free(payload);

	if (add_file_to_cbfs(cbfsfile, filesize, param.baseaddress)) {
		ERROR("Adding payload '%s' failed.\n",
			param.filename);
		free(cbfsfile);
		free(rom);
		return 1;
	}

	if (writerom(param.cbfs_name, rom, romsize)) {
		free(cbfsfile);
		free(rom);
		return 1;
	}

	free(cbfsfile);
	free(rom);
	return 0;
}

static int cbfs_add_stage(void)
{
	uint32_t filesize = 0;
	void *rom, *filedata, *cbfsfile;
	unsigned char *stage;

	if (!param.filename) {
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		ERROR("Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	filedata = loadfile(param.filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		ERROR("Could not load file '%s'.\n",
			param.filename);
		free(rom);
		return 1;
	}

	filesize = parse_elf_to_stage(filedata, &stage, param.algo, &param.baseaddress);

	cbfsfile = create_cbfs_file(param.name, stage, &filesize,
				CBFS_COMPONENT_STAGE, &param.baseaddress);

	free(filedata);
	free(stage);

	if (add_file_to_cbfs(cbfsfile, filesize, param.baseaddress)) {
		ERROR("Adding stage '%s' failed.\n",
			param.filename);
		free(cbfsfile);
		free(rom);
		return 1;
	}

	if (writerom(param.cbfs_name, rom, romsize)) {
		free(cbfsfile);
		free(rom);
		return 1;
	}

	free(cbfsfile);
	free(rom);
	return 0;
}

static int cbfs_add_flat_binary(void)
{
	uint32_t filesize = 0;
	void *rom, *filedata, *cbfsfile;
	unsigned char *payload;

	if (!param.filename) {
		ERROR("You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (param.loadaddress == 0) {
		ERROR("You need to specify a valid "
			"-l/--load-address.\n");
		return 1;
	}

	if (param.entrypoint == 0) {
		ERROR("You need to specify a valid "
			"-e/--entry-point.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		ERROR("Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	filedata = loadfile(param.filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		ERROR("Could not load file '%s'.\n",
			param.filename);
		free(rom);
		return 1;
	}

	filesize = parse_flat_binary_to_payload(filedata, &payload,
						filesize,
						param.loadaddress,
						param.entrypoint,
						param.algo);
	free(filedata);

	if ((int)filesize <= 0) {
		ERROR("Adding payload '%s' failed.\n",
			param.filename);
		free(rom);
		return 1;
	}
	cbfsfile = create_cbfs_file(param.name, payload, &filesize,
				    CBFS_COMPONENT_PAYLOAD, &param.baseaddress);

	free(payload);
	if (add_file_to_cbfs(cbfsfile, filesize, param.baseaddress)) {
		ERROR("Adding payload '%s' failed.\n",
			param.filename);
		free(cbfsfile);
		free(rom);
		return 1;
	}
	if (writerom(param.cbfs_name, rom, romsize)) {
		free(cbfsfile);
		free(rom);
		return 1;
	}

	free(cbfsfile);
	free(rom);
	return 0;
}

static int cbfs_remove(void)
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

	if (cbfs_remove_entry(&image, param.name) != 0) {
		ERROR("Removing file '%s' failed.\n",
		      param.name);
		cbfs_image_delete(&image);
		return 1;
	}
	if (cbfs_image_write_file(&image, param.cbfs_name) != 0) {
		cbfs_image_delete(&image);
		return 1;
	}

	cbfs_image_delete(&image);
	return result;
}

static int cbfs_create(void)
{
	struct cbfs_image image;
	struct buffer bootblock;

	if (param.size == 0) {
		ERROR("You need to specify a valid -s/--size.\n");
		return 1;
	}

	if (!param.bootblock) {
		ERROR("You need to specify -B/--bootblock.\n");
		return 1;
	}

	// TODO Remove arch or pack into param.
	if (arch == CBFS_ARCHITECTURE_UNKNOWN) {
		ERROR("You need to specify -m/--machine arch\n");
		return 1;
	}

	if (buffer_from_file(&bootblock, param.bootblock) != 0) {
		return 1;
	}

	// Setup default boot offset and header offset.
	if (!param.baseaddress_assigned) {
		// put boot block before end of ROM.
		param.baseaddress = param.size - bootblock.size;
		DEBUG("bootblock in end of ROM.\n");
	}
	if (!param.headeroffset_assigned) {
		// Put header before bootblock, and make a reference in end of
		// bootblock.
		param.headeroffset = (
				param.baseaddress -
				sizeof(struct cbfs_header));
		if (bootblock.size >= sizeof(uint32_t)) {
			// TODO this only works for 32b top-aligned system now...
			uint32_t ptr = param.headeroffset - param.size;
			uint32_t *sig = (uint32_t *)(bootblock.data +
						     bootblock.size -
						     sizeof(ptr));
			*sig = ptr;
			DEBUG("CBFS header reference in end of bootblock.\n");
		}
	}

	if (cbfs_image_create(&image,
			      arch,
			      param.size,
			      param.alignment,
			      &bootblock,
			      param.baseaddress,
			      param.headeroffset,
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

static int cbfs_locate(void)
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

	address = cbfs_locate_entry(&image, param.name, buffer.size,
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

static int cbfs_print(void)
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

static int cbfs_extract(void)
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
	{"add", "f:n:t:b:vh?", cbfs_add},
	{"add-payload", "f:n:t:c:b:vh?", cbfs_add_payload},
	{"add-stage", "f:n:t:c:b:vh?", cbfs_add_stage},
	{"add-flat-binary", "f:n:l:e:c:b:vh?", cbfs_add_flat_binary},
	{"remove", "n:vh?", cbfs_remove},
	{"create", "s:B:b:H:a:o:m:vh?", cbfs_create},
	{"locate", "f:n:a:Tvh?", cbfs_locate},
	{"print", "vh?", cbfs_print},
	{"extract", "n:f:vh?", cbfs_extract},
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
	     "  -T              Output top-aligned memory address\n"
	     "  -v              Provide verbose output\n"
	     "  -h              Display this help message\n\n"
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
	     " create -s size -B bootblock -m ARCH [-a align] [-o offset]  "
			"Create a ROM file\n"
	     " locate -f FILE -n NAME [-a align] [-T]                      "
			"Find a place for a file of that size\n"
	     " print                                                       "
			"Show the contents of the ROM\n"
	     " extract -n NAME -f FILE                                     "
			"Extracts a raw payload from ROM\n"
	     "\n"
	     "ARCHes:\n"
	     "  armv7, x86\n"
	     "TYPEs:\n", name, name
	    );
	print_supported_filetypes();
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
				ERROR("%s: invalid option -- '%c'\n",
				      argv[0], c);
				c = '?';
			}

			switch(c) {
			case 'n':
				param.name = optarg;
				break;
			case 't':
				if (intfiletype(optarg) != ((uint64_t) - 1))
					param.type = intfiletype(optarg);
				else
					param.type = strtoul(optarg, NULL, 0);
				if (param.type == 0)
					WARN("Unknown type '%s' ignored\n",
							optarg);
				break;
			case 'c':
				if (!strncasecmp(optarg, "lzma", 5))
					param.algo = CBFS_COMPRESS_LZMA;
				else if (!strncasecmp(optarg, "none", 5))
					param.algo = CBFS_COMPRESS_NONE;
				else
					WARN("Unknown compression '%s'"
					     " ignored.\n", optarg);
				break;
			case 'b':
				param.baseaddress = strtoul(optarg, NULL, 0);
				// baseaddress may be zero on non-x86, so we
				// need an explicit "baseaddress_assigned".
				param.baseaddress = strtoul(optarg, NULL, 0);
				param.baseaddress_assigned = 1;
				break;
			case 'l':
				param.loadaddress = strtoul(optarg, NULL, 0);

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
				param.bootblock = optarg;
				break;
			case 'H':
				param.headeroffset = strtoul(
						optarg, NULL, 0);
				param.headeroffset_assigned = 1;
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
				arch = string_to_arch(optarg);
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
