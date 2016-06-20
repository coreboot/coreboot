/*
 * fmaptool, CLI utility for converting plaintext fmd files into fmap blobs
 *
 * Copyright (C) 2015 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "common.h"
#include "cbfs_sections.h"
#include "fmap_from_fmd.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define STDIN_FILENAME_SENTINEL "-"

#define HEADER_FMAP_OFFSET "FMAP_OFFSET"

enum fmaptool_return {
	FMAPTOOL_EXIT_SUCCESS = 0,
	FMAPTOOL_EXIT_BAD_ARGS,
	FMAPTOOL_EXIT_BAD_INPUT_PATH,
	FMAPTOOL_EXIT_BAD_OUTPUT_PATH,
	FMAPTOOL_EXIT_FAILED_DESCRIPTOR,
	FMAPTOOL_EXIT_MISSING_FMAP_SECTION,
	FMAPTOOL_EXIT_MISSING_PRIMARY_CBFS,
	FMAPTOOL_EXIT_FAILED_FMAP_CONVERSION,
	FMAPTOOL_EXIT_UNKNOWN_FMAP_SIZE,
	FMAPTOOL_EXIT_FAILED_WRITING_OUTPUT,
	FMAPTOOL_EXIT_FAILED_WRITING_HEADER,
};

static void usage(const char *invoked_as)
{
	fputs("fmaptool: Compiler for fmd (flashmap descriptor) files\n",
									stderr);
	fputs("\nUSAGE:\n", stderr);
	fprintf(stderr,
		"\t%s [-h <header output file>] [-R <region output file>] <fmd input file> <binary output file>\n",
								invoked_as);
	fputs("\nMANDATORY ARGUMENTS:\n", stderr);
	fprintf(stderr,
		"<fmd input file> may be '%s' to read from standard input\n",
						STDIN_FILENAME_SENTINEL);
	fputs("<binary output file> must be a regular file\n", stderr);
	fputs("\nOPTIONAL SWITCHES:\n", stderr);
	fprintf(stderr,
		"-h\tAlso produce a C header defining %s to the FMAP section's flash offset.\n",
							HEADER_FMAP_OFFSET);
	fprintf(stderr,
		"-R\tAlso produce a text file listing the CBFS regions, comma separated.\n");
	fputs("\nOUTPUT:\n", stderr);
	fputs("A successful invocation prints a summary of work done to standard error, and a comma-separated list\n",
									stderr);
	fputs("of those sections that contain CBFSes, starting with the primary such section, to standard output.\n",
									stderr);
}

static void list_cbfs_section_names(FILE *out)
{
	cbfs_section_iterator_t cbfs_it = cbfs_sections_iterator();
	assert(cbfs_it);

	bool subsequent = false;
	while (cbfs_it) {
		const char *cur_name =
				cbfs_sections_iterator_deref(cbfs_it)->name;
		if (cbfs_sections_iterator_advance(&cbfs_it) && subsequent)
			fputc(',', out);
		fputs(cur_name, out);
		subsequent = true;
	}
	fputc('\n', out);
}

static bool write_header(const char *out_fname,
					const struct flashmap_descriptor *root)
{
	assert(out_fname);

	FILE *header = fopen(out_fname, "w");
	if (!header) {
		fprintf(stderr, "FATAL: Unable to open file '%s' for writing\n",
				out_fname);
		return false;
	}

	unsigned fmap_offset =
			fmd_calc_absolute_offset(root, SECTION_NAME_FMAP);
	assert(fmap_offset != FMD_NOTFOUND);

	fputs("#ifndef FMAPTOOL_GENERATED_HEADER_H_\n", header);
	fputs("#define FMAPTOOL_GENERATED_HEADER_H_\n\n", header);
	fprintf(header, "#define %s %#x\n\n", HEADER_FMAP_OFFSET, fmap_offset);

	/* also add defines for each CBFS-carrying fmap region: base and size */
	cbfs_section_iterator_t cbfs_it = cbfs_sections_iterator();
	while (cbfs_it) {
		const struct flashmap_descriptor *item =
				cbfs_sections_iterator_deref(cbfs_it);
		assert(item->offset_known && item->size_known);
		unsigned abs_base = fmd_calc_absolute_offset(root, item->name);
		fprintf(header, "#define ___FMAP__%s_BASE 0x%x\n",
			item->name, abs_base);
		fprintf(header, "#define ___FMAP__%s_SIZE 0x%x\n",
			item->name, item->size);
		cbfs_sections_iterator_advance(&cbfs_it);
	}

	fputs("#endif\n", header);

	fclose(header);
	return true;
}

static void full_fmd_cleanup(struct flashmap_descriptor **victim)
{
	assert(victim);

	cbfs_sections_cleanup();
	fmd_cleanup(*victim);
	*victim = NULL;
}

int main(int argc, char **argv)
{
	struct {
		// Mandatory
		const char *fmd_filename;
		const char *fmap_filename;

		// Optional
		const char *header_filename;
		const char *region_filename;
	} args = {NULL, NULL, NULL, NULL};

	bool show_usage = false;
	int each_arg;
	while (!show_usage && (each_arg = getopt(argc, argv, ":h:R:")) != -1) {
		switch (each_arg) {
		case 'h':
			args.header_filename = optarg;
			break;
		case 'R':
			args.region_filename = optarg;
			break;
		case ':':
			fprintf(stderr, "-%c: Expected an accompanying value\n",
									optopt);
			show_usage = true;
			break;
		default:
			fprintf(stderr, "-%c: Unexpected command-line switch\n",
									optopt);
			show_usage = true;
		}
	}

	if (show_usage || argc - optind != 2) {
		usage(argv[0]);
		return FMAPTOOL_EXIT_BAD_ARGS;
	}
	args.fmd_filename = argv[optind];
	args.fmap_filename = argv[optind + 1];

	FILE *fmd_file = stdin;
	if (strcmp(args.fmd_filename, STDIN_FILENAME_SENTINEL) != 0) {
		fmd_file = fopen(args.fmd_filename, "r");
		if (!fmd_file) {
			fprintf(stderr, "FATAL: Unable to open file '%s'\n",
							args.fmd_filename);
			return FMAPTOOL_EXIT_BAD_INPUT_PATH;
		}
	}

	struct flashmap_descriptor *descriptor = fmd_create(fmd_file);
	fclose(fmd_file);
	if (!descriptor) {
		fputs("FATAL: Failed while processing provided descriptor\n",
									stderr);
		full_fmd_cleanup(&descriptor);
		return FMAPTOOL_EXIT_FAILED_DESCRIPTOR;
	}

	if (!fmd_find_node(descriptor, SECTION_NAME_FMAP)) {
		fprintf(stderr,
			"FATAL: Flashmap descriptor must have an '%s' section\n",
							SECTION_NAME_FMAP);
		full_fmd_cleanup(&descriptor);
		return FMAPTOOL_EXIT_MISSING_FMAP_SECTION;
	}

	if (!cbfs_sections_primary_cbfs_accounted_for()) {
		fprintf(stderr,
			"FATAL: Flashmap descriptor must have a '%s' section that is annotated with '(%s)'\n",
						SECTION_NAME_PRIMARY_CBFS,
						SECTION_ANNOTATION_CBFS);
		full_fmd_cleanup(&descriptor);
		return FMAPTOOL_EXIT_MISSING_PRIMARY_CBFS;
	}

	struct fmap *flashmap = fmap_from_fmd(descriptor);
	if (!flashmap) {
		fputs("FATAL: Failed while constructing FMAP section\n",
									stderr);
		full_fmd_cleanup(&descriptor);
		return FMAPTOOL_EXIT_FAILED_FMAP_CONVERSION;
	}

	int size = fmap_size(flashmap);
	if (size < 0) {
		fputs("FATAL: Failed to determine FMAP section size\n",
									stderr);
		fmap_destroy(flashmap);
		full_fmd_cleanup(&descriptor);
		return FMAPTOOL_EXIT_UNKNOWN_FMAP_SIZE;
	}

	FILE *fmap_file = fopen(args.fmap_filename, "wb");
	if (!fmap_file) {
		fprintf(stderr, "FATAL: Unable to open file '%s' for writing\n",
							args.fmap_filename);
		fmap_destroy(flashmap);
		full_fmd_cleanup(&descriptor);
		return FMAPTOOL_EXIT_BAD_OUTPUT_PATH;
	}

	if (!fwrite(flashmap, size, 1, fmap_file)) {
		fputs("FATAL: Failed to write final FMAP to file\n", stderr);
		fclose(fmap_file);
		fmap_destroy(flashmap);
		full_fmd_cleanup(&descriptor);
		return FMAPTOOL_EXIT_FAILED_WRITING_OUTPUT;
	}
	fclose(fmap_file);
	fmap_destroy(flashmap);

	if (args.header_filename &&
			!write_header(args.header_filename, descriptor)) {
		full_fmd_cleanup(&descriptor);
		return FMAPTOOL_EXIT_FAILED_WRITING_HEADER;
	}

	fprintf(stderr, "SUCCESS: Wrote %d bytes to file '%s'%s\n", size,
							args.fmap_filename,
			args.header_filename ? " (and generated header)" : "");
	fputs("The sections containing CBFSes are: ", stderr);
	list_cbfs_section_names(stdout);
	if (args.region_filename) {
		FILE *region_file = fopen(args.region_filename, "w");
		if (region_file == NULL)
			return FMAPTOOL_EXIT_FAILED_WRITING_OUTPUT;

		list_cbfs_section_names(region_file);
		fclose(region_file);
	}

	full_fmd_cleanup(&descriptor);
	return FMAPTOOL_EXIT_SUCCESS;
}
