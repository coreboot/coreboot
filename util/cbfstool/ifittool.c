/* cbfstool, CLI utility for creating rmodules */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "common.h"
#include "cbfs_image.h"
#include "partitioned_file.h"
#include "fit.h"

/* Global variables */
partitioned_file_t *image_file;

static const char *optstring  = "H:j:f:r:d:t:n:s:cAaDvhF?";
static struct option long_options[] = {
	{"file",            required_argument, 0, 'f' },
	{"region",          required_argument, 0, 'r' },
	{"add-cbfs-entry",  no_argument,       0, 'a' },
	{"add-region",      no_argument,       0, 'A' },
	{"del-entry",       required_argument, 0, 'd' },
	{"clear-table",     no_argument,       0, 'c' },
	{"set-fit-pointer", no_argument,       0, 'F' },
	{"fit-type",        required_argument, 0, 't' },
	{"cbfs-filename",   required_argument, 0, 'n' },
	{"max-table-size",  required_argument, 0, 's' },
	{"topswap-size",    required_argument, 0, 'j' },
	{"dump",            no_argument,       0, 'D' },
	{"verbose",         no_argument,       0, 'v' },
	{"help",            no_argument,       0, 'h' },
	{"header-offset",   required_argument, 0, 'H' },
	{NULL,              0,                 0,  0  }
};

static void usage(const char *name)
{
	printf(
		"ifittool: utility for modifying Intel Firmware Interface Table\n\n"
		"USAGE: %s [-h] [-H] [-v] [-D] [-c] <-f|--file name> <-s|--max-table-size size> <-r|--region fmap region> OPERATION\n"
		"\tOPERATION:\n"
		"\t\t-a|--add-entry        :   Add a CBFS file as new entry to FIT\n"
		"\t\t-A|--add-region       :   Add region as new entry to FIT (for microcodes)\n"
		"\t\t-d|--del-entry number :   Delete existing <number> entry\n"
		"\t\t-F|--set-fit-pointer  :   Set the FIT pointer to a CBFS file\n"
		"\t\t-t|--fit-type         :   Type of new entry\n"
		"\t\t-n|--name             :   The CBFS filename or region to add to table\n"
		"\tOPTIONAL ARGUMENTS:\n"
		"\t\t-h|--help             :   Display this text\n"
		"\t\t-H|--header-offset    :   Do not search for header, use this offset\n"
		"\t\t-v|--verbose          :   Be verbose\n"
		"\t\t-D|--dump             :   Dump FIT table (at end of operation)\n"
		"\t\t-c|--clear-table      :   Remove all existing entries (do not update)\n"
		"\t\t-j|--topswap-size     :   Use second FIT table if non zero\n"
		"\tREQUIRED ARGUMENTS:\n"
		"\t\t-f|--file name        :   The file containing the CBFS\n"
		"\t\t-s|--max-table-size   :   The number of possible FIT entries in table\n"
		"\t\t-r|--region           :   The FMAP region to operate on\n"
	, name);
}

static int is_valid_topswap(size_t topswap_size)
{
	switch (topswap_size) {
	case (64 * KiB):
	case (128 * KiB):
	case (256 * KiB):
	case (512 * KiB):
	case (1 * MiB):
		break;
	default:
		ERROR("Invalid topswap_size %zd\n", topswap_size);
		ERROR("topswap can be 64K|128K|256K|512K|1M\n");
		return 0;
	}
	return 1;
}

/*
 * Converts between offsets from the start of the specified image region and
 * "top-aligned" offsets from the top of the entire boot media. See comment
 * below for convert_to_from_top_aligned() about forming addresses.
 */
static unsigned int convert_to_from_absolute_top_aligned(
		const struct buffer *region, unsigned int offset)
{
	assert(region);

	size_t image_size = partitioned_file_total_size(image_file);

	return image_size - region->offset - offset;
}

/*
 * Converts between offsets from the start of the specified image region and
 * "top-aligned" offsets from the top of the image region. Works in either
 * direction: pass in one type of offset and receive the other type.
 * N.B. A top-aligned offset is always a positive number, and should not be
 * confused with a top-aligned *address*, which is its arithmetic inverse. */
static unsigned int convert_to_from_top_aligned(const struct buffer *region,
						unsigned int offset)
{
	assert(region);

	/* Cover the situation where a negative base address is given by the
	 * user. Callers of this function negate it, so it'll be a positive
	 * number smaller than the region.
	 */
	if ((offset > 0) && (offset < region->size))
		return region->size - offset;

	return convert_to_from_absolute_top_aligned(region, offset);
}

/*
 * Get a pointer from an offset. This function assumes the ROM is located
 * in the host address space at [4G - romsize -> 4G). It also assume all
 * pointers have values within this address range.
 */
static inline uint32_t offset_to_ptr(fit_offset_converter_t helper,
				     const struct buffer *region, int offset)
{
	return -helper(region, offset);
}

enum fit_operation {
	NO_OP = 0,
	ADD_CBFS_OP,
	ADD_REGI_OP,
	DEL_OP,
	SET_FIT_PTR_OP
};

int main(int argc, char *argv[])
{
	int c;
	const char *input_file = NULL;
	const char *name = NULL;
	const char *region_name = NULL;
	enum fit_operation op = NO_OP;
	bool dump = false, clear_table = false;
	size_t max_table_size = 0;
	size_t table_entry = 0;
	uint32_t addr = 0;
	size_t topswap_size = 0;
	enum fit_type fit_type = 0;
	uint32_t headeroffset = ~0u;

	verbose = 0;

	if (argc < 4) {
		usage(argv[0]);
		return 1;
	}

	while (1) {
		int optindex = 0;
		char *suffix = NULL;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case 'h':
			usage(argv[0]);
			return 1;
		case 'a':
			if (op != NO_OP) {
				ERROR("specified multiple actions at once\n");
				usage(argv[0]);
				return 1;
			}
			op = ADD_CBFS_OP;
			break;
		case 'A':
			if (op != NO_OP) {
				ERROR("specified multiple actions at once\n");
				usage(argv[0]);
				return 1;
			}
			op = ADD_REGI_OP;
			break;
		case 'c':
			clear_table = true;
			break;
		case 'd':
			if (op != NO_OP) {
				ERROR("specified multiple actions at once\n");
				usage(argv[0]);
				return 1;
			}
			op = DEL_OP;
			table_entry = atoi(optarg);
			break;
		case 'D':
			dump = true;
			break;
		case 'f':
			input_file = optarg;
			break;
		case 'F':
			if (op != NO_OP) {
				ERROR("specified multiple actions at once\n");
				usage(argv[0]);
				return 1;
			}
			op = SET_FIT_PTR_OP;
			break;
		case 'H':
			headeroffset = strtoul(optarg, &suffix, 0);
			if (!*optarg || (suffix && *suffix)) {
				ERROR("Invalid header offset '%s'.\n", optarg);
				return 1;
			}
			break;
		case 'j':
			topswap_size = strtol(optarg, NULL, 0);
			if (!is_valid_topswap(topswap_size))
				return 1;
			break;
		case 'n':
			name = optarg;
			break;
		case 'r':
			region_name = optarg;
			break;
		case 's':
			max_table_size = atoi(optarg);
			break;
		case 't':
			fit_type = atoi(optarg);
			break;
		case 'v':
			verbose++;
			break;
		default:
			break;
		}
	}

	if (input_file == NULL) {
		ERROR("No input file given\n");
		usage(argv[0]);
		return 1;
	}

	if (op == ADD_CBFS_OP || op == ADD_REGI_OP) {
		if (fit_type == 0) {
			ERROR("Adding FIT entry, but no type given\n");
			usage(argv[0]);
			return 1;
		} else if (name == NULL) {
			ERROR("Adding FIT entry, but no name set\n");
			usage(argv[0]);
			return 1;
		} else if (max_table_size == 0) {
			ERROR("Maximum table size not given\n");
			usage(argv[0]);
			return 1;
		}
	}

	if (op == SET_FIT_PTR_OP) {
		if (name == NULL) {
			ERROR("Adding FIT entry, but no name set\n");
			usage(argv[0]);
			return 1;
		}
	}

	if (!region_name) {
		ERROR("Region not given\n");
		usage(argv[0]);
		return 1;
	}

	image_file = partitioned_file_reopen(input_file,
					     op != NO_OP || clear_table);

	struct buffer image_region;

	if (!partitioned_file_read_region(&image_region, image_file,
					  region_name)) {
		partitioned_file_close(image_file);
		ERROR("The image will be left unmodified.\n");
		return 1;
	}

	struct buffer bootblock;
	// The bootblock is part of the CBFS on x86
	buffer_clone(&bootblock, &image_region);

	struct cbfs_image image;
	if (cbfs_image_from_buffer(&image, &image_region, headeroffset)) {
		partitioned_file_close(image_file);
		return 1;
	}

	struct fit_table *fit = NULL;
	if (op != SET_FIT_PTR_OP) {
		fit = fit_get_table(&bootblock, convert_to_from_top_aligned, topswap_size);
		if (!fit) {
			partitioned_file_close(image_file);
			ERROR("FIT not found.\n");
			return 1;
		}
		if (clear_table) {
			if (fit_clear_table(fit)) {
				partitioned_file_close(image_file);
				ERROR("Failed to clear table.\n");
				return 1;
			}
		}
	}

	switch (op) {
	case ADD_REGI_OP:
	{
		struct buffer region;

		if (partitioned_file_read_region(&region, image_file, name)) {
			addr = -convert_to_from_top_aligned(&region, 0);
		} else {
			partitioned_file_close(image_file);
			return 1;
		}

		if (fit_add_entry(fit, addr, 0, fit_type, max_table_size)) {
			partitioned_file_close(image_file);
			ERROR("Adding type %u FIT entry\n", fit_type);
			return 1;
		}
		break;
	}
	case ADD_CBFS_OP:
	{
		if (fit_type == FIT_TYPE_MICROCODE) {
			if (fit_add_microcode_file(fit, &image, name,
						   convert_to_from_top_aligned,
						   max_table_size)) {
				return 1;
			}
		} else {
			uint32_t offset, len;
			struct cbfs_file *cbfs_file;

			cbfs_file = cbfs_get_entry(&image, name);
			if (!cbfs_file) {
				partitioned_file_close(image_file);
				ERROR("%s not found in CBFS.\n", name);
				return 1;
			}

			len = be32toh(cbfs_file->len);
			offset = offset_to_ptr(convert_to_from_top_aligned,
					&image.buffer,
					cbfs_get_entry_addr(&image, cbfs_file) +
					be32toh(cbfs_file->offset));


			if (fit_add_entry(fit, offset, len, fit_type,
					  max_table_size)) {
				partitioned_file_close(image_file);
				ERROR("Adding type %u FIT entry\n", fit_type);
				return 1;
			}
		}
		break;
	}
	case SET_FIT_PTR_OP:
	{
		uint32_t fit_address;
		struct cbfs_file *cbfs_file = cbfs_get_entry(&image, name);
		if (!cbfs_file) {
			partitioned_file_close(image_file);
			ERROR("%s not found in CBFS.\n", name);
			return 1;
		}

		fit_address = offset_to_ptr(convert_to_from_top_aligned, &image.buffer,
				       cbfs_get_entry_addr(&image, cbfs_file)
					       + be32toh(cbfs_file->offset));


		if (set_fit_pointer(&bootblock, fit_address, convert_to_from_top_aligned,
				    topswap_size)) {
			partitioned_file_close(image_file);
			ERROR("%s is not a FIT table\n", name);
			return 1;
		}
		fit = fit_get_table(&bootblock, convert_to_from_top_aligned, topswap_size);

		if (clear_table) {
			if (fit_clear_table(fit)) {
				partitioned_file_close(image_file);
				ERROR("Failed to clear table.\n");
				return 1;
			}
		}

		break;
	}
	case DEL_OP:
	{
		if (fit_delete_entry(fit, table_entry)) {
			partitioned_file_close(image_file);
			ERROR("Deleting FIT entry %zu failed\n", table_entry);
			return 1;
		}
		break;
	}
	case NO_OP:
	default:
		break;
	}

	if (op != NO_OP || clear_table) {
		if (!partitioned_file_write_region(image_file, &bootblock)) {
			ERROR("Failed to write changes to disk.\n");
			partitioned_file_close(image_file);
			return 1;
		}
	}

	if (dump) {
		if (fit_dump(fit)) {
			partitioned_file_close(image_file);
			return 1;
		}
	}

	partitioned_file_close(image_file);

	return 0;
}
