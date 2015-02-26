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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include "common.h"
#include "fmap_from_fmd.h"

#include <stdio.h>
#include <string.h>

#define STDIN_FILENAME_SENTINEL "-"

enum fmaptool_return {
	FMAPTOOL_EXIT_SUCCESS = 0,
	FMAPTOOL_EXIT_BAD_ARGS,
	FMAPTOOL_EXIT_BAD_INPUT_PATH,
	FMAPTOOL_EXIT_BAD_OUTPUT_PATH,
	FMAPTOOL_EXIT_FAILED_DESCRIPTOR,
	FMAPTOOL_EXIT_FAILED_FMAP_CONVERSION,
	FMAPTOOL_EXIT_UNKNOWN_FMAP_SIZE,
	FMAPTOOL_EXIT_FAILED_WRITING_FILE,
};

bool fmd_process_annotation_impl(unused const struct flashmap_descriptor *node,
						unused const char *annotation)
{
	// We always accept annotations, but never act on them.
	return true;
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		fputs("Convert a human-readable flashmap descriptor (fmd) file into the binary FMAP format for use in firmware images\n",
									stderr);
		fprintf(stderr,
			"USAGE: %s <fmd input file> <binary output file>\n",
								argv[0]);
		fprintf(stderr,
			"To read from standard input, provide '%s' as the input filename.\n",
					STDIN_FILENAME_SENTINEL);
		return FMAPTOOL_EXIT_BAD_ARGS;
	}
	const char *fmd_filename = argv[1];
	const char *fmap_filename = argv[2];

	FILE *fmd_file = stdin;
	if (strcmp(fmd_filename, STDIN_FILENAME_SENTINEL) != 0) {
		fmd_file = fopen(fmd_filename, "r");
		if (!fmd_file) {
			fprintf(stderr, "FATAL: Unable to open file '%s'\n",
								fmd_filename);
			return FMAPTOOL_EXIT_BAD_INPUT_PATH;
		}
	}

	struct flashmap_descriptor *descriptor = fmd_create(fmd_file);
	fclose(fmd_file);
	if (!descriptor) {
		fputs("FATAL: Failed while processing provided descriptor\n",
									stderr);
		return FMAPTOOL_EXIT_FAILED_DESCRIPTOR;
	}

	struct fmap *flashmap = fmap_from_fmd(descriptor);
	if (!flashmap) {
		fputs("FATAL: Failed while constructing FMAP section\n",
									stderr);
		fmd_cleanup(descriptor);
		return FMAPTOOL_EXIT_FAILED_FMAP_CONVERSION;
	}

	int size = fmap_size(flashmap);
	if (size < 0) {
		fputs("FATAL: Failed to determine FMAP section size\n",
									stderr);
		fmap_destroy(flashmap);
		fmd_cleanup(descriptor);
		return FMAPTOOL_EXIT_UNKNOWN_FMAP_SIZE;
	}

	FILE *fmap_file = fopen(fmap_filename, "wb");
	if (!fmap_file) {
		fprintf(stderr, "FATAL: Unable to open file '%s' for writing\n",
								fmap_filename);
		fmap_destroy(flashmap);
		fmd_cleanup(descriptor);
		return FMAPTOOL_EXIT_BAD_OUTPUT_PATH;
	}

	if (!fwrite(flashmap, size, 1, fmap_file)) {
		fputs("FATAL: Failed to write final FMAP to file\n", stderr);
		fclose(fmap_file);
		fmap_destroy(flashmap);
		fmd_cleanup(descriptor);
		return FMAPTOOL_EXIT_FAILED_WRITING_FILE;
	}
	fclose(fmap_file);
	printf("SUCCESS: Wrote %d bytes to file '%s'\n", size, fmap_filename);

	fmap_destroy(flashmap);
	fmd_cleanup(descriptor);
	return FMAPTOOL_EXIT_SUCCESS;
}
