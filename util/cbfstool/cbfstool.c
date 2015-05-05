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
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include "common.h"
#include "cbfs.h"
#include "cbfs_image.h"
#include "fit.h"

struct command {
	const char *name;
	const char *optstring;
	int (*function) (void);
};

static struct param {
	char *cbfs_name;
	char *name;
	char *filename;
	char *bootblock;
	char *ignore_section;
	uint64_t u64val;
	uint32_t type;
	uint32_t baseaddress;
	uint32_t baseaddress_assigned;
	uint32_t loadaddress;
	uint32_t copyoffset;
	uint32_t copyoffset_assigned;
	uint32_t headeroffset;
	uint32_t headeroffset_assigned;
	uint32_t entrypoint;
	uint32_t size;
	uint32_t alignment;
	uint32_t pagesize;
	uint32_t cbfsoffset;
	uint32_t cbfsoffset_assigned;
	uint32_t top_aligned;
	uint32_t arch;
	int fit_empty_entries;
	comp_algo algo;
	/* for linux payloads */
	char *initrd;
	char *cmdline;
} param = {
	/* All variables not listed are initialized as zero. */
	.arch = CBFS_ARCHITECTURE_UNKNOWN,
	.algo = CBFS_COMPRESS_NONE,
	.headeroffset = ~0,
};

typedef int (*convert_buffer_t)(struct buffer *buffer, uint32_t *offset);

static int cbfs_add_integer_component(const char *cbfs_name,
			      const char *name,
			      uint64_t u64val,
			      uint32_t offset,
			      uint32_t headeroffset) {
	struct cbfs_image image;
	struct buffer buffer;
	int i, ret = 1;

	if (!name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (buffer_create(&buffer, 8, name) != 0)
		return 1;

	for (i = 0; i < 8; i++)
		buffer.data[i] = (u64val >> i*8) & 0xff;

	if (cbfs_image_from_file(&image, cbfs_name, headeroffset) != 0) {
		ERROR("Could not load ROM image '%s'.\n", cbfs_name);
		buffer_delete(&buffer);
		return 1;
	}

	if (cbfs_get_entry(&image, name)) {
		ERROR("'%s' already in ROM image.\n", name);
		goto done;
	}

	if (cbfs_add_entry(&image, &buffer, name, CBFS_COMPONENT_RAW, offset) !=
									0) {
		ERROR("Failed to add %llu into ROM image as '%s'.\n",
					(long long unsigned)u64val, name);
		goto done;
	}

	if (cbfs_image_write_file(&image, cbfs_name) == 0)
		ret = 0;

done:
	buffer_delete(&buffer);
	cbfs_image_delete(&image);
	return ret;
}

static int cbfs_add_component(const char *cbfs_name,
			      const char *filename,
			      const char *name,
			      uint32_t type,
			      uint32_t offset,
			      uint32_t headeroffset,
			      convert_buffer_t convert)
{
	struct cbfs_image image;
	struct buffer buffer;

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

	if (cbfs_image_from_file(&image, cbfs_name, headeroffset))
		return 1;

	if (buffer_from_file(&buffer, filename) != 0) {
		ERROR("Could not load file '%s'.\n", filename);
		cbfs_image_delete(&image);
		return 1;
	}

	if (convert && convert(&buffer, &offset) != 0) {
		ERROR("Failed to parse file '%s'.\n", filename);
		buffer_delete(&buffer);
		cbfs_image_delete(&image);
		return 1;
	}

	if (cbfs_get_entry(&image, name)) {
		ERROR("'%s' already in ROM image.\n", name);
		buffer_delete(&buffer);
		cbfs_image_delete(&image);
		return 1;
	}

	if (cbfs_add_entry(&image, &buffer, name, type, offset) != 0) {
		ERROR("Failed to add '%s' into ROM image.\n", filename);
		buffer_delete(&buffer);
		cbfs_image_delete(&image);
		return 1;
	}

	if (cbfs_image_write_file(&image, cbfs_name) != 0) {
		buffer_delete(&buffer);
		cbfs_image_delete(&image);
		return 1;
	}

	buffer_delete(&buffer);
	cbfs_image_delete(&image);
	return 0;
}

static int cbfstool_convert_mkstage(struct buffer *buffer, uint32_t *offset)
{
	struct buffer output;
	int ret;
	ret = parse_elf_to_stage(buffer, &output, param.algo, offset,
							param.ignore_section);
	if (ret != 0)
		return -1;
	buffer_delete(buffer);
	// direct assign, no dupe.
	memcpy(buffer, &output, sizeof(*buffer));
	return 0;
}

static int cbfstool_convert_mkpayload(struct buffer *buffer,
						unused uint32_t *offset)
{
	struct buffer output;
	int ret;
	/* per default, try and see if payload is an ELF binary */
	ret = parse_elf_to_payload(buffer, &output, param.algo);

	/* If it's not an ELF, see if it's a UEFI FV */
	if (ret != 0)
		ret = parse_fv_to_payload(buffer, &output, param.algo);

	/* If it's neither ELF nor UEFI Fv, try bzImage */
	if (ret != 0)
		ret = parse_bzImage_to_payload(buffer, &output,
				param.initrd, param.cmdline, param.algo);

	/* Not a supported payload type */
	if (ret != 0) {
		ERROR("Not a supported payload type (ELF / FV).\n");
		return -1;
	}

	buffer_delete(buffer);
	// direct assign, no dupe.
	memcpy(buffer, &output, sizeof(*buffer));
	return 0;
}

static int cbfstool_convert_mkflatpayload(struct buffer *buffer,
					  unused uint32_t *offset)
{
	struct buffer output;
	if (parse_flat_binary_to_payload(buffer, &output,
					 param.loadaddress,
					 param.entrypoint,
					 param.algo) != 0) {
		return -1;
	}
	buffer_delete(buffer);
	// direct assign, no dupe.
	memcpy(buffer, &output, sizeof(*buffer));
	return 0;
}


static int cbfs_add(void)
{
	return cbfs_add_component(param.cbfs_name,
				  param.filename,
				  param.name,
				  param.type,
				  param.baseaddress,
				  param.headeroffset,
				  NULL);
}

static int cbfs_add_stage(void)
{
	return cbfs_add_component(param.cbfs_name,
				  param.filename,
				  param.name,
				  CBFS_COMPONENT_STAGE,
				  param.baseaddress,
				  param.headeroffset,
				  cbfstool_convert_mkstage);
}

static int cbfs_add_payload(void)
{
	return cbfs_add_component(param.cbfs_name,
				  param.filename,
				  param.name,
				  CBFS_COMPONENT_PAYLOAD,
				  param.baseaddress,
				  param.headeroffset,
				  cbfstool_convert_mkpayload);
}

static int cbfs_add_flat_binary(void)
{
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
	return cbfs_add_component(param.cbfs_name,
				  param.filename,
				  param.name,
				  CBFS_COMPONENT_PAYLOAD,
				  param.baseaddress,
				  param.headeroffset,
				  cbfstool_convert_mkflatpayload);
}

static int cbfs_add_integer(void)
{
	return cbfs_add_integer_component(param.cbfs_name,
				  param.name,
				  param.u64val,
				  param.baseaddress,
				  param.headeroffset);
}

static int cbfs_remove(void)
{
	struct cbfs_image image;

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name, param.headeroffset))
		return 1;

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
	return 0;
}

static int cbfs_create(void)
{
	struct cbfs_image image;
	struct buffer bootblock;

	if (param.size == 0) {
		ERROR("You need to specify a valid -s/--size.\n");
		return 1;
	}

	if (param.arch == CBFS_ARCHITECTURE_UNKNOWN) {
		ERROR("You need to specify -m/--machine arch.\n");
		return 1;
	}

	if (!param.bootblock) {
		DEBUG("-B not given, creating image without bootblock.\n");
		buffer_create(&bootblock, 0, "(dummy)");
	} else if (buffer_from_file(&bootblock, param.bootblock)) {
		return 1;
	}

	if (!param.alignment)
		param.alignment = 64;	// default CBFS entry alignment

	// Set default offsets. x86, as usual, needs to be a special snowflake.
	if (!param.baseaddress_assigned) {
		if (param.arch == CBFS_ARCHITECTURE_X86) {
			// Make sure there's at least enough room for rel_offset
			param.baseaddress = param.size - (
				bootblock.size > sizeof(int32_t) ?
				bootblock.size : sizeof(int32_t));
			DEBUG("x86 -> bootblock lies at end of ROM (%#x).\n",
			      param.baseaddress);
		} else {
			param.baseaddress = 0;
			DEBUG("bootblock starts at address 0x0.\n");
		}
	}
	if (!param.headeroffset_assigned) {
		if (param.arch == CBFS_ARCHITECTURE_X86) {
			param.headeroffset = param.baseaddress -
					     sizeof(struct cbfs_header);
			DEBUG("x86 -> CBFS header before bootblock (%#x).\n",
				param.headeroffset);
		} else {
			param.headeroffset = align_up(param.baseaddress +
				bootblock.size, sizeof(uint32_t));
			DEBUG("CBFS header placed behind bootblock (%#x).\n",
				param.headeroffset);
		}
	}
	if (!param.cbfsoffset_assigned) {
		if (param.arch == CBFS_ARCHITECTURE_X86) {
			param.cbfsoffset = 0;
			DEBUG("x86 -> CBFS entries start at address 0x0.\n");
		} else {
			param.cbfsoffset = align_up(param.headeroffset +
						    sizeof(struct cbfs_header),
						    param.alignment);
			DEBUG("CBFS entries start beind master header (%#x).\n",
			      param.cbfsoffset);
		}
	}

	if (cbfs_image_create(&image,
			      param.arch,
			      param.size,
			      param.alignment,
			      &bootblock,
			      param.baseaddress,
			      param.headeroffset,
			      param.cbfsoffset) != 0) {
		ERROR("Failed to create %s.\n", param.cbfs_name);
		return 1;
	}
	buffer_delete(&bootblock);

	if (cbfs_image_write_file(&image, param.cbfs_name) != 0) {
		ERROR("Failed to write %s.\n", param.cbfs_name);
		cbfs_image_delete(&image);
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

	if (cbfs_image_from_file(&image, param.cbfs_name, param.headeroffset))
		return 1;

	if (cbfs_get_entry(&image, param.name))
		WARN("'%s' already in CBFS.\n", param.name);

	if (buffer_from_file(&buffer, param.filename) != 0) {
		ERROR("Cannot load %s.\n", param.filename);
		cbfs_image_delete(&image);
		return 1;
	}

	address = cbfs_locate_entry(&image, param.name, buffer.size,
				    param.pagesize, param.alignment);
	buffer_delete(&buffer);

	if (address == -1) {
		ERROR("'%s' can't fit in CBFS for page-size %#x, align %#x.\n",
		      param.name, param.pagesize, param.alignment);
		cbfs_image_delete(&image);
		return 1;
	}

	if (param.top_aligned)
		address = address - image.header.romsize;

	cbfs_image_delete(&image);
	printf("0x%x\n", address);
	return 0;
}

static int cbfs_print(void)
{
	struct cbfs_image image;
	if (cbfs_image_from_file(&image, param.cbfs_name, param.headeroffset))
		return 1;

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

	if (cbfs_image_from_file(&image, param.cbfs_name, param.headeroffset))
		result = 1;
	else if (cbfs_export_entry(&image, param.name,
				   param.filename))
		result = 1;

	cbfs_image_delete(&image);
	return result;
}

static int cbfs_update_fit(void)
{
	int ret = 0;
	struct cbfs_image image;

	if (!param.name) {
		ERROR("You need to specify -n/--name.\n");
		return 1;
	}

	if (param.fit_empty_entries <= 0) {
		ERROR("Invalid number of fit entries "
		        "(-x/--empty-fits): %d\n", param.fit_empty_entries);
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name, param.headeroffset))
		return 1;

	ret = fit_update_table(&image, param.fit_empty_entries, param.name);
	if (!ret)
		ret = cbfs_image_write_file(&image, param.cbfs_name);

	cbfs_image_delete(&image);
	return ret;
}

static int cbfs_copy(void)
{
	struct cbfs_image image;

	if (!param.copyoffset_assigned) {
		ERROR("You need to specify -D/--copy_offset.\n");
		return 1;
	}

	if (!param.size) {
		ERROR("You need to specify -s/--size.\n");
		return 1;
	}

	if (cbfs_image_from_file(&image, param.cbfs_name,
				 param.headeroffset) != 0)
		return 1;

	if (cbfs_copy_instance(&image, param.copyoffset, param.size))
		return 1;

	/* Save the new image. */
	return buffer_write_file(&image.buffer, param.cbfs_name);

}

static const struct command commands[] = {
	{"add", "H;f:n:t:b:vh?", cbfs_add},
	{"add-flat-binary", "H:f:n:l:e:c:b:vh?", cbfs_add_flat_binary},
	{"add-payload", "H:f:n:t:c:b:vh?C:I:", cbfs_add_payload},
	{"add-stage", "H:f:n:t:c:b:S:vh?", cbfs_add_stage},
	{"add-int", "H:i:n:b:vh?", cbfs_add_integer},
	{"create", "s:B:b:H:a:o:m:vh?", cbfs_create},
	{"copy", "H:D:s:", cbfs_copy},
	{"extract", "H:n:f:vh?", cbfs_extract},
	{"locate", "H:f:n:P:a:Tvh?", cbfs_locate},
	{"print", "H:vh?", cbfs_print},
	{"remove", "H:n:vh?", cbfs_remove},
	{"update-fit", "H:n:x:vh?", cbfs_update_fit},
};

static struct option long_options[] = {
	{"alignment",     required_argument, 0, 'a' },
	{"base-address",  required_argument, 0, 'b' },
	{"bootblock",     required_argument, 0, 'B' },
	{"cmdline",       required_argument, 0, 'C' },
	{"compression",   required_argument, 0, 'c' },
	{"copy-offset",   required_argument, 0, 'D' },
	{"empty-fits",    required_argument, 0, 'x' },
	{"entry-point",   required_argument, 0, 'e' },
	{"file",          required_argument, 0, 'f' },
	{"header-offset", required_argument, 0, 'H' },
	{"help",          no_argument,       0, 'h' },
	{"ignore-sec",    required_argument, 0, 'S' },
	{"initrd",        required_argument, 0, 'I' },
	{"int",           required_argument, 0, 'i' },
	{"load-address",  required_argument, 0, 'l' },
	{"machine",       required_argument, 0, 'm' },
	{"name",          required_argument, 0, 'n' },
	{"offset",        required_argument, 0, 'o' },
	{"page-size",     required_argument, 0, 'P' },
	{"size",          required_argument, 0, 's' },
	{"top-aligned",   required_argument, 0, 'T' },
	{"type",          required_argument, 0, 't' },
	{"verbose",       no_argument,       0, 'v' },
	{NULL,            0,                 0,  0  }
};

static void usage(char *name)
{
	printf
	    ("cbfstool: Management utility for CBFS formatted ROM images\n\n"
	     "USAGE:\n" " %s [-h]\n"
	     " %s FILE COMMAND [-v] [PARAMETERS]...\n\n" "OPTIONs:\n"
	     "  -H header_offset  Do not search for header, use this offset\n"
	     "  -T                Output top-aligned memory address\n"
	     "  -v                Provide verbose output\n"
	     "  -h                Display this help message\n\n"
	     "COMMANDs:\n"
	     " add -f FILE -n NAME -t TYPE [-b base-address]               "
			"Add a component\n"
	     " add-payload -f FILE -n NAME [-c compression] [-b base]      "
			"Add a payload to the ROM\n"
	     "        (linux specific: [-C cmdline] [-I initrd])\n"
	     " add-stage -f FILE -n NAME [-c compression] [-b base] \\\n"
	     "        [-S section-to-ignore]                               "
			"Add a stage to the ROM\n"
	     " add-flat-binary -f FILE -n NAME -l load-address \\\n"
	     "        -e entry-point [-c compression] [-b base]            "
			"Add a 32bit flat mode binary\n"
	     " add-int -i INTEGER -n NAME [-b base]                        "
			"Add a raw 64-bit integer value\n"
	     " remove -n NAME                                              "
			"Remove a component\n"
	     " copy -D new_header_offset -s region size \\\n"
	     "        [-H source header offset]         "
			"Create a copy (duplicate) cbfs instance\n"
	     " create -s size -m ARCH [-B bootblock] [-b bootblock offset] \\\n"
	     "        [-o CBFS offset] [-H header offset] [-a align]       "
			"Create a ROM file\n"
	     " locate -f FILE -n NAME [-P page-size] [-a align] [-T]       "
			"Find a place for a file of that size\n"
	     " print                                                       "
			"Show the contents of the ROM\n"
	     " extract -n NAME -f FILE                                     "
			"Extracts a raw payload from ROM\n"
	     " update-fit -n MICROCODE_BLOB_NAME -x EMTPY_FIT_ENTRIES\n  "
			"Updates the FIT table with microcode entries\n"
	     "\n"
	     "OFFSETs:\n"
	     "  Numbers accompanying -b, -H, and -o switches may be provided\n"
	     "  in two possible formats: if their value is greater than\n"
	     "  0x80000000, they are interpreted as a top-aligned x86 memory\n"
	     "  address; otherwise, they are treated as an offset into flash.\n"
	     "ARCHes:\n"
	     "  arm64, arm, mips, x86\n"
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
				break;
			case 'B':
				param.bootblock = optarg;
				break;
			case 'H':
				param.headeroffset = strtoul(
						optarg, NULL, 0);
				param.headeroffset_assigned = 1;
				break;
			case 'D':
				param.copyoffset = strtoul(optarg, NULL, 0);
				param.copyoffset_assigned = 1;
				break;
			case 'a':
				param.alignment = strtoul(optarg, NULL, 0);
				break;
			case 'P':
				param.pagesize = strtoul(optarg, NULL, 0);
				break;
			case 'o':
				param.cbfsoffset = strtoul(optarg, NULL, 0);
				param.cbfsoffset_assigned = 1;
				break;
			case 'f':
				param.filename = optarg;
				break;
			case 'i':
				param.u64val = strtoull(optarg, NULL, 0);
				break;
			case 'T':
				param.top_aligned = 1;
				break;
			case 'x':
				param.fit_empty_entries = strtol(optarg, NULL, 0);
				break;
			case 'v':
				verbose++;
				break;
			case 'm':
				param.arch = string_to_arch(optarg);
				break;
			case 'I':
				param.initrd = optarg;
				break;
			case 'C':
				param.cmdline = optarg;
				break;
			case 'S':
				param.ignore_section = optarg;
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
