/* SPDX-License-Identifier: GPL-2.0-only */
/* CSE FPT tool */

#include <commonlib/endian.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "cse_fpt.h"

static struct params {
	const char *output_dir;
	const char *partition_name;

	struct fpt_hdr_ops *hdr_ops;
} params;

#define FPT_ENTRY_TYPE_MASK		0x7f
#define FPT_ENTRY_TYPE_SHIFT		0
#define GET_FPT_ENTRY_TYPE(x)		(((x) >> FPT_ENTRY_TYPE_SHIFT) & FPT_ENTRY_TYPE_MASK)
#define FPT_ENTRY_TYPE_CODE		0x0
#define FPT_ENTRY_TYPE_DATA		0x1

#define FPT_ENTRY_VALID_MASK		0xff
#define FPT_ENTRY_VALID_SHIFT		24
#define GET_FPT_ENTRY_VALID(x)		(((x) >> FPT_ENTRY_VALID_SHIFT) & FPT_ENTRY_VALID_MASK)
#define FPT_ENTRY_INVALID		0xff
#define FPT_ENTRY_VALID			0x0

struct fpt_entry {
	uint8_t name[4];		/* ASCII short name */
	uint8_t rsvd1[4];
	uint32_t offset;		/* Offset in bytes from start of FPT binary */
	uint32_t length;		/* Size in bytes */
	uint8_t rsvd2[12];
	uint32_t flags;
} __packed;

static struct fpt {
	struct buffer input_buff;

	const struct fpt_hdr_ops *hdr_ops;

	fpt_hdr_ptr hdr;
	struct fpt_entry *entries;
} fpt;

static void usage(const char *name)
{
	printf("%s: Utility for CSE FPT\n\n"
		"USAGE:\n"
		" %s FILE COMMAND\n\n"
		"COMMANDs:\n"
		" print\n"
		" dump [-o OUTPUT_DIR] [-n NAME]\n"
		"\nOPTIONS:\n"
		" -o OUTPUT_DIR : Directory to dump the partition files in\n"
		" -n NAME       : Name of partition to dump\n"
		"\n",
		name, name);
}

void read_member(struct buffer *buff, void *dst, size_t size)
{
	uint8_t *src = buffer_get(buff);

	switch (size) {
	case 1:
		*(uint8_t *)dst = read_le8(src);
		break;
	case 2:
		*(uint16_t *)dst = read_le16(src);
		break;
	case 4:
		*(uint32_t *)dst = read_le32(src);
		break;
	case 8:
		*(uint64_t *)dst = read_le64(src);
		break;
	default:
		memcpy(dst, src, size);
	}

	buffer_seek(buff, size);
}

static int get_fpt_buff(struct buffer *input_buff, struct buffer *fpt_buff)
{
	/*
	 * FPT marker is typically at offset 0x10 in the released CSE binary. Check at offset
	 * 0x10 first and if that fails fall back to checking offset 0.
	 */
	const size_t fpt_offsets[] = { 0x10, 0 };
	size_t i;

	for (i = 0; i < ARRAY_SIZE(fpt_offsets); i++) {
		if (buffer_size(input_buff) < (strlen(FPT_MARKER) + fpt_offsets[i]))
			continue;

		const uint8_t *data = buffer_get(input_buff);

		if (!memcmp(data + fpt_offsets[i], FPT_MARKER, strlen(FPT_MARKER)))
			break;
	}

	if (i == ARRAY_SIZE(fpt_offsets)) {
		ERROR("Could not locate FPT at known offsets.\n");
		return -1;
	}

	buffer_clone(fpt_buff, input_buff);
	buffer_seek(fpt_buff, fpt_offsets[i]);

	return 0;
}

static int read_fpt_entries(struct buffer *buff)
{
	size_t i;
	struct fpt_entry *e;
	const size_t entries = fpt.hdr_ops->get_entry_count(fpt.hdr);
	const size_t fpt_entries_size = sizeof(struct fpt_entry) * entries;

	if (buffer_size(buff) < fpt_entries_size) {
		ERROR("Not enough bytes(actual=0x%zx, expected=0x%zx) for FPT entries!\n",
		      buffer_size(buff), fpt_entries_size);
		return -1;
	}

	e = fpt.entries = malloc(fpt_entries_size);

	for (i = 0; i < entries; i++, e++) {
		READ_MEMBER(buff, e->name);
		READ_MEMBER(buff, e->rsvd1);
		READ_MEMBER(buff, e->offset);
		READ_MEMBER(buff, e->length);
		READ_MEMBER(buff, e->rsvd2);
		READ_MEMBER(buff, e->flags);
	}

	return 0;
}

static const struct fpt_hdr_ops *get_fpt_hdr_ops(struct buffer *buff)
{
	static const struct fpt_hdr_ops *hdr_ops[] = {
		&fpt_hdr_20_ops,
		&fpt_hdr_21_ops,
	};

	for (size_t i = 0; i < ARRAY_SIZE(hdr_ops); i++) {
		if (hdr_ops[i]->match_version(buff))
			return hdr_ops[i];
	}

	return NULL;
}

static int fpt_parse(const char *image_name)
{
	struct buffer *input_buff = &fpt.input_buff;
	struct buffer fpt_buff;

	if (buffer_from_file(input_buff, image_name)) {
		ERROR("Failed to read input file %s\n", image_name);
		return -1;
	}

	if (get_fpt_buff(input_buff, &fpt_buff))
		return -1;

	fpt.hdr_ops = get_fpt_hdr_ops(&fpt_buff);
	if (fpt.hdr_ops == NULL) {
		ERROR("FPT header format not supported!\n");
		return -1;
	}

	fpt.hdr = fpt.hdr_ops->read(&fpt_buff);
	if (!fpt.hdr) {
		ERROR("Unable to read FPT header!\n");
		return -1;
	}

	return read_fpt_entries(&fpt_buff);
}

static bool is_partition_valid(const struct fpt_entry *e)
{
	return e->offset != 0 && e->length != 0 &&
		GET_FPT_ENTRY_VALID(e->flags) != FPT_ENTRY_INVALID;
}

static bool is_partition_code(const struct fpt_entry *e)
{
	return GET_FPT_ENTRY_TYPE(e->flags) == FPT_ENTRY_TYPE_CODE;
}

static void print_fpt_entry(const struct fpt_entry *e)
{
	printf("%-25s0x%-23x0x%-23x%c,%c (0x%.8x)\n",
	       e->name, e->offset, e->length,
	       is_partition_code(e) ? 'C' : 'D',
	       is_partition_valid(e) ? 'V' : 'I',
	       e->flags);
}

static void print_fpt_entries(const struct fpt_entry *e, size_t count)
{
	printf("\n * FPT entries\n");

	printf("%-25s%-25s%-25s%-25s\n", "Name", "Offset", "Size", "Flags");

	printf("=============================================================="
		"===============================\n");

	for (size_t i = 0; i < count; i++)
		print_fpt_entry(&e[i]);

	printf("=============================================================="
	       "================================\n");
	printf("Flags: I=invalid, V=valid, C=code, D=data\n");
}

static bool partition_name_match(const struct fpt_entry *e, const char *name)
{
	if (!name)
		return false;

	return !memcmp(e->name, name, sizeof(e->name));
}

static const struct fpt_entry *get_partition_entry(const char *name)
{
	for (size_t i = 0; i < fpt.hdr_ops->get_entry_count(fpt.hdr); i++) {
		if (partition_name_match(&fpt.entries[i], name))
			return &fpt.entries[i];
	}

	return NULL;
}

static int cmd_print(void)
{
	if (params.partition_name == NULL) {
		fpt.hdr_ops->print(fpt.hdr);
		print_fpt_entries(fpt.entries, fpt.hdr_ops->get_entry_count(fpt.hdr));
	} else {
		const struct fpt_entry *e = get_partition_entry(params.partition_name);
		if (e)
			print_fpt_entry(e);
		else {
			ERROR("Partition %s not found!\n", params.partition_name);
			return -1;
		}
	}
	return 0;
}

static bool should_dump_partition(const struct fpt_entry *e)
{
	if (!is_partition_valid(e)) {
		if (partition_name_match(e, params.partition_name)) {
			ERROR("Invalid partition requested to be dumped!\n");
			exit(-1);
		}
		return false;
	}

	/* Dump all partitions if no name provided. */
	if (params.partition_name == NULL)
		return true;

	return partition_name_match(e, params.partition_name);
}

static char *get_file_path(const struct fpt_entry *e)
{
	size_t filename_len = sizeof(e->name) + 1;
	char *filepath;

	/* output_dir name followed by '/' */
	if (params.output_dir)
		filename_len += strlen(params.output_dir) + 1;

	filepath = malloc(filename_len);
	if (!filepath)
		return NULL;

	snprintf(filepath, filename_len, "%s%s%s",
			params.output_dir ? : "",
			params.output_dir ? "/" : "",
			e->name);

	return filepath;
}

static int write_partition_to_file(const struct fpt_entry *e)
{
	size_t end_offset = e->offset + e->length - 1;
	struct buffer part_buffer;
	char *filepath;

	if (end_offset > buffer_size(&fpt.input_buff)) {
		ERROR("Offset out of bounds for the partition!\n");
		return -1;
	}

	filepath = get_file_path(e);
	if (!filepath) {
		ERROR("Failed to allocate space for filepath!\n");
		return -1;
	}

	printf("Dumping %.4s in %s\n", e->name, filepath);

	buffer_splice(&part_buffer, &fpt.input_buff, e->offset, e->length);
	buffer_write_file(&part_buffer, filepath);

	free(filepath);

	return 0;
}

static int cmd_dump(void)
{
	size_t i;
	bool found = false;
	struct stat sb;

	if (params.output_dir && (stat(params.output_dir, &sb) == -1)) {
		ERROR("Failed to stat %s: %s\n", params.output_dir, strerror(errno));
		return -1;
	}

	for (i = 0; i < fpt.hdr_ops->get_entry_count(fpt.hdr); i++) {
		if (!should_dump_partition(&fpt.entries[i]))
			continue;
		found = true;
		if (write_partition_to_file(&fpt.entries[i]))
			return -1;
	}

	if (found == false) {
		if (params.partition_name)
			ERROR("%s not found!\n", params.partition_name);
		ERROR("No partitions dumped!\n");
		return -1;
	}

	return 0;
}

static struct command {
	const char *name;
	const char *optstring;
	int (*function)(void);
} commands[] = {
	{ "print", "n:?", cmd_print },
	{ "dump", "n:o:?", cmd_dump },
};

static struct option long_options[] = {
	{"help",		required_argument,	0,	'h'},
	{"partition_name",	required_argument,	0,	'n'},
	{"output_dir",		required_argument,	0,	'o'},
	{NULL,			0,			0,	0 }
};

int main(int argc, char **argv)
{
	if (argc < 3) {
		ERROR("Incorrect number of args(%d)!\n", argc);
		usage(argv[0]);
		return 1;
	}

	const char *prog_name = argv[0];
	const char *image_name = argv[1];
	const char *cmd = argv[2];
	size_t i;

	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(cmd, commands[i].name))
			continue;

		int c;
		int option_index;

		while (1) {
			c = getopt_long(argc, argv, commands[i].optstring,
					long_options, &option_index);

			if (c == -1)
				break;

			if (strchr(commands[i].optstring, c) == NULL) {
				ERROR("Invalid option '%c'\n", c);
				usage(prog_name);
				return 1;
			}

			switch (c) {
			case 'o':
				params.output_dir = optarg;
				break;
			case 'n':
				params.partition_name = optarg;
				break;
			case 'h':
			case '?':
			default:
				usage(prog_name);
				return 1;
			}
		}

		break;
	}

	if (i == ARRAY_SIZE(commands)) {
		ERROR("No command match %s\n", cmd);
		usage(prog_name);
		return 1;
	}

	if (fpt_parse(image_name))
		return 1;

	return commands[i].function();
}
