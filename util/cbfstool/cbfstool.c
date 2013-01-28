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

struct command {
	const char *name;
	const char *optstring;
	int (*function) (void);
};

int verbose = 0;
struct param {
	char *cbfs_name;
	char *name;
	char *filename;
	char *bootblock;
	uint32_t type;
	uint32_t baseaddress;
	uint32_t loadaddress;
	uint32_t entrypoint;
	uint32_t size;
	uint32_t alignment;
	uint32_t offset;
	comp_algo algo;
} param;

static int cbfs_add(void)
{
	uint32_t filesize = 0;
	void *rom, *filedata, *cbfsfile;

	if (!param.filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (param.type == 0) {
		fprintf(stderr, "E: You need to specify a valid -t/--type.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	filedata = loadfile(param.filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		fprintf(stderr, "E: Could not load file '%s'.\n",
			param.filename);
		free(rom);
		return 1;
	}

	cbfsfile = create_cbfs_file(param.name, filedata, &filesize,
					param.type, &param.baseaddress);
	free(filedata);

	if (add_file_to_cbfs(cbfsfile, filesize, param.baseaddress)) {
		fprintf(stderr, "E: Adding file '%s' failed.\n", param.filename);
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
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	filedata = loadfile(param.filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		fprintf(stderr, "E: Could not load file '%s'.\n",
			param.filename);
		free(rom);
		return 1;
	}

	filesize = parse_elf_to_payload(filedata, &payload, param.algo);
	if (filesize <= 0) {
		fprintf(stderr, "E: Adding payload '%s' failed.\n",
			param.filename);
		free(rom);
		return 1;
	}

	cbfsfile = create_cbfs_file(param.name, payload, &filesize,
				CBFS_COMPONENT_PAYLOAD, &param.baseaddress);

	free(filedata);
	free(payload);

	if (add_file_to_cbfs(cbfsfile, filesize, param.baseaddress)) {
		fprintf(stderr, "E: Adding payload '%s' failed.\n",
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
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	filedata = loadfile(param.filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		fprintf(stderr, "E: Could not load file '%s'.\n",
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
		fprintf(stderr, "E: Adding stage '%s' failed.\n",
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
	uint32_t final_size;
	void *rom, *filedata, *cbfsfile;
	unsigned char *payload;
	comp_func_ptr compress;
	struct cbfs_payload_segment *segs;
	int doffset, len = 0;

	if (!param.filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	if (param.loadaddress == 0) {
		fprintf(stderr, "E: You need to specify a valid "
			"-l/--load-address.\n");
		return 1;
	}

	if (param.entrypoint == 0) {
		fprintf(stderr, "E: You need to specify a valid "
			"-e/--entry-point.\n");
		return 1;
	}

	compress = compression_function(param.algo);
	if (!compress)
		return 1;

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	filedata = loadfile(param.filename, &filesize, 0, SEEK_SET);
	if (filedata == NULL) {
		fprintf(stderr, "E: Could not load file '%s'.\n",
			param.filename);
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
	segs[0].load_addr = (uint64_t)htonll(param.loadaddress);
	segs[0].mem_len = (uint32_t)htonl(filesize);
	segs[0].offset = (uint32_t)htonl(doffset);

	compress(filedata, filesize, (char *)(payload + doffset), &len);
	segs[0].compression = htonl(param.algo);
	segs[0].len = htonl(len);

	if ((unsigned int)len >= filesize) {
		segs[0].compression = 0;
		segs[0].len = htonl(filesize);
		memcpy((char *)(payload + doffset), filedata, filesize);
	}

	/* prepare entry point segment */
	segs[1].type = PAYLOAD_SEGMENT_ENTRY;
	segs[1].load_addr = (uint64_t)htonll(param.entrypoint);

	final_size = doffset + ntohl(segs[0].len);
	cbfsfile =
	    create_cbfs_file(param.name, payload, &final_size,
			     CBFS_COMPONENT_PAYLOAD, &param.baseaddress);

	free(filedata);
	free(payload);

	if (add_file_to_cbfs(cbfsfile, final_size, param.baseaddress)) {
		fprintf(stderr, "E: Adding payload '%s' failed.\n",
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
	void *rom;

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	if (remove_file_from_cbfs(param.name)) {
		fprintf(stderr, "E: Removing file '%s' failed.\n",
			param.name);
		free(rom);
		return 1;
	}

	if (writerom(param.cbfs_name, rom, romsize)) {
		free(rom);
		return 1;
	}

	free(rom);
	return 0;
}

static int cbfs_create(void)
{
	if (param.size == 0) {
		fprintf(stderr, "E: You need to specify a valid -s/--size.\n");
		return 1;
	}

	if (!param.bootblock) {
		fprintf(stderr, "E: You need to specify -b/--bootblock.\n");
		return 1;
	}

	if (arch == CBFS_ARCHITECTURE_UNKNOWN) {
		fprintf(stderr, "E: You need to specify -m/--machine arch\n");
		return 1;
	}

	return create_cbfs_image(param.cbfs_name, param.size, param.bootblock,
						param.alignment, param.offset);
}

static int cbfs_locate(void)
{
	uint32_t filesize, location;

	if (!param.filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	filesize = getfilesize(param.filename);

	location = cbfs_find_location(param.cbfs_name, filesize,
					param.name, param.alignment);

	printf("%x\n", location);
	return location == 0 ? 1 : 0;
}

static int cbfs_print(void)
{
	void *rom;

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	print_cbfs_directory(param.cbfs_name);

	free(rom);
	return 0;
}

static int cbfs_extract(void)
{
	void *rom;
	int ret;

	if (!param.filename) {
		fprintf(stderr, "E: You need to specify -f/--filename.\n");
		return 1;
	}

	if (!param.name) {
		fprintf(stderr, "E: You need to specify -n/--name.\n");
		return 1;
	}

	rom = loadrom(param.cbfs_name);
	if (rom == NULL) {
		fprintf(stderr, "E: Could not load ROM image '%s'.\n",
			param.cbfs_name);
		return 1;
	}

	ret = extract_file_from_cbfs(param.cbfs_name, param.name, param.filename);

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
				if (intfiletype(optarg) != ((uint64_t) - 1))
					param.type = intfiletype(optarg);
				else
					param.type = strtoul(optarg, NULL, 0);
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
				param.baseaddress = strtoul(optarg, NULL, 0);
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
			case 'a':
				param.alignment = strtoul(optarg, NULL, 0);
				break;
			case 'o':
				param.offset = strtoul(optarg, NULL, 0);
				break;
			case 'f':
				param.filename = optarg;
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
