/*
 * Copyright 2015, Google Inc.
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

#ifndef FLASHMAP_LIB_FMAP_H__
#define FLASHMAP_LIB_FMAP_H__

#include <inttypes.h>

#include <valstr.h>

#define FMAP_SIGNATURE		"__FMAP__"
#define FMAP_VER_MAJOR		1	/* this header's FMAP minor version */
#define FMAP_VER_MINOR		1	/* this header's FMAP minor version */
#define FMAP_STRLEN		32	/* maximum length for strings, */
					/* including null-terminator */
extern const struct valstr flag_lut[16];
enum fmap_flags {
	FMAP_AREA_STATIC	= 1 << 0,
	FMAP_AREA_COMPRESSED	= 1 << 1,
	FMAP_AREA_RO		= 1 << 2,
};

/* Mapping of volatile and static regions in firmware binary */
struct fmap_area {
	uint32_t offset;		/* offset relative to base */
	uint32_t size;			/* size in bytes */
	uint8_t  name[FMAP_STRLEN];	/* descriptive name */
	uint16_t flags;			/* flags for this area */
}  __attribute__((packed));

struct fmap {
	uint8_t  signature[8];		/* "__FMAP__" (0x5F5F464D41505F5F) */
	uint8_t  ver_major;		/* major version */
	uint8_t  ver_minor;		/* minor version */
	uint64_t base;			/* address of the firmware binary */
	uint32_t size;			/* size of firmware binary in bytes */
	uint8_t  name[FMAP_STRLEN];	/* name of this firmware binary */
	uint16_t nareas;		/* number of areas described by
					   fmap_areas[] below */
	struct fmap_area areas[];
} __attribute__((packed));

/*
 * fmap_find - find FMAP signature in a binary image
 *
 * @image:	binary image
 * @len:	length of binary image
 *
 * This function does no error checking. The caller is responsible for
 * verifying that the contents are sane.
 *
 * returns offset of FMAP signature to indicate success
 * returns <0 to indicate failure
 */
extern long int fmap_find(const uint8_t *image, unsigned int len);

/*
 * fmap_print - Print contents of flash map data structure
 *
 * @map:	raw map data
 *
 * returns 0 to indiciate success
 * returns <0 to indicate failure
 */
extern int fmap_print(const struct fmap *map);

/*
 * fmap_flags_to_string - convert raw flags field into user-friendly string
 *
 * @flags:	raw flags
 *
 * This function returns a user-friendly comma-separated list of fmap area
 * flags. If there are no flags (flags == 0), the string will contain only
 * a terminating character ('\0')
 *
 * This function allocates memory which the caller must free.
 *
 * returns pointer to an allocated string if successful
 * returns NULL to indicate failure
 */
char *fmap_flags_to_string(uint16_t flags);

/*
 * fmap_create - allocate and initialize a new fmap structure
 *
 * @base:	base address of firmware within address space
 * @size:	size of the firmware (bytes)
 * @name:	name of firmware
 *
 * This function will allocate a flashmap header. Members of the structure
 * which are not passed in are automatically initialized.
 *
 * returns pointer to newly allocated flashmap header if successful
 * returns NULL to indicate failure
 */
extern struct fmap *fmap_create(uint64_t base,
				uint32_t size, uint8_t *name);

/* free memory used by an fmap structure */
extern void fmap_destroy(struct fmap *fmap);

/*
 * fmap_size - returns size of fmap data structure (including areas)
 *
 * @fmap:	fmap
 *
 * returns size of fmap structure if successful
 * returns <0 to indicate failure
 */
extern int fmap_size(const struct fmap *fmap);

/*
 * fmap_append_area - realloc an existing flashmap and append an area
 *
 * @fmap:	double pointer to existing flashmap
 * @offset:	offset of area
 * @size:	size of area
 * @name:	name of area
 * @flags:	area flags
 *
 * returns total size of reallocated flashmap structure if successful
 * returns <0 to indicate failure
 */
extern int fmap_append_area(struct fmap **fmap,
			    uint32_t offset, uint32_t size,
			    const uint8_t *name, uint16_t flags);

/*
 * fmap_find_area - find an fmap_area entry (by name) and return pointer to it
 *
 * @fmap:	fmap structure to parse
 * @name:	name of area to find
 *
 * returns a pointer to the entry in the fmap structure if successful
 * returns NULL to indicate failure or if no matching area entry is found
 */
extern const struct fmap_area *fmap_find_area(const struct fmap *fmap,
							const char *name);

/* unit testing stuff */
extern int fmap_test(void);

#endif	/* FLASHMAP_LIB_FMAP_H__*/
