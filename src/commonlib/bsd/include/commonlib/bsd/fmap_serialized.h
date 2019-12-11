/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

#ifndef FLASHMAP_SERIALIZED_H__
#define FLASHMAP_SERIALIZED_H__

#include <stdint.h>

#define FMAP_SIGNATURE		"__FMAP__"
#define FMAP_VER_MAJOR		1	/* this header's FMAP minor version */
#define FMAP_VER_MINOR		1	/* this header's FMAP minor version */
#define FMAP_STRLEN		32	/* maximum length for strings, */
					/* including null-terminator */

enum fmap_flags {
	FMAP_AREA_STATIC	= 1 << 0,
	FMAP_AREA_COMPRESSED	= 1 << 1,
	FMAP_AREA_RO		= 1 << 2,
	FMAP_AREA_PRESERVE	= 1 << 3,
};

/* Mapping of volatile and static regions in firmware binary */
struct fmap_area {
	uint32_t offset;                /* offset relative to base */
	uint32_t size;                  /* size in bytes */
	uint8_t  name[FMAP_STRLEN];     /* descriptive name */
	uint16_t flags;                 /* flags for this area */
}  __packed;

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
} __packed;

#endif	/* FLASHMAP_SERIALIZED_H__ */
