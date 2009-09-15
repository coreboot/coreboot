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
#include <stdint.h>
#include "common.h"
#include "cbfs.h"

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf
		    ("cbfstool: Management utility for CBFS formatted ROM images\n"
		     "USAGE:\n" "cbfstool [-h]\n"
		     "cbfstool FILE COMMAND [PARAMETERS]...\n\n" "OPTIONs:\n"
		     " -h		Display this help message\n\n"
		     "COMMANDs:\n"
		     "add FILE NAME TYPE [base address]    Add a component\n"
		     "add-payload FILE NAME [COMP] [base]  Add a payload to the ROM\n"
		     "add-stage FILE NAME [COMP] [base]    Add a stage to the ROM\n"
		     "create SIZE BSIZE BOOTBLOCK [ALIGN]  Create a ROM file\n"
		     "print                                Show the contents of the ROM\n");
		return 1;
	}
	char *romname = argv[1];
	char *cmd = argv[2];

	if (strcmp(cmd, "create") == 0) {
		if (argc < 6) {
			printf("not enough arguments to 'create'.\n");
			return 1;
		}
		uint32_t size = strtoul(argv[3], NULL, 0);
		/* ignore bootblock size. we use whatever we get and won't allocate any larger */
		char *bootblock = argv[5];
		uint32_t align = 0;
		if (argc > 6)
			align = strtoul(argv[6], NULL, 0);
		return create_cbfs_image(romname, size, bootblock, align);
	}

	void *rom = loadrom(romname);
	if (rom == NULL) {
		printf("Could not load ROM image '%s'.\n", romname);
		return 1;
	}

	if (strcmp(cmd, "print") == 0) {
		print_cbfs_directory(romname);
		return 0;
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
	void *cbfsfile;

	if (strcmp(cmd, "add") == 0) {
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
		cbfsfile =
		    create_cbfs_file(cbfsname, filedata, &filesize, type,
				     &base);
	}

	if (strcmp(cmd, "add-payload") == 0) {
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
	}

	if (strcmp(cmd, "add-stage") == 0) {
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
	}

	add_file_to_cbfs(cbfsfile, filesize, base);
	writerom(romname, rom, romsize);
	return 0;
}
