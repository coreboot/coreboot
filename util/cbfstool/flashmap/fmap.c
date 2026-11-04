/* SPDX-License-Identifier: BSD-3-Clause or GPL-2.0-only */

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
#include <commonlib/bsd/sysincludes.h>
#include <commonlib/bsd/helpers.h>

#include "fmap.h"
#include "kv_pair.h"
#include "valstr.h"

const struct valstr flag_lut[] = {
	{ FMAP_AREA_STATIC, "static" },
	{ FMAP_AREA_COMPRESSED, "compressed" },
	{ FMAP_AREA_RO, "ro" },
	{ FMAP_AREA_PRESERVE, "preserve" },
};

/* returns size of fmap data structure if successful, <0 to indicate error */
int fmap_size(const struct fmap *fmap)
{
	if (!fmap)
		return -1;

	return sizeof(*fmap) + (le16toh(fmap->nareas) * sizeof(struct fmap_area));
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
	if (le32toh(fmap->size) <
		sizeof(*fmap) + le16toh(fmap->nareas) * sizeof(struct fmap_area))
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
	unsigned long offset;
	int fmap_found = 0;

	if (len < sizeof(struct fmap))
		return -1;

	for (offset = 0; offset <= len - sizeof(struct fmap); offset++) {
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
static long fmap_bsearch(const uint8_t *image, size_t len)
{
	unsigned long offset;
	int fmap_found = 0, stride;

	if (len < sizeof(struct fmap))
		return -1;

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
		     offset <= len - sizeof(struct fmap);
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

long fmap_find(const uint8_t *image, unsigned int image_len)
{
	long ret = -1;

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
		    (unsigned long long)le64toh(fmap->base));
	kv_pair_fmt(kv, "fmap_size", "0x%04x", le32toh(fmap->size));
	kv_pair_fmt(kv, "fmap_name", "%s", fmap->name);
	kv_pair_fmt(kv, "fmap_nareas", "%d", le16toh(fmap->nareas));
	kv_pair_print(kv);
	kv_pair_free(kv);

	for (i = 0; i < le16toh(fmap->nareas); i++) {
		struct kv_pair *pair;
		uint16_t flags;
		char *str;

		pair = kv_pair_new();
		if (!pair)
			return -1;

		kv_pair_fmt(pair, "area_offset", "0x%08x",
				le32toh(fmap->areas[i].offset));
		kv_pair_fmt(pair, "area_size", "0x%08x",
				le32toh(fmap->areas[i].size));
		kv_pair_fmt(pair, "area_name", "%s",
				fmap->areas[i].name);
		kv_pair_fmt(pair, "area_flags_raw", "0x%02x",
				le16toh(fmap->areas[i].flags));

		/* Print descriptive strings for flags rather than the field */
		flags = le16toh(fmap->areas[i].flags);
		str = fmap_flags_to_string(flags);
		if (str == NULL) {
			kv_pair_free(pair);
			return -1;
		}
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
	fmap->base = htole64(base);
	fmap->size = htole32(size);
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
	if (le16toh((*fmap)->nareas) >= 0xffff)
		return -1;

	orig_size = fmap_size(*fmap);
	new_size = orig_size + sizeof(*area);

	*fmap = realloc(*fmap, new_size);
	if (*fmap == NULL)
		return -1;

	area = (struct fmap_area *)((uint8_t *)*fmap + orig_size);
	memset(area, 0, sizeof(*area));
	memccpy(&area->name, name, '\0', FMAP_STRLEN);
	area->offset = htole32(offset);
	area->size   = htole32(size);
	area->flags  = htole16(flags);

	(*fmap)->nareas = htole16(le16toh((*fmap)->nareas) + 1);
	return new_size;
}

const struct fmap_area *fmap_find_area(const struct fmap *fmap,
							const char *name)
{
	int i;
	const struct fmap_area *area = NULL;

	if (!fmap || !name)
		return NULL;

	for (i = 0; i < le16toh(fmap->nareas); i++) {
		if (!strcmp((const char *)fmap->areas[i].name, name)) {
			area = &fmap->areas[i];
			break;
		}
	}

	return area;
}
