/*
 * cbfstool, CLI utility for CBFS file manipulation
 *
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
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
#include <string.h>
#include "common.h"
#include "cbfs.h"

typedef enum {
	CMD_ADD,
	CMD_ADD_PAYLOAD,
	CMD_ADD_STAGE,
	CMD_CREATE,
	CMD_LOCATE,
	CMD_PRINT,
	CMD_EXTRACT,
} cmd_t;

struct command {
	cmd_t id;
	const char *name;
	int (*function) (int argc, char **argv);
};

static int cbfs_add(int argc, char **argv)
{
	char *romname = argv[1];
	char *cmd = argv[2];
	void *rom = loadrom(romname);

	if (rom == NULL) {
		printf("Could not load ROM image '%s'.\n", romname);
		return 1;
	}

	if (argc < 5) {
		printf("not enough arguments to '%s'.\n", cmd);
		return 1;
	}

	char *filename = argv[3];
	char *cbfsname = argv[4];

	uint32_t filesize = 0;
	void *filedata = loadfile(filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		printf("Could not load file '%s'.\n", filename);
		return 1;
	}

	uint32_t base = 0;
	void *cbfsfile = NULL;

	if (argc < 6) {
		printf("not enough arguments to 'add'.\n");
		return 1;
	}
	uint32_t type;
	if (intfiletype(argv[5]) != ((uint64_t) - 1))
		type = intfiletype(argv[5]);
	else
		type = strtoul(argv[5], NULL, 0);
	if (argc > 6) {
		base = strtoul(argv[6], NULL, 0);
	}
	cbfsfile = create_cbfs_file(cbfsname, filedata, &filesize, type, &base);
	if (add_file_to_cbfs(cbfsfile, filesize, base))
		return 1;
	if (writerom(romname, rom, romsize))
		return 1;
	return 0;
}

static int cbfs_add_payload(int argc, char **argv)
{
	char *romname = argv[1];
	char *cmd = argv[2];
	void *rom = loadrom(romname);

	if (rom == NULL) {
		printf("Could not load ROM image '%s'.\n", romname);
		return 1;
	}

	if (argc < 5) {
		printf("not enough arguments to '%s'.\n", cmd);
		return 1;
	}

	char *filename = argv[3];
	char *cbfsname = argv[4];

	uint32_t filesize = 0;
	void *filedata = loadfile(filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		printf("Could not load file '%s'.\n", filename);
		return 1;
	}

	uint32_t base = 0;
	void *cbfsfile = NULL;

	comp_algo algo = CBFS_COMPRESS_NONE;
	if (argc > 5) {
		if (argv[5][0] == 'l')
			algo = CBFS_COMPRESS_LZMA;
	}
	if (argc > 6) {
		base = strtoul(argv[6], NULL, 0);
	}
	unsigned char *payload;
	filesize = parse_elf_to_payload(filedata, &payload, algo);
	cbfsfile =
	    create_cbfs_file(cbfsname, payload, &filesize,
			     CBFS_COMPONENT_PAYLOAD, &base);
	if (add_file_to_cbfs(cbfsfile, filesize, base))
		return 1;
	if (writerom(romname, rom, romsize))
		return 1;
	return 0;
}

static int cbfs_add_stage(int argc, char **argv)
{
	char *romname = argv[1];
	char *cmd = argv[2];
	void *rom = loadrom(romname);

	if (rom == NULL) {
		printf("Could not load ROM image '%s'.\n", romname);
		return 1;
	}

	if (argc < 5) {
		printf("not enough arguments to '%s'.\n", cmd);
		return 1;
	}

	char *filename = argv[3];
	char *cbfsname = argv[4];

	uint32_t filesize = 0;
	void *filedata = loadfile(filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		printf("Could not load file '%s'.\n", filename);
		return 1;
	}

	uint32_t base = 0;
	void *cbfsfile = NULL;

	comp_algo algo = CBFS_COMPRESS_NONE;
	if (argc > 5) {
		if (argv[5][0] == 'l')
			algo = CBFS_COMPRESS_LZMA;
	}
	if (argc > 6) {
		base = strtoul(argv[6], NULL, 0);
	}
	unsigned char *stage;
	filesize = parse_elf_to_stage(filedata, &stage, algo, &base);
	cbfsfile =
	    create_cbfs_file(cbfsname, stage, &filesize,
			     CBFS_COMPONENT_STAGE, &base);

	if (add_file_to_cbfs(cbfsfile, filesize, base))
		return 1;
	if (writerom(romname, rom, romsize))
		return 1;
	return 0;
}

static int cbfs_create(int argc, char **argv)
{
	char *romname = argv[1];
	char *cmd = argv[2];
	if (argc < 5) {
		printf("not enough arguments to 'create'.\n");
		return 1;
	}

	char* suffix;
	uint32_t size = strtoul(argv[3], &suffix, 0);
	if (tolower(suffix[0])=='k') {
		size *= 1024;
	}
	if (tolower(suffix[0])=='m') {
		size *= 1024 * 1024;
	}
	char *bootblock = argv[4];
	uint32_t align = 0;

	if (argc > 5)
		align = strtoul(argv[5], NULL, 0);

	return create_cbfs_image(romname, size, bootblock, align);
}

static int cbfs_locate(int argc, char **argv)
{
	char *romname = argv[1];
	if (argc < 6) {
		printf("not enough arguments to 'locate'.\n");
		return 1;
	}

	const char *file = argv[3];
	uint32_t filesize = getfilesize(file);
	const char *filename = argv[4];
	int align = strtoul(argv[5], NULL, 0);

	printf("%x\n", cbfs_find_location(romname, filesize, filename, align));
	return 0;
}

static int cbfs_print(int argc, char **argv)
{
	char *romname = argv[1];
	char *cmd = argv[2];
	void *rom = loadrom(romname);

	if (rom == NULL) {
		printf("Could not load ROM image '%s'.\n", romname);
		return 1;
	}

	print_cbfs_directory(romname);
	return 0;
}

static int cbfs_extract(int argc, char **argv)
{
	char *romname = argv[1];
	char *cmd = argv[2];
	void *rom = loadrom(romname);

	if (rom == NULL) {
		printf("Could not load ROM image '%s'.\n", romname);
		return 1;
	}

	if (argc != 5)
	{
		printf("Error: you must specify a CBFS name and a file to dump it in.\n");
		return 1;
	}

	return extract_file_from_cbfs(romname, argv[3], argv[4]);
}

struct command commands[] = {
	{CMD_ADD, "add", cbfs_add},
	{CMD_ADD_PAYLOAD, "add-payload", cbfs_add_payload},
	{CMD_ADD_STAGE, "add-stage", cbfs_add_stage},
	{CMD_CREATE, "create", cbfs_create},
	{CMD_LOCATE, "locate", cbfs_locate},
	{CMD_PRINT, "print", cbfs_print},
	{CMD_EXTRACT, "extract", cbfs_extract},
};

void usage(void)
{
	printf
	    ("cbfstool: Management utility for CBFS formatted ROM images\n\n"
	     "USAGE:\n" " cbfstool [-h]\n"
	     " cbfstool FILE COMMAND [PARAMETERS]...\n\n" "OPTIONs:\n"
	     "  -h		Display this help message\n\n"
	     "COMMANDs:\n"
	     " add FILE NAME TYPE [base address]    Add a component\n"
	     " add-payload FILE NAME [COMP] [base]  Add a payload to the ROM\n"
	     " add-stage FILE NAME [COMP] [base]    Add a stage to the ROM\n"
	     " create SIZE BOOTBLOCK [ALIGN]        Create a ROM file\n"
	     " locate FILE NAME ALIGN               Find a place for a file of that size\n"
	     " print                                Show the contents of the ROM\n"
	     " extract NAME FILE                    Extracts a raw payload from ROM\n"
	     "\n"
	     "TYPEs:\n"
	     );
	print_supported_filetypes();
}

int main(int argc, char **argv)
{
	int i;

	if (argc < 3) {
		usage();
		return 1;
	}

	char *cmd = argv[2];

	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(cmd, commands[i].name) != 0)
			continue;
		return commands[i].function(argc, argv);
	}

	printf("Unknown command '%s'.\n", cmd);
	usage();
	return 1;
}
