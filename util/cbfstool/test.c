/* fmap_from_fmd.c, simple launcher for fmap library unit test suite */
/* SPDX-License-Identifier: GPL-2.0-only */

#include "flashmap/fmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commonlib/bsd/helpers.h>

static enum test_status { pass = EXIT_SUCCESS, fail = EXIT_FAILURE } status;

static struct fmap *fmap_create_test(void)
{
	struct fmap *fmap;
	uint64_t base = 0;
	uint32_t size = 0x100000;
	char name[] = "test_fmap";

	status = fail;

	fmap = fmap_create(base, size, (uint8_t *)name);
	if (!fmap)
		return NULL;

	if (memcmp(&fmap->signature, FMAP_SIGNATURE, strlen(FMAP_SIGNATURE))) {
		printf("FAILURE: signature is incorrect\n");
		goto fmap_create_test_exit;
	}

	if ((fmap->ver_major != FMAP_VER_MAJOR) ||
	    (fmap->ver_minor != FMAP_VER_MINOR)) {
		printf("FAILURE: version is incorrect\n");
		goto fmap_create_test_exit;
	}

	if (le64toh(fmap->base) != base) {
		printf("FAILURE: base is incorrect\n");
		goto fmap_create_test_exit;
	}

	if (le32toh(fmap->size) != 0x100000) {
		printf("FAILURE: size is incorrect\n");
		goto fmap_create_test_exit;
	}

	if (strcmp((char *)fmap->name, "test_fmap")) {
		printf("FAILURE: name is incorrect\n");
		goto fmap_create_test_exit;
	}

	if (le16toh(fmap->nareas) != 0) {
		printf("FAILURE: number of areas is incorrect\n");
		goto fmap_create_test_exit;
	}

	status = pass;
fmap_create_test_exit:
	/* preserve fmap if all went well */
	if (status == fail) {
		fmap_destroy(fmap);
		fmap = NULL;
	}
	return fmap;
}

static int fmap_print_test(struct fmap *fmap)
{
	return fmap_print(fmap);
}

static int fmap_size_test(void)
{
	status = fail;

	if (fmap_size(NULL) >= 0) {
		printf("FAILURE: failed to abort on NULL pointer input\n");
		goto fmap_size_test_exit;
	}

	status = pass;
fmap_size_test_exit:
	return status;
}

/* this test re-allocates the fmap, so it gets a double-pointer */
static int fmap_append_area_test(struct fmap **fmap)
{
	int total_size;
	uint16_t nareas_orig;
	/* test_area will be used by fmap_csum_test and find_area_test */
	struct fmap_area test_area = {
		.offset = htole32(0x400),
		.size = htole32(0x10000),
		.name = "test_area_1",
		.flags = htole16(FMAP_AREA_STATIC),
	};

	status = fail;

	if ((fmap_append_area(NULL, 0, 0, test_area.name, 0) >= 0) ||
	    (fmap_append_area(fmap, 0, 0, NULL, 0) >= 0)) {
		printf("FAILURE: failed to abort on NULL pointer input\n");
		goto fmap_append_area_test_exit;
	}

	nareas_orig = le16toh((*fmap)->nareas);
	(*fmap)->nareas = htole16(~(0));
	if (fmap_append_area(fmap, 0, 0, (const uint8_t *)"foo", 0) >= 0) {
		printf("FAILURE: failed to abort with too many areas\n");
		goto fmap_append_area_test_exit;
	}
	(*fmap)->nareas = htole16(nareas_orig);

	total_size = sizeof(**fmap) + sizeof(test_area);
	if (fmap_append_area(fmap,
			     le32toh(test_area.offset),
			     le32toh(test_area.size),
			     test_area.name,
			     le16toh(test_area.flags)
			     ) != total_size) {
		printf("failed to append area\n");
		goto fmap_append_area_test_exit;
	}

	if (le16toh((*fmap)->nareas) != 1) {
		printf("FAILURE: failed to increment number of areas\n");
		goto fmap_append_area_test_exit;
	}

	status = pass;
fmap_append_area_test_exit:
	return status;
}

static int fmap_find_area_test(struct fmap *fmap)
{
	status = fail;
	char area_name[] = "test_area_1";

	if (fmap_find_area(NULL, area_name) ||
	    fmap_find_area(fmap, NULL)) {
		printf("FAILURE: failed to abort on NULL pointer input\n");
		goto fmap_find_area_test_exit;
	}

	if (fmap_find_area(fmap, area_name) == NULL) {
		printf("FAILURE: failed to find \"%s\"\n", area_name);
		goto fmap_find_area_test_exit;
	}

	status = pass;
fmap_find_area_test_exit:
	return status;
}

static int fmap_flags_to_string_test(void)
{
	char *str = NULL;
	char *my_str = NULL;
	unsigned int i;
	uint16_t flags;

	status = fail;

	/* no area flag */
	str = fmap_flags_to_string(0);
	if (!str || strcmp(str, "")) {
		printf("FAILURE: failed to return empty string when no flag"
		       "are set");
		goto fmap_flags_to_string_test_exit;
	}
	free(str);

	/* single area flags */
	for (i = 0; i < ARRAY_SIZE(flag_lut); i++) {
		if (!flag_lut[i].str)
			continue;

		if ((str = fmap_flags_to_string(flag_lut[i].val)) == NULL) {
			printf("FAILURE: failed to translate flag to string");
			goto fmap_flags_to_string_test_exit;
		}
		free(str);
	}

	/* construct our own flags field and string using all available flags
	 * and compare output with fmap_flags_to_string() */
	my_str = calloc(256, 1);
	flags = 0;
	for (i = 0; i < ARRAY_SIZE(flag_lut); i++) {
		if (!flag_lut[i].str)
			continue;
		else if (i > 0)
			strcat(my_str, ",");

		flags |= flag_lut[i].val;
		strcat(my_str, flag_lut[i].str);
	}

	str = fmap_flags_to_string(flags);
	if (strcmp(str, my_str)) {
		printf("FAILURE: bad result from fmap_flags_to_string\n");
		goto fmap_flags_to_string_test_exit;
	}

	status = pass;
fmap_flags_to_string_test_exit:
	free(str);
	free(my_str);
	return status;

}

static int fmap_find_test(struct fmap *fmap)
{
	uint8_t *buf;
	size_t total_size, offset;

	status = fail;

	/*
	 * Note: In these tests, we'll use fmap_find() and control usage of
	 * lsearch and bsearch by using a power-of-2 total_size. For lsearch,
	 * use total_size - 1. For bsearch, use total_size.
	 */

	total_size = 0x100000;
	buf = calloc(total_size, 1);

	/* test if image length is zero */
	if (fmap_find(buf, 0) >= 0) {
		printf("FAILURE: failed to abort on zero-length image\n");
		goto fmap_find_test_exit;
	}

	/* test if no fmap exists */
	if (fmap_find(buf, total_size - 1) >= 0) {
		printf("FAILURE: lsearch returned false positive\n");
		goto fmap_find_test_exit;
	}
	if (fmap_find(buf, total_size) >= 0) {
		printf("FAILURE: bsearch returned false positive\n");
		goto fmap_find_test_exit;
	}

	/* bsearch only probes 16-byte-aligned offsets; lsearch scans every byte */
	offset = (total_size / 2) + 1;
	memcpy(&buf[offset], fmap, fmap_size(fmap));

	if (fmap_find(buf, total_size - 1) != (long)offset) {
		printf("FAILURE: lsearch failed to find unaligned fmap\n");
		goto fmap_find_test_exit;
	}
	if (fmap_find(buf, total_size) >= 0) {
		printf("FAILURE: bsearch matched unaligned fmap\n");
		goto fmap_find_test_exit;
	}

	/* 16-byte-aligned fmap: bsearch must find it */
	offset = (total_size / 2) + 16;
	memset(buf, 0, total_size);
	memcpy(&buf[offset], fmap, fmap_size(fmap));

	if (fmap_find(buf, total_size) != (long)offset) {
		printf("FAILURE: bsearch failed to find fmap\n");
		goto fmap_find_test_exit;
	}

	/* test bsearch if offset is at 0 */
	offset = 0;
	memset(buf, 0, total_size);
	memcpy(buf, fmap, fmap_size(fmap));
	if (fmap_find(buf, total_size) != (long)offset) {
		printf("FAILURE: bsearch failed to find fmap at offset 0\n");
		goto fmap_find_test_exit;
	}

	/* test overrun detection */
	memset(buf, 0, total_size);
	memcpy(&buf[total_size - fmap_size(fmap) + 1],
	       fmap,
	       fmap_size(fmap) - 1);
	if (fmap_find(buf, total_size - 1) >= 0) {
		printf("FAILURE: lsearch failed to catch overrun\n");
		goto fmap_find_test_exit;
	}
	if (fmap_find(buf, total_size) >= 0) {
		printf("FAILURE: bsearch failed to catch overrun\n");
		goto fmap_find_test_exit;
	}

	status = pass;
fmap_find_test_exit:
	free(buf);
	return status;
}

int fmap_test(void)
{
	int rc = EXIT_SUCCESS;
	struct fmap *my_fmap;

	/*
	 * This test has two parts: Creation of an fmap with one or more
	 * area(s), and other stuff. Since a valid fmap is required to run
	 * many tests, we abort if fmap creation fails in any way.
	 *
	 * Also, fmap_csum_test() makes some assumptions based on the areas
	 * appended. See fmap_append_area_test() for details.
	 */
	if ((my_fmap = fmap_create_test()) == NULL) {
		rc = EXIT_FAILURE;
		goto fmap_test_exit;
	}

	if (fmap_find_test(my_fmap)) {
		rc = EXIT_FAILURE;
		goto fmap_test_exit;
	}

	if (fmap_append_area_test(&my_fmap)) {
		rc = EXIT_FAILURE;
		goto fmap_test_exit;
	}

	rc |= fmap_find_area_test(my_fmap);
	rc |= fmap_size_test();
	rc |= fmap_flags_to_string_test();
	rc |= fmap_print_test(my_fmap);

fmap_test_exit:
	fmap_destroy(my_fmap);
	if (rc)
		printf("FAILED\n");
	return rc;
}

int main(void)
{
	int result = fmap_test();

	puts("");
	puts("===");
	puts("");
	if (!result)
		puts("RESULT: All unit tests PASSED.");
	else
		puts("RESULT: One or more tests FAILED!");

	return result;
}
