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

#ifdef __MINGW32__
#define index strchr
#endif

struct command {
	const char *name;
	const char *optstring;
	int (*function) (void);
};

int verbose = 0;
static char *cbfs_name = NULL;
static char *rom_name = NULL;
static char *rom_filename = NULL;
static char *rom_bootblock = NULL;
static uint32_t rom_type = 0;
static uint32_t rom_baseaddress = 0;
static uint32_t rom_loadaddress = 0;
static uint32_t rom_entrypoint = 0;
static uint32_t rom_size = 0;
static uint32_t rom_alignment = 0;
static uint32_t rom_offset = 0;
static comp_algo rom_algo = CBFS_COMPRESS_NONE;

static int cbfs_add(void)
{
	uint32_t filesize = 0;
	void *rom, *filedata, *cbfsfile;

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

	rom = loadrom(cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	filedata = loadfile(rom_filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		fprintf(stderr, "E: Could not load file '%s'.\n",
			rom_filename);
		free(rom);
		return 1;
	}

	cbfsfile = create_cbfs_file(rom_name, filedata, &filesize,
					rom_type, &rom_baseaddress);
	free(filedata);

	if (add_file_to_cbfs(cbfsfile, filesize, rom_baseaddress)) {
		fprintf(stderr, "E: Adding file '%s' failed.\n", rom_filename);
		free(cbfsfile);
		free(rom);
		return 1;
	}
	if (writerom(cbfs_name, rom, romsize)) {
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
	int32_t filesize = 0;
	void *rom, *filedata, *cbfsfile;
	unsigned char *payload;

	if (!rom_filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	filedata = loadfile(rom_filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		fprintf(stderr, "E: Could not load file '%s'.\n",
			rom_filename);
		free(rom);
		return 1;
	}

	filesize = parse_elf_to_payload(filedata, &payload, rom_algo);
	if (filesize <= 0) {
		fprintf(stderr, "E: Adding payload '%s' failed.\n",
			rom_filename);
		free(rom);
		return 1;
	}

	cbfsfile = create_cbfs_file(rom_name, payload, &filesize,
				CBFS_COMPONENT_PAYLOAD, &rom_baseaddress);

	free(filedata);
	free(payload);

	if (add_file_to_cbfs(cbfsfile, filesize, rom_baseaddress)) {
		fprintf(stderr, "E: Adding payload '%s' failed.\n",
			rom_filename);
		free(cbfsfile);
		free(rom);
		return 1;
	}

	if (writerom(cbfs_name, rom, romsize)) {
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

	if (!rom_filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	filedata = loadfile(rom_filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		fprintf(stderr, "E: Could not load file '%s'.\n",
			rom_filename);
		free(rom);
		return 1;
	}

	filesize = parse_elf_to_stage(filedata, &stage, rom_algo, &rom_baseaddress);

	cbfsfile = create_cbfs_file(rom_name, stage, &filesize,
				CBFS_COMPONENT_STAGE, &rom_baseaddress);

	free(filedata);
	free(stage);

	if (add_file_to_cbfs(cbfsfile, filesize, rom_baseaddress)) {
		fprintf(stderr, "E: Adding stage '%s' failed.\n",
			rom_filename);
		free(cbfsfile);
		free(rom);
		return 1;
	}

	if (writerom(cbfs_name, rom, romsize)) {
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
	uint32_t final_size;
	void *rom, *filedata, *cbfsfile;
	unsigned char *payload;
	comp_func_ptr compress;
	struct cbfs_payload_segment *segs;
	int doffset, len = 0;

	if (!rom_filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (rom_loadaddress == 0) {
		fprintf(stderr, "E: You need to specify a valid "
			"-l/--load-address.\n");
		return 1;
	}

	if (rom_entrypoint == 0) {
		fprintf(stderr, "E: You need to specify a valid "
			"-e/--entry-point.\n");
		return 1;
	}

	compress = compression_function(rom_algo);
	if (!compress)
		return 1;

	rom = loadrom(cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	filedata = loadfile(rom_filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		fprintf(stderr, "E: Could not load file '%s'.\n",
			rom_filename);
		free(rom);
		return 1;
	}

	/* FIXME compressed file size might be bigger than original file */
	payload = calloc((2 * sizeof(struct cbfs_payload_segment)) + filesize, 1);
	if (payload == NULL) {
		fprintf(stderr, "E: Could not allocate memory.\n");
		free(filedata);
		free(rom);
		return 1;
	}

	segs = (struct cbfs_payload_segment *)payload;
	doffset = (2 * sizeof(struct cbfs_payload_segment));

	/* Prepare code segment */
	segs[0].type = PAYLOAD_SEGMENT_CODE;
	segs[0].load_addr = (uint64_t)htonll(rom_loadaddress);
	segs[0].mem_len = (uint32_t)htonl(filesize);
	segs[0].offset = (uint32_t)htonl(doffset);

	compress(filedata, filesize, (char *)(payload + doffset), &len);
	segs[0].compression = htonl(rom_algo);
	segs[0].len = htonl(len);

	if ((unsigned int)len >= filesize) {
		segs[0].compression = 0;
		segs[0].len = htonl(filesize);
		memcpy((char *)(payload + doffset), filedata, filesize);
	}

	/* prepare entry point segment */
	segs[1].type = PAYLOAD_SEGMENT_ENTRY;
	segs[1].load_addr = (uint64_t)htonll(rom_entrypoint);

	final_size = doffset + ntohl(segs[0].len);
	cbfsfile =
	    create_cbfs_file(rom_name, payload, &final_size,
			     CBFS_COMPONENT_PAYLOAD, &rom_baseaddress);

	free(filedata);
	free(payload);

	if (add_file_to_cbfs(cbfsfile, final_size, rom_baseaddress)) {
		fprintf(stderr, "E: Adding payload '%s' failed.\n",
			rom_filename);
		free(cbfsfile);
		free(rom);
		return 1;
	}
	if (writerom(cbfs_name, rom, romsize)) {
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
	void *rom;

	if (!rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	if (remove_file_from_cbfs(rom_name)) {
		fprintf(stderr, "E: Removing file '%s' failed.\n",
			rom_name);
		free(rom);
		return 1;
	}

	if (writerom(cbfs_name, rom, romsize)) {
		free(rom);
		return 1;
	}

	free(rom);
	return 0;
}

static int cbfs_create(void)
{
	if (rom_size == 0) {
		fprintf(stderr, "E: You need to specify a valid -s/--size.\n");
		return 1;
	}

	if (!rom_bootblock) {
		fprintf(stderr, "E: You need to specify -b/--bootblock.\n");
		return 1;
	}

	if (arch == CBFS_ARCHITECTURE_UNKNOWN) {
		fprintf(stderr, "E: You need to specify -m/--machine arch\n");
		return 1;
	}

	return create_cbfs_image(cbfs_name, rom_size, rom_bootblock,
						rom_alignment, rom_offset);
}

static int cbfs_locate(void)
{
	uint32_t filesize, location;

	if (!rom_filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	filesize = getfilesize(rom_filename);

	location = cbfs_find_location(cbfs_name, filesize,
					rom_name, rom_alignment);

	printf("%x\n", location);
	return location == 0 ? 1 : 0;
}

static int cbfs_print(void)
{
	void *rom;

	rom = loadrom(cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	print_cbfs_directory(cbfs_name);

	free(rom);
	return 0;
}

static int cbfs_extract(void)
{
	void *rom;
	int ret;

	if (!rom_filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!rom_name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			cbfs_name);
		return 1;
	}

	ret = extract_file_from_cbfs(cbfs_name, rom_name, rom_filename);

	free(rom);
	return ret;
}

static const struct command commands[] = {
	{"add", "f:n:t:b:h?", cbfs_add},
	{"add-payload", "f:n:t:c:b:h?", cbfs_add_payload},
	{"add-stage", "f:n:t:c:b:h?", cbfs_add_stage},
	{"add-flat-binary", "f:n:l:e:c:b:h?", cbfs_add_flat_binary},
	{"remove", "n:h?", cbfs_remove},
	{"create", "s:B:a:o:m:h?", cbfs_create},
	{"locate", "f:n:a:h?", cbfs_locate},
	{"print", "h?", cbfs_print},
	{"extract", "n:f:h?", cbfs_extract},
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
	     " %s FILE COMMAND [PARAMETERS]...\n\n" "OPTIONs:\n"
	     "  -h		Display this help message\n\n"
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
	     " locate -f FILE -n NAME -a align                             "
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

/* Small, OS/libc independent runtime check for endianess */
int host_bigendian = 0;

static void which_endian(void)
{
	static const uint32_t inttest = 0x12345678;
	uint8_t inttest_lsb = *(uint8_t *)&inttest;
	if (inttest_lsb == 0x12) {
		host_bigendian = 1;
	}
}

int main(int argc, char **argv)
{
	size_t i;
	int c;

	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	which_endian();

	cbfs_name = argv[1];
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
			if (index(commands[i].optstring, c) == NULL) {
				/* TODO maybe print actual long option instead */
				printf("%s: invalid option -- '%c'\n",
					argv[0], c);
				c = '?';
			}

			switch(c) {
			case 'n':
				rom_name = optarg;
				break;
			case 't':
				if (intfiletype(optarg) != ((uint64_t) - 1))
					rom_type = intfiletype(optarg);
				else
					rom_type = strtoul(optarg, NULL, 0);
				if (rom_type == 0)
					printf("W: Unknown type '%s' ignored\n",
							optarg);
				break;
			case 'c':
				if (!strncasecmp(optarg, "lzma", 5))
					rom_algo = CBFS_COMPRESS_LZMA;
				else if (!strncasecmp(optarg, "none", 5))
					rom_algo = CBFS_COMPRESS_NONE;
				else
					printf("W: Unknown compression '%s'"
							" ignored.\n", optarg);
				break;
			case 'b':
				rom_baseaddress = strtoul(optarg, NULL, 0);
				break;
			case 'l':
				rom_loadaddress = strtoul(optarg, NULL, 0);

				break;
			case 'e':
				rom_entrypoint = strtoul(optarg, NULL, 0);
				break;
			case 's':
				rom_size = strtoul(optarg, &suffix, 0);
				if (tolower(suffix[0])=='k') {
					rom_size *= 1024;
				}
				if (tolower(suffix[0])=='m') {
					rom_size *= 1024 * 1024;
				}
			case 'B':
				rom_bootblock = optarg;
				break;
			case 'a':
				rom_alignment = strtoul(optarg, NULL, 0);
				break;
			case 'o':
				rom_offset = strtoul(optarg, NULL, 0);
				break;
			case 'f':
				rom_filename = optarg;
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

	printf("Unknown command '%s'.\n", cmd);
	usage(argv[0]);
	return 1;
}
