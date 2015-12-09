/* Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *    * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#define _XOPEN_SOURCE 700

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>

#include <fmap.h>
#include <valstr.h>

#include "kv_pair.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

const struct valstr flag_lut[] = {
	{ FMAP_AREA_STATIC, "static" },
	{ FMAP_AREA_COMPRESSED, "compressed" },
	{ FMAP_AREA_RO, "ro" },
};

/* returns size of fmap data structure if successful, <0 to indicate error */
int fmap_size(const struct fmap *fmap)
{
	if (!fmap)
		return -1;

	return sizeof(*fmap) + (fmap->nareas * sizeof(struct fmap_area));
}

/* Make a best-effort assessment if the given fmap is real */
static int is_valid_fmap(const struct fmap *fmap)
{
	if (memcmp(fmap, FMAP_SIGNATURE, strlen(FMAP_SIGNATURE)) != 0)
		return 0;
	/* strings containing the magic tend to fail here */
	if (fmap->ver_major != FMAP_VER_MAJOR)
		return 0;
	/* a basic consistency check: flash should be larger than fmap */
	if (fmap->size <
		sizeof(*fmap) + fmap->nareas * sizeof(struct fmap_area))
		return 0;

	/* fmap-alikes along binary data tend to fail on having a valid,
	 * null-terminated string in the name field.*/
	int i = 0;
	while (i < FMAP_STRLEN) {
		if (fmap->name[i] == 0)
			break;
		if (!isgraph(fmap->name[i]))
			return 0;
		if (i == FMAP_STRLEN - 1) {
			/* name is specified to be null terminated single-word string
			 * without spaces. We did not break in the 0 test, we know it
			 * is a printable spaceless string but we're seeing FMAP_STRLEN
			 * symbols, which is one too many.
			 */
			 return 0;
		}
		i++;
	}
	return 1;

}

/* brute force linear search */
static long int fmap_lsearch(const uint8_t *image, size_t len)
{
	unsigned long int offset;
	int fmap_found = 0;

	for (offset = 0; offset < len - strlen(FMAP_SIGNATURE); offset++) {
		if (is_valid_fmap((const struct fmap *)&image[offset])) {
			fmap_found = 1;
			break;
		}
	}

	if (!fmap_found)
		return -1;

	if (offset + fmap_size((const struct fmap *)&image[offset]) > len)
		return -1;

	return offset;
}

/* if image length is a power of 2, use binary search */
static long int fmap_bsearch(const uint8_t *image, size_t len)
{
	unsigned long int offset = -1;
	int fmap_found = 0, stride;

	/*
	 * For efficient operation, we start with the largest stride possible
	 * and then decrease the stride on each iteration. Also, check for a
	 * remainder when modding the offset with the previous stride. This
	 * makes it so that each offset is only checked once.
	 */
	for (stride = len / 2; stride >= 16; stride /= 2) {
		if (fmap_found)
			break;

		for (offset = 0;
		     offset < len - strlen(FMAP_SIGNATURE);
		     offset += stride) {
			if ((offset % (stride * 2) == 0) && (offset != 0))
					continue;
			if (is_valid_fmap(
				(const struct fmap *)&image[offset])) {
				fmap_found = 1;
				break;
			}
		}
	}

	if (!fmap_found)
		return -1;

	if (offset + fmap_size((const struct fmap *)&image[offset]) > len)
		return -1;

	return offset;
}

static int popcnt(unsigned int u)
{
	int count;

	/* K&R method */
	for (count = 0; u; count++)
		u &= (u - 1);

	return count;
}

long int fmap_find(const uint8_t *image, unsigned int image_len)
{
	long int ret = -1;

	if ((image == NULL) || (image_len == 0))
		return -1;

	if (popcnt(image_len) == 1)
		ret = fmap_bsearch(image, image_len);
	else
		ret = fmap_lsearch(image, image_len);

	return ret;
}

int fmap_print(const struct fmap *fmap)
{
	int i;
	struct kv_pair *kv = NULL;
	const uint8_t *tmp;

	kv = kv_pair_new();
	if (!kv)
		return -1;

	tmp = fmap->signature;
	kv_pair_fmt(kv, "fmap_signature",
			"0x%02x%02x%02x%02x%02x%02x%02x%02x",
			tmp[0], tmp[1], tmp[2], tmp[3],
			tmp[4], tmp[5], tmp[6], tmp[7]);
	kv_pair_fmt(kv, "fmap_ver_major", "%d", fmap->ver_major);
	kv_pair_fmt(kv, "fmap_ver_minor","%d", fmap->ver_minor);
	kv_pair_fmt(kv, "fmap_base", "0x%016llx",
		    (unsigned long long)fmap->base);
	kv_pair_fmt(kv, "fmap_size", "0x%04x", fmap->size);
	kv_pair_fmt(kv, "fmap_name", "%s", fmap->name);
	kv_pair_fmt(kv, "fmap_nareas", "%d", fmap->nareas);
	kv_pair_print(kv);
	kv_pair_free(kv);

	for (i = 0; i < fmap->nareas; i++) {
		struct kv_pair *pair;
		uint16_t flags;
		char *str;

		pair = kv_pair_new();
		if (!pair)
			return -1;

		kv_pair_fmt(pair, "area_offset", "0x%08x",
				fmap->areas[i].offset);
		kv_pair_fmt(pair, "area_size", "0x%08x",
				fmap->areas[i].size);
		kv_pair_fmt(pair, "area_name", "%s",
				fmap->areas[i].name);
		kv_pair_fmt(pair, "area_flags_raw", "0x%02x",
				fmap->areas[i].flags);

		/* Print descriptive strings for flags rather than the field */
		flags = fmap->areas[i].flags;
		if ((str = fmap_flags_to_string(flags)) == NULL)
			return -1;
		kv_pair_fmt(pair, "area_flags", "%s", str);
		free(str);

		kv_pair_print(pair);
		kv_pair_free(pair);
	}

	return 0;
}

/* convert raw flags field to user-friendly string */
char *fmap_flags_to_string(uint16_t flags)
{
	char *str = NULL;
	unsigned int i, total_size;

	str = malloc(1);
	str[0] = '\0';
	total_size = 1;

	for (i = 0; i < sizeof(flags) * CHAR_BIT; i++) {
		if (!flags)
			break;

		if (flags & (1 << i)) {
			const char *tmp = val2str(1 << i, flag_lut);

			total_size += strlen(tmp);
			str = realloc(str, total_size);
			strcat(str, tmp);

			flags &= ~(1 << i);
			if (flags) {
				total_size++;
				str = realloc(str, total_size);
				strcat(str, ",");
			}
		}
	}

	return str;
}

/* allocate and initialize a new fmap structure */
struct fmap *fmap_create(uint64_t base, uint32_t size, uint8_t *name)
{
	struct fmap *fmap;

	fmap = malloc(sizeof(*fmap));
	if (!fmap)
		return NULL;

	memset(fmap, 0, sizeof(*fmap));
	memcpy(&fmap->signature, FMAP_SIGNATURE, strlen(FMAP_SIGNATURE));
	fmap->ver_major = FMAP_VER_MAJOR;
	fmap->ver_minor = FMAP_VER_MINOR;
	fmap->base = base;
	fmap->size = size;
	memccpy(&fmap->name, name, '\0', FMAP_STRLEN);

	return fmap;
}

/* free memory used by an fmap structure */
void fmap_destroy(struct fmap *fmap) {
	free(fmap);
}

/* append area to existing structure, return new total size if successful */
int fmap_append_area(struct fmap **fmap,
		     uint32_t offset, uint32_t size,
		     const uint8_t *name, uint16_t flags)
{
	struct fmap_area *area;
	int orig_size, new_size;

	if ((fmap == NULL || *fmap == NULL) || (name == NULL))
		return -1;

	/* too many areas */
	if ((*fmap)->nareas >= 0xffff)
		return -1;

	orig_size = fmap_size(*fmap);
	new_size = orig_size + sizeof(*area);

	*fmap = realloc(*fmap, new_size);
	if (*fmap == NULL)
		return -1;

	area = (struct fmap_area *)((uint8_t *)*fmap + orig_size);
	memset(area, 0, sizeof(*area));
	memcpy(&area->offset, &offset, sizeof(area->offset));
	memcpy(&area->size, &size, sizeof(area->size));
	memccpy(&area->name, name, '\0', FMAP_STRLEN);
	memcpy(&area->flags, &flags, sizeof(area->flags));

	(*fmap)->nareas++;
	return new_size;
}

const struct fmap_area *fmap_find_area(const struct fmap *fmap,
							const char *name)
{
	int i;
	const struct fmap_area *area = NULL;

	if (!fmap || !name)
		return NULL;

	for (i = 0; i < fmap->nareas; i++) {
		if (!strcmp((const char *)fmap->areas[i].name, name)) {
			area = &fmap->areas[i];
			break;
		}
	}

	return area;
}

/*
 * LCOV_EXCL_START
 * Unit testing stuff done here so we do not need to expose static functions.
 */
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

	if (fmap->base != base) {
		printf("FAILURE: base is incorrect\n");
		goto fmap_create_test_exit;
	}

	if (fmap->size != 0x100000) {
		printf("FAILURE: size is incorrect\n");
		goto fmap_create_test_exit;
	}

	if (strcmp((char *)fmap->name, "test_fmap")) {
		printf("FAILURE: name is incorrect\n");
		goto fmap_create_test_exit;
	}

	if (fmap->nareas != 0) {
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
		.offset = 0x400,
		.size = 0x10000,
		.name = "test_area_1",
		.flags = FMAP_AREA_STATIC,
	};

	status = fail;

	if ((fmap_append_area(NULL, 0, 0, test_area.name, 0) >= 0) ||
	    (fmap_append_area(fmap, 0, 0, NULL, 0) >= 0)) {
		printf("FAILURE: failed to abort on NULL pointer input\n");
		goto fmap_append_area_test_exit;
	}

	nareas_orig = (*fmap)->nareas;
	(*fmap)->nareas = ~(0);
	if (fmap_append_area(fmap, 0, 0, (const uint8_t *)"foo", 0) >= 0) {
		printf("FAILURE: failed to abort with too many areas\n");
		goto fmap_append_area_test_exit;
	}
	(*fmap)->nareas = nareas_orig;

	total_size = sizeof(**fmap) + sizeof(test_area);
	if (fmap_append_area(fmap,
			     test_area.offset,
			     test_area.size,
			     test_area.name,
			     test_area.flags
			     ) != total_size) {
		printf("failed to append area\n");
		goto fmap_append_area_test_exit;
	}

	if ((*fmap)->nareas != 1) {
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
	char *str, *my_str;
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
	free(my_str);
	free(str);

	status = pass;
fmap_flags_to_string_test_exit:
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

	/* simple test case: fmap at (total_size / 2) + 1 */
	offset = (total_size / 2) + 1;
	memcpy(&buf[offset], fmap, fmap_size(fmap));

	if ((unsigned)fmap_find(buf, total_size - 1) != offset) {
		printf("FAILURE: lsearch failed to find fmap\n");
		goto fmap_find_test_exit;
	}
	if ((unsigned)fmap_find(buf, total_size) != offset) {
		printf("FAILURE: bsearch failed to find fmap\n");
		goto fmap_find_test_exit;
	}

	/* test bsearch if offset is at 0 */
	offset = 0;
	memset(buf, 0, total_size);
	memcpy(buf, fmap, fmap_size(fmap));
	if ((unsigned)fmap_find(buf, total_size) != offset) {
		printf("FAILURE: bsearch failed to find fmap at offset 0\n");
		goto fmap_find_test_exit;
	}

	/* test overrun detection */
	memset(buf, 0, total_size);
	memcpy(&buf[total_size - fmap_size(fmap) + 1],
	       fmap,
	       fmap_size(fmap) + 1);
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
/* LCOV_EXCL_STOP */
